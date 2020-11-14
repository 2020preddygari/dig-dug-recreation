#include "core/game_state_generator.h"

namespace dig_dug {

vector<vector<TileType>> GameStateGenerator::Generate() const {
/* #TODO Generate game map*/
  vector<vector<TileType>> temp_vect;
  return temp_vect;
}

void GameStateGenerator::IncreaseLevel() {
  level_++;
}

size_t GameStateGenerator::GetLevel() const {
  return level_;
}

}