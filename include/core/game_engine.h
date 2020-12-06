#pragma once

#include <cinder/gl/gl.h>

#include "core/game_state_generator.h"
#include "core/player.h"
#include "core/enemy.h"
#include "core/harpoon.h"

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
   * Constructs an empty game engine
   */
  GameEngine() = default;
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

  /**
   * Checks whether an enemy killed the player
   *
   * @return true if player dies, false if not
   */
  bool IsPlayerDead();

  /*
   * Player shoots the harpoon
   */
  void AttackEnemy();

  vector<vector<TileType>> GetGameMap();

  Player GetPlayer();

  vector<Enemy> GetEnemies();

  size_t GetNumLives();

  void SetNumLives(size_t num_lives);

  bool GetIsAttacking();

  Harpoon GetHarpoon();

  size_t GetScore();

  void SetScore(size_t score);

 private:
  vector<vector<TileType>> game_map_;
  Player player_;
  vector<Enemy> enemies_;
  Harpoon harpoon_;
  bool is_attacking_ = false;
  double enemy_ghost_percentage_;
  size_t tile_size_;
  size_t num_lives_ = 3;
  size_t cur_attack_frames_ = 0;
  size_t max_harpoon_traveling_frames_;
  vec2 delayed_turn_velocity_ {0, 0};
  size_t score_ = 0;
  constexpr static double kPlayerSpeed = 10;
  constexpr static double kEnemySpeed = 4;
  constexpr static double kEnemyDifficulty = 0.01;
  constexpr static double kGhostDistanceBuffer = 500;
  const static size_t kAttackFrames = 20;
  const static size_t kHarpoonLength = 10;
  const static size_t kHarpoonSpeed = 20;
  const static size_t kEnemyKillScore = 100;

  /**
   * Moves a normal, walking enemy
   *
   * @param enemy
   */
  void MoveWalkingEnemy(Enemy& enemy);

  /**
   * Checks whether the next tile along the object's path is dirt
   *
   * @param velocity velocity of object
   * @param position position of object
   * @return true if next tile is dirt, false otherwise
   */
  bool IsNextTileDirt(const vec2& velocity, const vec2& position);

  /**
   * Checks whether the next tile along the object's path is within the bounds of the board
   *
   * @param velocity velocity of object
   * @param position position of object
   * @return true if next tile is allowed, false otherwise
   */
  bool IsNextTileOpen(const vec2& velocity, const vec2& position);

  /**
   * Moves a ghosted enemy that can go through dirt
   *
   * @param enemy
   */
  void MoveGhostedEnemy(Enemy& enemy);

  /**
   * Turns the dirt tiles that the player enters into tunnels
   *
   * @param player_pos player position
   * @param velocity player velocity
   */
  void DigUpTiles(const vec2& player_pos, const vec2& velocity);

  /**
   * Creates a harpoon
   */
  void CreateHarpoon();

  /**
   * Gets the index of the enemy which the harpoon is hurting
   *
   * @return index of enemy, or -1 if harpoon not hurting anything
   */
  int GetHurtEnemy();

  /**
   * Gets the index on the game board of a player's position in pixels
   *
   * @param position player position
   * @return index on gameboard
   */
  size_t GetIndexOfPlayer(size_t position);
};
} // namespace dig_dug