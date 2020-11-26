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

      if (x < initial_game_state.size() / 2 + 1 && y == initial_game_state.size() / 2 + 1) {
        game_map_[x][y] = TileType::Tunnel;
      }
    }
  }
}

void GameEngine::MoveEnemies() {

}

void GameEngine::MovePlayer(const vec2 &velocity) {

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
} // namespace dig_dug