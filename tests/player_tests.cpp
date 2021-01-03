#include <catch2/catch.hpp>
#include "core/player.h"

using dig_dug::Player;
using dig_dug::CharacterOrientation;
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

  SECTION("Player previous velocity") {
    Player player ({7, 7});

    SECTION("Check that previous velocity is updated") {
      vec2 prev_velocity {1, 0};
      player.Move({1, 0});
      REQUIRE(player.GetPrevVelocity() == prev_velocity);
    }

    SECTION("Check that previous velocity is not update when the current velocity is zero") {
      vec2 prev_velocity {1, 0};
      player.Move({1, 0});
      player.Move({0, 0});
      REQUIRE(player.GetPrevVelocity() == prev_velocity);
    }
  }

  SECTION("Player orientation") {
    Player player ({7, 7});

    SECTION("Check updated to right") {
      player.Move({1, 0});
      REQUIRE(player.GetOrientation() == CharacterOrientation::Right);
    }

    SECTION("Check updated to left") {
      player.Move({-1, 0});
      REQUIRE(player.GetOrientation() == CharacterOrientation::Left);
    }
  }
}