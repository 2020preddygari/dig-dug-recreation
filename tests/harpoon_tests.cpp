#include "catch2/catch.hpp"

#include "core/harpoon.h"

using glm::vec2;
using dig_dug::Harpoon;

TEST_CASE("Harpoon Tests") {
  Harpoon harpoon({9, 9}, {1, 0});

  SECTION("Check harpoon starting position") {
    vec2 position {9, 9};
    REQUIRE(harpoon.GetArrowPosition() == position);
  }

  SECTION("Check harpoon movement") {
    vec2 new_position {10, 9};
    harpoon.Move();
    REQUIRE(harpoon.GetArrowPosition() == new_position);

    SECTION("Check distance traveled") {
      REQUIRE(harpoon.GetDistanceTraveled() == 100);
    }
  }
}