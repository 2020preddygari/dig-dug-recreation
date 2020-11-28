#include <catch2/catch.hpp>
#include "core/enemy.h"
#include "core/game_state_generator.h"

using dig_dug::Enemy;
using dig_dug::TileType;
using glm::vec2;

TEST_CASE("Enemy Movement tests") {
  SECTION("Enemy instantiation") {
    Enemy enemy({7, 7}, {1, 0}, TileType::Pooka);

    SECTION("Check position") {
      vec2 position {7, 7};
      REQUIRE(enemy.GetPosition() == position);
    }

    SECTION("Check velocity") {
      vec2 velocity {1, 0};
      REQUIRE(enemy.GetVelocity() == velocity);
    }

    SECTION("Check enemy type") {
      REQUIRE(enemy.GetType() == TileType::Pooka);
    }
  }

  SECTION("Enemy movement with positive velocity") {
    Enemy enemy({7, 7}, {1, 0}, TileType::Fygar);
    enemy.Move();

    vec2 position = {8, 7};
    REQUIRE(enemy.GetPosition() == position);
  }

  SECTION("Enemy movement with negative velocity") {
    Enemy enemy({7, 7}, {-1, 0}, TileType::Fygar);
    enemy.Move();

    vec2 position = {6, 7};
    REQUIRE(enemy.GetPosition() == position);
  }
}