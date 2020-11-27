#include <catch2/catch.hpp>

#include "core/game_engine.h"

using dig_dug::GameStateGenerator;
using dig_dug::GameEngine;
using dig_dug::TileType;
using dig_dug::Player;
using dig_dug::Enemy;
using std::vector;
using glm::vec2;

TEST_CASE("Constructing Game Engine") {
  GameStateGenerator generator;
  generator.Generate();
  GameEngine engine (generator.GetGameMap(), 100);
  vector<vector<TileType>> game_map = engine.GetGameMap();

  SECTION("Check that game_map does not have enemies") {
    bool is_enemies = false;
    for (size_t x = 0; x < game_map.size(); x++) {
      for (size_t y = 0; y < game_map.size(); y++) {
        if (game_map[x][y] == TileType::Fygar || game_map[x][y] == TileType::Pooka) {
          is_enemies = true;
        }
      }
    }

    REQUIRE_FALSE(is_enemies);
  }

  SECTION("Check that game_map has 3 or 4 rocks") {
    size_t num_rocks = 0;
    for (size_t x = 0; x < game_map.size(); x++) {
      for (size_t y = 0; y < game_map.size(); y++) {
        if (game_map[x][y] == TileType::Rock) {
          num_rocks++;
        }
      }
    }

    bool is_correct_rocks = num_rocks == 3 || num_rocks == 4;
    REQUIRE(is_correct_rocks);
  }

  SECTION("Check tunnel in middle of board") {
    size_t is_tunnel_in_middle = true;
    size_t x = 8;
    for (size_t y = 0; y <= 8; y++) {
      if (game_map[x][y] != TileType::Tunnel) {
        is_tunnel_in_middle = false;
      }
    }

    REQUIRE(is_tunnel_in_middle);
  }

  SECTION("Check that player location is in middle of the board") {
    Player player = engine.GetPlayer();
    vec2 position {700, 700};
    REQUIRE(player.GetPosition() == position);
  }

  SECTION("Check enemy attributes") {
    vector<Enemy> enemies = engine.GetEnemies();

    SECTION("Check number of enemies") {
      REQUIRE(enemies.size() == 4);
    }

    SECTION("Check velocities of enemies") {
      vec2 horizontal_velocity {1, 0};
      vec2 vertical_velocity {0, 1};

      for (Enemy enemy : enemies) {
        vec2 velocity = enemy.GetVelocity();
        vec2 position = enemy.GetPosition();

        if (game_map[(size_t) (position[0] / 100 + 1)][(size_t) (position[1] / 100)] == TileType::Tunnel) {
          REQUIRE(velocity == horizontal_velocity);
        } else if (game_map[(size_t) (position[0] / 100)][(size_t) (position[1] / 100 + 1)] == TileType::Tunnel) {
          REQUIRE(velocity == vertical_velocity);
        }
      }
    }
  }
}

TEST_CASE("Moving Enemies") {
  GameStateGenerator generator;
  generator.Generate();
  GameEngine engine (generator.GetGameMap(), 100);

  SECTION("Checking enemies walking") {
    vector<Enemy> enemies = engine.GetEnemies();
    vector<vec2> enemy_positions;

    for (Enemy enemy : enemies) {
      enemy.Move();
      enemy_positions.push_back(enemy.GetPosition());
    }

    engine.MoveEnemies();
    vector<Enemy> new_enemies = engine.GetEnemies();

    for (size_t enemy = 0; enemy < enemies.size(); enemy++) {
      REQUIRE(enemy_positions[enemy] == new_enemies[enemy].GetPosition());
    }
  }

  SECTION("Check that game map is not changed by enemy movement") {
    vector<vector<TileType>> game_map = engine.GetGameMap();

    SECTION("First enemy movement") {
      engine.MoveEnemies();
      REQUIRE(game_map == engine.GetGameMap());
    }

    SECTION("Multiple moves") {
      engine.MoveEnemies();
      engine.MoveEnemies();
      engine.MoveEnemies();
      REQUIRE(game_map == engine.GetGameMap());
    }
  }
}