#include "core/game_state_generator.h"

namespace dig_dug {

vector<vector<TileType>> GameStateGenerator::Generate() {
  game_map_.clear();
  const size_t kMaxEnemies = 8;
  const size_t kMaxLevelWithAdditionalEnemy = 10;
  const size_t kMinRocks = 3;
  // Formula for calculating number of enemies using level
  size_t num_enemies;
  // 3 or 4 rocks
  size_t num_rocks = (rand() % 2) + kMinRocks;
  if (level_ <= kMaxLevelWithAdditionalEnemy) {
    num_enemies = static_cast<int>((level_ - 1) / 2) + 4;
  } else {
    num_enemies = kMaxEnemies;
  }

  // Sets default map with all dirt
  for (size_t i = 0; i < kBoardDimension_; i++) {
    vector<TileType> cur_row;
    for (size_t j = 0; j < kBoardDimension_; j++) {
      cur_row.push_back(TileType::Dirt);
    }

    /* #TODO put srand(time(NULL)) at start of project for truly random numbers every execution*/
    game_map_.push_back(cur_row);
  }

  GenerateEnemies(num_enemies);
  GenerateRocks(num_rocks);

  return game_map_;

}

void GameStateGenerator::IncreaseLevel() {
  level_++;
}

size_t GameStateGenerator::GetLevel() const {
  return level_;
}

vector<vector<TileType>> GameStateGenerator::GetGameMap() const {
  return game_map_;
}

void GameStateGenerator::GenerateEnemies(size_t num_enemies) {
  TileType cur_enemy = TileType::Pooka;
  size_t mid_value = static_cast<int>(kBoardDimension_ / 2);

  for (size_t num = 0; num < num_enemies; num++) {
    bool is_space_possible = false;
    while (!is_space_possible) {
      is_space_possible = true;

      size_t x_pos = rand() % (kBoardDimension_ - kTunnelSize_);
      size_t y_pos = rand() % (kBoardDimension_ - kTunnelSize_);
      // 0 - horizontal; 1 - vertical
      size_t direction = rand() % 2;

      // Find different coordinates if the selected ones will lead to a tunnel intersecting the player's starting place
      if ((x_pos >= mid_value - kTunnelSize_ && x_pos <= mid_value + 1 && y_pos <= mid_value + 1 && direction == 0)
          || (x_pos >= mid_value - 1 && x_pos <= mid_value + 1 && direction == 1)) {
        is_space_possible = false;
        continue;
      }


      if (direction == 0) {
        // Checks if every space in tunnel is not next to another enemy or tunnel
        for (size_t tunnel_space = 0; tunnel_space < kTunnelSize_; tunnel_space++) {
          if (!IsGoodSpot(x_pos + tunnel_space, y_pos)) {
            is_space_possible = false;
            break;
          }
        }

        if (is_space_possible) {
          game_map_[x_pos][y_pos] = cur_enemy;

          if (cur_enemy == TileType::Pooka) {
            cur_enemy = TileType::Fygar;
          } else {
            cur_enemy = TileType::Pooka;
          }

          for (size_t tunnel_space = 1; tunnel_space < kTunnelSize_; tunnel_space++) {
            game_map_[x_pos + tunnel_space][y_pos] = TileType::Tunnel;
          }
        }

      } else {
        for (size_t tunnel_space = 0; tunnel_space < kTunnelSize_; tunnel_space++) {
          if (!IsGoodSpot(x_pos, y_pos + tunnel_space)) {
            is_space_possible = false;
            break;
          }
        }

        if (is_space_possible) {
          game_map_[x_pos][y_pos] = cur_enemy;

          if (cur_enemy == TileType::Pooka) {
            cur_enemy = TileType::Fygar;
          } else {
            cur_enemy = TileType::Pooka;
          }

          for (size_t tunnel_space = 1; tunnel_space < kTunnelSize_; tunnel_space++) {
            game_map_[x_pos][y_pos + tunnel_space] = TileType::Tunnel;
          }
        }
      }
    }
  }
}

void GameStateGenerator::GenerateRocks(size_t num_rocks) {
  for (size_t num = 0; num < num_rocks; num++) {
    bool is_space_possible = false;
    while (!is_space_possible) {
      is_space_possible = true;

      size_t x_pos = rand() % kBoardDimension_;
      size_t y_pos = rand() % kBoardDimension_;

      if (game_map_[x_pos][y_pos] != TileType::Dirt) {
        is_space_possible = false;
        continue;
      }

      game_map_[x_pos][y_pos] = TileType::Rock;
    }
  }
}

bool GameStateGenerator::IsGoodSpot(size_t x_pos, size_t y_pos) {
  // Checks if there is an enemy or tunnel in a (kEnemyBuffer)-block radius of the current coordinate
  if (IsEnemyOrTunnel(x_pos + kEnemyBuffer, y_pos)
      || IsEnemyOrTunnel(x_pos + kEnemyBuffer, y_pos + kEnemyBuffer)
      || IsEnemyOrTunnel(x_pos + kEnemyBuffer, y_pos - kEnemyBuffer)
      || IsEnemyOrTunnel(x_pos, y_pos + kEnemyBuffer)
      || IsEnemyOrTunnel(x_pos, y_pos - kEnemyBuffer)
      || IsEnemyOrTunnel(x_pos - kEnemyBuffer, y_pos)
      || IsEnemyOrTunnel(x_pos - kEnemyBuffer, y_pos + kEnemyBuffer)
      || IsEnemyOrTunnel(x_pos - kEnemyBuffer, y_pos + kEnemyBuffer)) {
    return false;
  }

  return true;
}

bool GameStateGenerator::IsEnemyOrTunnel(size_t x_pos, size_t y_pos) {
  if (x_pos < 0 || x_pos >= kBoardDimension_ || y_pos < 0 || y_pos >= kBoardDimension_) {
    return false;
  }

  if (game_map_[x_pos][y_pos] == TileType::Fygar
      || game_map_[x_pos][y_pos] == TileType::Pooka
      || game_map_[x_pos][y_pos] == TileType::Tunnel) {
    return true;
  }

  return false;
}

}