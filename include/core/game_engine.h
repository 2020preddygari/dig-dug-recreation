#pragma once

#include <cinder/gl/gl.h>

#include "core/game_state_generator.h"
#include "core/player.h"
#include "core/enemy.h"

namespace dig_dug {

using glm::vec2;
using dig_dug::TileType;

class GameEngine {
 public:
  /**
   * Creates the game map based on the generated starting state
   *
   * @param initial_game_state starting game map
   */
  GameEngine(const vector<vector<TileType>>& initial_game_state, size_t tile_size);

  /**
   * Moves the enemies on the board
   */
  void MoveEnemies();

  /**
   * Moves the player using the given velocity
   *
   * @param velocity player velocity
   */
  void MovePlayer(const vec2& velocity);

  vector<vector<TileType>> GetGameMap();

  Player GetPlayer();

  vector<Enemy> GetEnemies();

 private:
  vector<vector<TileType>> game_map_;
  Player player_;
  vector<Enemy> enemies_;
  const double kSpeed = 1.0;
};
} // namespace dig_dug