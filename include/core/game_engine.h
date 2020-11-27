#pragma once

#include <cinder/gl/gl.h>

#include "core/game_state_generator.h"
#include "core/player.h"
#include "core/enemy.h"

namespace dig_dug {

using glm::vec2;
using dig_dug::TileType;

enum class PossibleMove {
  Forward,
  Right,
  Left
};

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
  size_t enemy_ghost_percentage_;
  size_t tile_size_;
  const double kSpeed = 1;
  const size_t kEnemyDifficulty = 2;
  const double kGhostDistanceBuffer = 500;

  /**
   * Moves a normal, walking enemy
   *
   * @param enemy
   */
  void MoveWalkingEnemy(Enemy& enemy);

  /**
   * Checks whether the next tile along the object's path is open
   *
   * @param velocity velocity of object
   * @param position position ofo object
   * @return true if next tile open, false otherwise
   */
  bool IsNextTileOpen(const vec2& velocity, const vec2& position);

  /**
   * Moves a ghosted enemy that can go through dirt
   *
   * @param enemy
   */
  void MoveGhostedEnemy(Enemy& enemy);
};
} // namespace dig_dug