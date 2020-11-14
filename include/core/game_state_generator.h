#pragma once

#include <vector>

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
  vector<vector<TileType>> Generate() const;

  /**
   * Increases the level by 1
   */
  void IncreaseLevel();

  size_t GetLevel() const;

 private:
  size_t level_ = 1;
  size_t kBoardDimension_ = 15;
};
}