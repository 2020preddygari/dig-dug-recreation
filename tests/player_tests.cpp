#include <catch2/catch.hpp>
#include "core/player.h"

using dig_dug::Player;
using glm::vec2;

TEST_CASE("Player Movement tests") {
  SECTION("Player instantiation") {
    Player player({7, 7});

    vec2 position {7, 7};
    REQUIRE(player.GetPosition() == position);
  }

  SECTION("Player movement with positive velocity") {
    Player player({7, 7});
    player.Move({1, 1});

    vec2 position = {8, 8};
    REQUIRE(player.GetPosition() == position);
  }

  SECTION("Player movement with negative velocity") {
    Player player({7, 7});
    player.Move({-1, -1});

    vec2 position = {6, 6};
    REQUIRE(player.GetPosition() == position);
  }
}