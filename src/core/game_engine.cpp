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

        enemies_.emplace_back(position, velocity);
        game_map_[x][y] = TileType::Tunnel;
      }

      if (x == initial_game_state.size() / 2 + 1 && y <= initial_game_state.size() / 2 + 1) {
        game_map_[x][y] = TileType::Tunnel;
      }
    }
  }

  enemy_ghost_percentage_ = enemies_.size() * kEnemyDifficulty;
  tile_size_ = tile_size;
}

void GameEngine::MoveEnemies() {
  if (rand() % tile_size_ < enemy_ghost_percentage_) {
    size_t ghost_index = rand() % enemies_.size();
    enemies_[ghost_index].SetIsGhost();
  }

  for (size_t index = 0; index < enemies_.size(); index++) {
    Enemy& cur_enemy = enemies_[index];

    if (cur_enemy.GetIsGhost()) {
      MoveGhostedEnemy(cur_enemy);
    } else {
      MoveWalkingEnemy(cur_enemy);
    }

    cur_enemy.Move();
  }
}

void GameEngine::MovePlayer(const vec2& velocity) {
  vec2 velocity_with_speed {velocity.x * kSpeed, velocity.y * kSpeed};
  player_.Move(velocity_with_speed);
  vec2 position = player_.GetPosition();
  DigUpTiles(position, velocity);
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

void GameEngine::MoveWalkingEnemy(Enemy& enemy) {
  vec2 cur_position = enemy.GetPosition();
  vec2 cur_velocity = enemy.GetVelocity();

  // Enemy is aligned with a tile on the board
  if ((size_t) (cur_position.x) % tile_size_ == 0 && (size_t) (cur_position.y) % tile_size_ == 0) {
    vector<PossibleMove> possible_moves;

    // check forward tile open
    if (IsNextTileOpen(cur_velocity, cur_position)) {
      possible_moves.push_back(PossibleMove::Forward);
    }

    // check left tile open
    vec2 turn_left_velocity {cur_velocity.y, cur_velocity.x * -1};
    if (IsNextTileOpen(turn_left_velocity, cur_position)) {
      possible_moves.push_back(PossibleMove::Left);
    }

    // check right tile open
    vec2 turn_right_velocity {cur_velocity.y * -1, cur_velocity.x};
    if (IsNextTileOpen(turn_right_velocity, cur_position)) {
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

bool GameEngine::IsNextTileOpen(const vec2& velocity, const vec2& position) {
  if (velocity.x > 0 && velocity.y == 0) {
    size_t next_x = ((size_t) (position.x) + (size_t) (velocity.x)) / tile_size_ + 1;
    if (next_x < game_map_.size() && game_map_[next_x][(size_t) (position.y) / tile_size_] == TileType::Tunnel) {
      return true;
    }

  } else if (velocity.x == 0 && velocity.y > 0) {
    size_t next_y = ((size_t) (position.y) + (size_t) (velocity.y)) / tile_size_ + 1;
    if (next_y < game_map_.size() && game_map_[(size_t) (position.x) / tile_size_][next_y] == TileType::Tunnel) {
      return true;
    }

  } else if (velocity.x < 0 && velocity.y == 0) {
    size_t next_x = ((size_t) (position.x) + (size_t) (velocity.x)) / tile_size_;
    if (next_x >= 0 && game_map_[next_x][(size_t) (position.y) / tile_size_] == TileType::Tunnel) {
      return true;
    }

  } else {
    size_t next_y = ((size_t) (position.y) + (size_t) (velocity.y)) / tile_size_;
    if (next_y >= 0 && game_map_[(size_t) (position.x) / tile_size_][next_y] == TileType::Tunnel) {
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
    game_map_[((size_t) (player_pos.x) + tile_size_) / tile_size_][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;
  } else if (velocity.x < 0 && velocity.y == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;
  } else if (velocity.y > 0 && velocity.x == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][((size_t) (player_pos.y + tile_size_)) / tile_size_] = TileType::Tunnel;
  } else if (velocity.y < 0 && velocity.x == 0) {
    game_map_[(size_t) (player_pos.x) / tile_size_][(size_t) (player_pos.y) / tile_size_] = TileType::Tunnel;
  }
}

} // namespace dig_dug