#include "core/game_engine.h"

namespace dig_dug {


GameEngine::GameEngine(const vector<vector<TileType>>& initial_game_state, size_t tile_size) {
  size_t center_coord = (initial_game_state.size() / 2) * tile_size;
  player_ = Player({center_coord, center_coord});

  game_map_ = initial_game_state;
  for (size_t x = 0; x < game_map_.size(); x++) {
    for (size_t y = 0; y < game_map_.size(); y++) {
      if (game_map_[x][y] == TileType::Pooka || game_map_[x][y] == TileType::Fygar) {
        vec2 position {x * tile_size, y * tile_size};

        vec2 velocity;
        if (game_map_[x + 1][y] == TileType::Tunnel) {
          velocity = {kSpeed, 0};
        } else if (game_map_[x][y + 1] == TileType::Tunnel) {
          velocity = {0, kSpeed};
        }

        Enemy enemy (position, velocity, game_map_[x][y]);
        enemies_.push_back(enemy);
        game_map_[x][y] = TileType::Tunnel;
      }

      if (x == initial_game_state.size() / 2 + 1 && y <= initial_game_state.size() / 2 + 1) {
        game_map_[x][y] = TileType::Tunnel;
      }
    }
  }

  enemy_ghost_percentage_ = enemies_.size() * kEnemyDifficulty;
  tile_size_ = tile_size;
  max_harpoon_traveling_frames_ = tile_size * kHarpoonLength / (size_t) (kSpeed);
}

void GameEngine::MoveEnemies() {
  if (rand() % tile_size_ < enemy_ghost_percentage_) {
    size_t ghost_index = rand() % enemies_.size();
    enemies_[ghost_index].SetIsGhost();
  }

  for (size_t index = 0; index < enemies_.size(); index++) {
    Enemy& cur_enemy = enemies_[index];

    if (!cur_enemy.GetIsHurt()) {
      if (cur_enemy.GetIsGhost()) {
        MoveGhostedEnemy(cur_enemy);
      
      } else {
        MoveWalkingEnemy(cur_enemy);
      }

      cur_enemy.Move();
    }
  }
}

void GameEngine::MovePlayer(const vec2& velocity) {
  // Resets all attack fields because no enemy is being attacked if the player is moving
  cur_attack_frames_ = 0;
  is_attacking_ = false;
  
  for (size_t index = 0; index < enemies_.size(); index++) {
    Enemy& enemy = enemies_[index];
    enemy.SetIsHurt(false);
  }

  const vec2 kZeroVelocity {0, 0};
  vec2 position = player_.GetPosition();
  vec2 velocity_with_speed {velocity.x * kSpeed, velocity.y * kSpeed};
  vec2 opposite_velocity {velocity_with_speed.x * -1, velocity_with_speed.y * -1};
  vec2 player_prev_speed = player_.GetPrevVelocity();
  bool is_next_tile_open = IsNextTileOpen(velocity_with_speed, player_.GetPosition());

  // Moves according to the given velocity when it is in the same axis as the previous velocity
  // OR the player is aligned with the tiles
  if (is_next_tile_open) {
    if ((size_t) (position.x) % 100 == 0 && (size_t) (position.y) % 100 == 0) {
      if ((velocity_with_speed == player_prev_speed || velocity_with_speed == kZeroVelocity)
          && delayed_turn_velocity_ != kZeroVelocity) {
        player_.Move(delayed_turn_velocity_);
        delayed_turn_velocity_ = {0, 0};
        DigUpTiles(player_.GetPosition(), delayed_turn_velocity_);

      } else {
        player_.Move(velocity_with_speed);
        DigUpTiles(player_.GetPosition(), velocity_with_speed);
      }

    } else if (velocity_with_speed == player_prev_speed || opposite_velocity == player_prev_speed) {
      player_.Move(velocity_with_speed);
      DigUpTiles(player_.GetPosition(), velocity_with_speed);

    } else {
      player_.Move(player_prev_speed);
      DigUpTiles(player_.GetPosition(), velocity_with_speed);
      delayed_turn_velocity_ = velocity_with_speed;
    }
  }
}

bool GameEngine::CheckPlayerDeath() {
  for (Enemy enemy : enemies_) {
    vec2 enemy_pos = enemy.GetPosition();
    vec2 player_pos = player_.GetPosition();
    double distance = glm::length(enemy_pos - player_pos);

    if (!enemy.GetIsGhost() && distance < tile_size_) {
      lives_--;
      return true;
    }
  }

  return false;
}

void GameEngine::AttackEnemy() {
  if (!is_attacking_) {
    CreateHarpoon();
    is_attacking_ = true;

  }

  int hurt_enemy_index = GetHurtEnemy();

  if (hurt_enemy_index > -1) {
    cur_attack_frames_++;
    enemies_[hurt_enemy_index].SetIsHurt(true);

    if (cur_attack_frames_ >= kAttackFrames) {
      enemies_.erase(enemies_.begin() + hurt_enemy_index);
      cur_attack_frames_ = 0;
      is_attacking_ = false;
    }

  } else if (harpoon_.GetDistanceTraveled() >= max_harpoon_traveling_frames_) {
    is_attacking_ = false;

  } else {
    harpoon_.Move();
  }
}

vector<vector<TileType>> GameEngine::GetGameMap() {
  return game_map_;
}

Player GameEngine::GetPlayer() {
  return player_;
}

vector<Enemy> GameEngine::GetEnemies() {
  return enemies_;
}

size_t GameEngine::GetLives() {
  return lives_;
}

bool GameEngine::GetIsAttacking() {
  return is_attacking_;
}

void GameEngine::MoveWalkingEnemy(Enemy& enemy) {
  vec2 cur_position = enemy.GetPosition();
  vec2 cur_velocity = enemy.GetVelocity();

  // Enemy is aligned with a tile on the board
  if ((size_t) (cur_position.x) % tile_size_ == 0 && (size_t) (cur_position.y) % tile_size_ == 0) {
    vector<PossibleMove> possible_moves;

    // check forward tile dirt
    if (IsNextTileDirt(cur_velocity, cur_position)) {
      possible_moves.push_back(PossibleMove::Forward);
    }

    // check left tile dirt
    vec2 turn_left_velocity {cur_velocity.y, cur_velocity.x * -1};
    if (IsNextTileDirt(turn_left_velocity, cur_position)) {
      possible_moves.push_back(PossibleMove::Left);
    }

    // check right tile dirt
    vec2 turn_right_velocity {cur_velocity.y * -1, cur_velocity.x};
    if (IsNextTileDirt(turn_right_velocity, cur_position)) {
      possible_moves.push_back(PossibleMove::Right);
    }

    // set new velocity of enemy
    if (possible_moves.empty()) {
      vec2 backwards_velocity {cur_velocity.x * -1, cur_velocity.y * -1};
      enemy.SetVelocity(backwards_velocity);
    } else {
      PossibleMove move = possible_moves[rand() % possible_moves.size()];

      if (move == PossibleMove::Left) {
        enemy.SetVelocity(turn_left_velocity);
      } else if (move == PossibleMove::Right) {
        enemy.SetVelocity(turn_right_velocity);
      }
    }
  }
}

bool GameEngine::IsNextTileDirt(const vec2& velocity, const vec2& position) {
  if (!IsNextTileOpen(velocity, position)) {
    return false;
  }

  if (velocity.x > 0 && velocity.y == 0) {
    size_t next_pixel_val = (size_t) (position.x) + (size_t) (velocity.x) + tile_size_;
    size_t next_x;
    
    if (next_pixel_val == game_map_.size() * tile_size_) {
      next_x = game_map_.size() - 1;
    } else {
      next_x = next_pixel_val / tile_size_;
    }

    if (game_map_[next_x][(size_t) (position.y) / tile_size_] == TileType::Tunnel) {
      return true;
    }

  } else if (velocity.x == 0 && velocity.y > 0) {
    size_t next_pixel_val = (size_t) (position.y) + (size_t) (velocity.y) + tile_size_;
    size_t next_y;
    
    if (next_pixel_val == game_map_.size() * tile_size_) {
      next_y = game_map_.size() - 1;
    } else {
      next_y = next_pixel_val / tile_size_;
    }

    if (game_map_[(size_t) (position.x) / tile_size_][next_y] == TileType::Tunnel) {
      return true;
    }

  } else if (velocity.x < 0 && velocity.y == 0) {
    int next_x = ((int) (position.x) + (int) (velocity.x)) / (int) (tile_size_);
    if (game_map_[next_x][(size_t) (position.y) / tile_size_] == TileType::Tunnel) {
      return true;
    }

  } else {
    int next_y = ((int) (position.y) + (int) (velocity.y)) / (int) (tile_size_);
    if (game_map_[(size_t) (position.x) / tile_size_][next_y] == TileType::Tunnel) {
      return true;
    }
  }

  return false;
}

bool GameEngine::IsNextTileOpen(const vec2 &velocity, const vec2 &position) {
  if (velocity.x > 0 && velocity.y == 0) {
    size_t next_x = (size_t) (position.x) + (size_t) (velocity.x) + tile_size_;
    if (next_x <= game_map_.size() * tile_size_) {
      return true;
    }

  } else if (velocity.x == 0 && velocity.y > 0) {
    size_t next_y = (size_t) (position.y) + (size_t) (velocity.y) + tile_size_;
    if (next_y <= game_map_.size() * tile_size_) {
      return true;
    }

  } else if (velocity.x < 0 && velocity.y == 0) {
    int next_x = ((int) (position.x) + (int) (velocity.x));
    if (next_x >= 0) {
      return true;
    }

  } else {
    int next_y = ((int) (position.y) + (int) (velocity.y));
    if (next_y >= 0) {
      return true;
    }
  }

  return false;
}

void GameEngine::MoveGhostedEnemy(Enemy& enemy) {
  vec2 enemy_position = enemy.GetPosition();
  vec2 player_position = player_.GetPosition();
  vec2 distance_vector = player_position - enemy_position;
  double distance = glm::length(distance_vector);

  // Enemy walks again and is not ghost anymore
  if (game_map_[(size_t) (enemy_position.x) / tile_size_][(size_t) (enemy_position.y) / tile_size_] == TileType::Tunnel
      && distance < kGhostDistanceBuffer) {
    enemy.SetIsGhost();
    enemy.SetPosition({(enemy_position.x / tile_size_) * tile_size_, (enemy_position.y / tile_size_) * tile_size_});
    // Makes sure velocity of enemy is correct now that it is walking again
    enemy.SetVelocity({kSpeed, 0});
    MoveWalkingEnemy(enemy);

  } else {
    vec2 new_velocity {distance_vector.x / distance * kSpeed, distance_vector.y / distance * kSpeed};
    enemy.SetVelocity(new_velocity);
  }
}

void GameEngine::DigUpTiles(const vec2& player_pos, const vec2& velocity) {
  if (velocity.x > 0 && velocity.y == 0) {
    // Makes sure right boundary of gameboard is considered part of the game tile to the left of it
    size_t new_pixel_position = (size_t) (player_pos.x) + tile_size_;
    size_t new_index;

    if (new_pixel_position == game_map_.size() * tile_size_) {
      new_index = game_map_.size() - 1;
    } else {
      new_index = new_pixel_position / tile_size_;
    }

    game_map_[new_index][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;

  } else if (velocity.x < 0 && velocity.y == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;

  } else if (velocity.y > 0 && velocity.x == 0) {
    // Makes sure bottom boundary of gameboard is considered part of the game tile above it
    size_t new_pixel_position = (size_t) (player_pos.y) + tile_size_;
    size_t new_index;

    if (new_pixel_position == game_map_.size() * tile_size_) {
      new_index = game_map_.size() - 1;
    } else {
      new_index = new_pixel_position / tile_size_;
    }

    game_map_[(size_t) (player_pos.x) / tile_size_][new_index] = TileType::Tunnel;

  } else if (velocity.y < 0 && velocity.x == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;
  }
}

void GameEngine::CreateHarpoon() {
  vec2 player_position = player_.GetPosition();
  vec2 player_prev_velocity = player_.GetPrevVelocity();
  vec2 harpoon_velocity_unit_vector = player_prev_velocity / glm::length(player_prev_velocity);
  vec2 harpoon_velocity = {harpoon_velocity_unit_vector.x * kHarpoonSpeed, harpoon_velocity_unit_vector.y * kHarpoonSpeed};

  harpoon_ = Harpoon(player_position, harpoon_velocity);
}

int GameEngine::GetHurtEnemy() {
  for (size_t index = 0; index < enemies_.size(); index++) {
    Enemy& enemy = enemies_[index];
    vec2 enemy_pos = enemy.GetPosition();
    vec2 harpoon_pos = harpoon_.GetArrowPosition();
    double distance = glm::length(enemy_pos - harpoon_pos);

    if (!enemy.GetIsGhost() && distance < tile_size_) {
      return index;
    }
  }

  return -1;
}

} // namespace dig_dug