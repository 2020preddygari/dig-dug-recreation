#include <catch2/catch.hpp>
#include "core/enemy.h"

using dig_dug::Enemy;
using glm::vec2;

TEST_CASE("Enemy Movement tests") {
  SECTION("Enemy instantiation") {
    Enemy enemy({7, 7});

    vec2 position {7, 7};
    REQUIRE(enemy.GetPosition() == position);
  }

  SECTION("Enemy movement with positive velocity") {
    Enemy enemy({7, 7});
    enemy.Move({1, 1});

    vec2 position = {8, 8};
    REQUIRE(enemy.GetPosition() == position);
  }

  SECTION("Enemy movement with negative velocity") {
    Enemy enemy({7, 7});
    enemy.Move({-1, -1});

    vec2 position = {6, 6};
    REQUIRE(enemy.GetPosition() == position);
  }
}