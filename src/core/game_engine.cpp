#include "core/game_engine.h"

namespace dig_dug {


GameEngine::GameEngine(const vector<vector<TileType>>& initial_game_state, size_t tile_size) {
  size_t center_coord = (initial_game_state.size() / 2) * tile_size;
  player_ = Player({center_coord, center_coord});
  board_size_ = initial_game_state.size();

  // Takes enemies out of board and stores them in enemies_
  game_map_ = initial_game_state;
  for (size_t x = 0; x < board_size_; x++) {
    for (size_t y = 0; y < board_size_; y++) {
      TileType type = game_map_[x][y];
      if (type == TileType::Pooka || type == TileType::Fygar) {
        vec2 position {x * tile_size, y * tile_size};

        vec2 velocity;
        if (game_map_[x + 1][y] == TileType::Tunnel) {
          velocity = {kEnemySpeed, 0};
        } else if (game_map_[x][y + 1] == TileType::Tunnel) {
          velocity = {0, kEnemySpeed};
        }

        Enemy enemy (position, velocity, type);
        enemies_.push_back(enemy);
        game_map_[x][y] = TileType::Tunnel;
      }

      if (x == initial_game_state.size() / 2 && y <= initial_game_state.size() / 2) {
        game_map_[x][y] = TileType::Tunnel;
      }
    }
  }

  enemy_ghost_percentage_ = enemies_.size() * kEnemyDifficulty;
  tile_size_ = tile_size;
  max_harpoon_traveling_frames_ = tile_size * kHarpoonLength / (size_t) (kEnemySpeed);
  
}

void GameEngine::MoveEnemies() {
  // Turns an enemy into a ghost if random number below enemy_ghost_percentage_
  if ((size_t) (rand() % 10000) < enemy_ghost_percentage_ * 100) {
    size_t ghost_index = rand() % enemies_.size();
    Enemy& cur_enemy = enemies_[ghost_index];
    
    if (!cur_enemy.IsGhost()) {
      cur_enemy.SetGhost();
    }
  }

  int enemy_hurt_index = GetHurtEnemy();
  if (enemy_hurt_index > -1) {
    enemies_[enemy_hurt_index].SetHurt(true);
  }

  for (size_t index = 0; index < enemies_.size(); index++) {
    Enemy& cur_enemy = enemies_[index];

    if (!cur_enemy.IsHurt()) {
      if (cur_enemy.IsGhost()) {
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
  player_attacking_ = false;
  
  for (size_t index = 0; index < enemies_.size(); index++) {
   enemies_[index].SetHurt(false);
  }

  const vec2 kZeroVelocity {0, 0};
  vec2 position = player_.GetPosition();
  vec2 velocity_with_speed {velocity.x * kPlayerSpeed, velocity.y * kPlayerSpeed};
  vec2 opposite_velocity {velocity_with_speed.x * -1, velocity_with_speed.y * -1};
  vec2 player_prev_speed = player_.GetPrevVelocity();
  bool next_tile_open = IsNextTileOpen(velocity_with_speed, player_.GetPosition());

  if (next_tile_open) {
    // Checks if player is aligned with tile
    if ((size_t) (position.x) % tile_size_ == 0 && (size_t) (position.y) % tile_size_ == 0) {
      // Check if player tried to turn in the middle of tiles, and performs that move if so
      if ((velocity_with_speed == player_prev_speed || velocity_with_speed == kZeroVelocity)
          && delayed_turn_velocity_ != kZeroVelocity) {
        player_.Move(delayed_turn_velocity_);
        delayed_turn_velocity_ = {0, 0};
        DigUpTiles(player_.GetPosition(), delayed_turn_velocity_);

      } else {
        player_.Move(velocity_with_speed);
        DigUpTiles(player_.GetPosition(), velocity_with_speed);
      }

    // Moves if movement is in the same axis as the previous move
    } else if (velocity_with_speed == player_prev_speed || opposite_velocity == player_prev_speed) {
      player_.Move(velocity_with_speed);
      DigUpTiles(player_.GetPosition(), velocity_with_speed);

    // Moves according to the previous velocity if trying to turn in the middle of tiles
    } else {
      player_.Move(player_prev_speed);
      DigUpTiles(player_.GetPosition(), player_prev_speed);

      if (velocity_with_speed != kZeroVelocity) {
        delayed_turn_velocity_ = velocity_with_speed;
      }
    }
  }
}

bool GameEngine::IsPlayerDead() {
  for (const Enemy& enemy : enemies_) {
    vec2 enemy_pos = enemy.GetPosition();
    vec2 player_pos = player_.GetPosition();
    double distance = glm::length(enemy_pos - player_pos);

    if (!enemy.IsGhost() && distance < tile_size_) {
      num_lives_--;
      return true;
    }
  }

  return false;
}

void GameEngine::AttackEnemy() {
  if (!player_attacking_) {
    CreateHarpoon();
    player_attacking_ = true;

  }

  vec2 arrow_position = harpoon_.GetArrowPosition();
  int hurt_enemy_index = GetHurtEnemy();
  if (hurt_enemy_index > -1) {
    cur_attack_frames_++;
    enemies_[hurt_enemy_index].SetHurt(true);

    // Enemy dies
    if (cur_attack_frames_ >= kAttackFrames) {
      enemies_.erase(enemies_.begin() + hurt_enemy_index);
      cur_attack_frames_ = 0;
      player_attacking_ = false;
      score_ += kEnemyKillScore;
    }

  } else if (harpoon_.GetDistanceTraveled() >= max_harpoon_traveling_frames_
            || !CanHarpoonContinue()) {
    player_attacking_ = false;

  } else {
    harpoon_.Move();
  }
}

vector<vector<TileType>> GameEngine::GetGameMap() const {
  return game_map_;
}

Player GameEngine::GetPlayer() const {
  return player_;
}

vector<Enemy> GameEngine::GetEnemies() const {
  return enemies_;
}

size_t GameEngine::GetNumLives() const {
  return num_lives_;
}

void GameEngine::SetNumLives(size_t num_lives) {
  num_lives_ = num_lives;
}

bool GameEngine::IsPlayerAttacking() const {
  return player_attacking_;
}

Harpoon GameEngine::GetHarpoon() const {
  return harpoon_;
}

size_t GameEngine::GetScore() const {
  return score_;
}

void GameEngine::SetScore(size_t score) {
  score_ = score;
}

void GameEngine::MoveWalkingEnemy(Enemy& enemy) const {
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

bool GameEngine::IsNextTileDirt(const vec2& velocity, const vec2& position) const {
  if (!IsNextTileOpen(velocity, position)) {
    return false;
  }

  int next_x;
  int next_y;
  // Gets the correct next x and y index for the given position and velocity
  if (velocity.x > 0 && velocity.y == 0) {
    next_x = GetIndexOfPlayer((size_t) (position.x + velocity.x));
    next_y = (int) (position.y) / (int) (tile_size_);

  } else if (velocity.x == 0 && velocity.y > 0) {
    next_x = (int) (position.x) / (int) (tile_size_);
    next_y = GetIndexOfPlayer((size_t) (position.y + velocity.y));

  } else if (velocity.x < 0 && velocity.y == 0) {
    next_x = ((int) (position.x) + (int) (velocity.x)) / (int) (tile_size_);
    next_y = (int) (position.y) / (int) (tile_size_);

  } else {
    next_y = ((int) (position.y) + (int) (velocity.y)) / (int) (tile_size_);
    next_x = (int) (position.x) / (int) (tile_size_);
  }

  if (game_map_[next_x][next_y] == TileType::Tunnel) {
    return true;
  }

  return false;
}

bool GameEngine::IsNextTileOpen(const vec2 &velocity, const vec2 &position) const {
  if (velocity.x > 0 && velocity.y == 0) {
    size_t next_x = (size_t) (position.x) + (size_t) (velocity.x) + tile_size_;
    if (next_x < board_size_ * tile_size_
        && game_map_[next_x / tile_size_][(size_t) (position.y) / tile_size_] != TileType::Rock) {
      return true;
    }

  } else if (velocity.x == 0 && velocity.y > 0) {
    size_t next_y = (size_t) (position.y) + (size_t) (velocity.y) + tile_size_;
    if (next_y < board_size_ * tile_size_
        && game_map_[(size_t) (position.x) / tile_size_][next_y / tile_size_] != TileType::Rock) {
      return true;
    }

  } else if (velocity.x < 0 && velocity.y == 0) {
    int next_x = ((int) (position.x) + (int) (velocity.x));
    if (next_x >= 0 && game_map_[next_x / tile_size_][(size_t) (position.y) / tile_size_] != TileType::Rock) {
      return true;
    }

  } else {
    int next_y = ((int) (position.y) + (int) (velocity.y));
    if (next_y >= 0 && game_map_[(size_t) (position.x) / tile_size_][next_y / tile_size_] != TileType::Rock) {
      return true;
    }
  }

  return false;
}

void GameEngine::MoveGhostedEnemy(Enemy& enemy) const {
  vec2 enemy_position = enemy.GetPosition();
  vec2 player_position = player_.GetPosition();
  vec2 distance_vector = player_position - enemy_position;
  double distance = glm::length(distance_vector);
  TileType tile = game_map_[(size_t) (enemy_position.x) / tile_size_][(size_t) (enemy_position.y) / tile_size_];
  
  if (tile == TileType::Dirt || tile == TileType::Rock) {
    enemy.SetInDirt(true);
  }

  // Enemy walks again and is not ghost anymore
  if (tile == TileType::Tunnel
      && distance < kGhostDistanceBuffer && enemy.IsInDirt()) {
    enemy.SetGhost();
    enemy.SetPosition({((size_t) (enemy_position.x) / tile_size_) * tile_size_,
                       ((size_t) (enemy_position.y) / tile_size_) * tile_size_});
    // Makes sure velocity of enemy is correct now that it is walking again
    enemy.SetVelocity({kEnemySpeed, 0});
    MoveWalkingEnemy(enemy);

  } else {
    vec2 new_velocity {distance_vector.x / distance * kEnemySpeed, distance_vector.y / distance * kEnemySpeed};
    enemy.SetVelocity(new_velocity);
  }
}

void GameEngine::DigUpTiles(const vec2& player_pos, const vec2& velocity) {
  // So player does not dig up tile it has not entered yet
  if ((size_t) (player_pos.x) % tile_size_ == 0 && (size_t) (player_pos.y) % tile_size_ == 0) {
    return;
  }

  if (velocity.x > 0 && velocity.y == 0) {
    game_map_[GetIndexOfPlayer((size_t) (player_pos.x))][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;

  } else if (velocity.y > 0 && velocity.x == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][GetIndexOfPlayer((size_t) (player_pos.y))] = TileType::Tunnel;

  } else {
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

int GameEngine::GetHurtEnemy() const {
  if (!player_attacking_) {
    return -1;
  }

  for (size_t index = 0; index < enemies_.size(); index++) {
    vec2 enemy_pos = enemies_[index].GetPosition();
    vec2 harpoon_pos = harpoon_.GetArrowPosition();
    double distance = glm::length(enemy_pos - harpoon_pos);

    if (!enemies_[index].IsGhost() && distance < tile_size_) {
      return index;
    }
  }

  return -1;
}

size_t GameEngine::GetIndexOfPlayer(size_t position) const {
  size_t new_pixel_position = position + tile_size_;
  size_t new_index;

  // Includes boundary as part of the tile before it
  if (new_pixel_position == board_size_ * tile_size_) {
    new_index = board_size_ - 1;
  } else {
    new_index = new_pixel_position / tile_size_;
  }

  return new_index;
}

bool GameEngine::CanHarpoonContinue() const {
  vec2 arrow_pos = harpoon_.GetArrowPosition();
  vec2 harpoon_velocity = harpoon_.GetVelocity();
  vec2 unit_harpoon_velocity = harpoon_velocity / glm::length(harpoon_velocity);
  vec2 next_pos = arrow_pos;

  if (harpoon_velocity.x > 0 || harpoon_velocity.y > 0) {
    next_pos.x += unit_harpoon_velocity.x * tile_size_;
    next_pos.y += unit_harpoon_velocity.y * tile_size_;
  }

  size_t arrow_x = (size_t) (arrow_pos.x) / tile_size_;
  size_t arrow_y = (size_t) (arrow_pos.y) / tile_size_;
  size_t next_x = (size_t) (next_pos.x) / tile_size_;
  size_t next_y = (size_t) (next_pos.y) / tile_size_;

  if (arrow_pos.x >= 0 && arrow_x < board_size_
      && arrow_pos.y >= 0 && arrow_y < board_size_
      && next_pos.x >= 0 && next_x < board_size_
      && next_pos.y >= 0 && next_y < board_size_
      && game_map_[arrow_x][arrow_y] == TileType::Tunnel
      && game_map_[next_x][next_y] == TileType::Tunnel) {
    return true;
  }

  return false;
}

} // namespace dig_dug