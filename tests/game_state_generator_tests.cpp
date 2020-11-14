#include <catch2/catch.hpp>

#include "core/game_state_generator.h"

using dig_dug::GameStateGenerator;

TEST_CASE("Gameboard Creation") {

}

TEST_CASE("GameStateGenerator Intialization and testing advancing levels") {
  GameStateGenerator generator;

  SECTION("Starting level of game") {
    REQUIRE(generator.GetLevel() == 1);
  }

  SECTION("Advancing a level") {
    generator.IncreaseLevel();
    REQUIRE(generator.GetLevel() == 2);
  }

  SECTION("Advancing level more than once") {
    generator.IncreaseLevel();
    generator.IncreaseLevel();
    REQUIRE(generator.GetLevel() == 3);
  }
}