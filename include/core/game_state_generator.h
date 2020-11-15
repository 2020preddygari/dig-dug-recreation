#pragma once

#include <vector>
#include <random>

namespace dig_dug {

using std::vector;

enum class TileType {
  Dirt,
  Tunnel,
  Player,
  Fygar,
  Pooka,
  Ghost,
  Rock,
};

class GameStateGenerator {
 public:
  /**
   * Fills in the field values when a GameStateGenerator is created
   */
  GameStateGenerator() = default;

  /**
   * Returns the starting game state for the current level
   */
  vector<vector<TileType>> Generate();

  /**
   * Increases the level by 1
   */
  void IncreaseLevel();

  size_t GetLevel() const;

  vector<vector<TileType>> GetGameMap() const;

 private:
  size_t level_ = 1;
  size_t kBoardDimension_ = 15;
  size_t kTunnelSize_ = 3;
  // minimum distance between enemies
  size_t kEnemyBuffer = 1;
  vector<vector<TileType>> game_map_;

  /**
   * Generates the specified number of the enemies in the map
   *
   * @param num_objects number of enemies
   */
  void GenerateEnemies(size_t num_enemies);

  /**
   * Generates the specified number of rocks in the map
   *
   * @param num_rocks number of rocks
   */
  void GenerateRocks(size_t num_rocks);

  /**
   * Checks whether the given coordinate is a good spot for an enemy
   *
   * @param x_pos x position
   * @param y_pos y position
   * @return true if it is a good spot, false if not
   */
  bool IsGoodSpot(size_t x_pos, size_t y_pos);

  /**
   * Checks whether given coordinate contains a fygar, pooka, or tunnel
   *
   * @param x_pos x position
   * @param y_pos y position
   * @return true if there is an enemy, false if not
   */
  bool IsEnemyOrTunnel(size_t x_pos, size_t y_pos);
};
}