#include <catch2/catch.hpp>

#include "core/game_state_generator.h"

using dig_dug::GameStateGenerator;
using dig_dug::TileType;
using std::vector;

TEST_CASE("Gameboard Creation") {
  GameStateGenerator generator;
  generator.Generate();
  vector<vector<TileType>> game_map = generator.GetGameMap();
  
  SECTION("Check if correct number of enemies were spawned") {
    SECTION("First level") {
      size_t num_enemies = 0;
      for (size_t i = 0; i < game_map.size(); i++) {
        for (size_t j = 0; j < game_map[0].size(); j++) {
          if (game_map[i][j] == TileType::Pooka || game_map[i][j] == TileType::Fygar) {
            num_enemies++;
          }
        }
      }

      REQUIRE(num_enemies == 4);
    }

    SECTION("Generating a different level after the player completes a level") {
      generator.IncreaseLevel();
      generator.IncreaseLevel();
      generator.Generate();
      game_map = generator.GetGameMap();

      size_t num_enemies = 0;
      for (size_t i = 0; i < game_map.size(); i++) {
        for (size_t j = 0; j < game_map[0].size(); j++) {
          if (game_map[i][j] == TileType::Pooka || game_map[i][j] == TileType::Fygar) {
            num_enemies++;
          }
        }
      }

      REQUIRE(num_enemies == 5);
    }

    SECTION("Generating a level over the level where no more enemies are added") {
      for (size_t level = 2; level <= 10; level++) {
        generator.IncreaseLevel();
      }
      generator.Generate();
      game_map = generator.GetGameMap();

      size_t num_enemies = 0;
      for (size_t i = 0; i < game_map.size(); i++) {
        for (size_t j = 0; j < game_map[0].size(); j++) {
          if (game_map[i][j] == TileType::Pooka || game_map[i][j] == TileType::Fygar) {
            num_enemies++;
          }
        }
      }

      REQUIRE(num_enemies == 8);
    }
  }
  
  SECTION("Check the length of all the tunnels in the map") {
    bool is_tunnel_length_correct = true;
    for (size_t i = 0; i < game_map.size(); i++) {
      for (size_t j = 0; j < game_map[0].size(); j++) {
        if (game_map[i][j] == TileType::Pooka || game_map[i][j] == TileType::Fygar) {
          if (game_map[i + 1][j] == TileType::Tunnel) {
            for (size_t tunnel_num = 1; tunnel_num < 3; tunnel_num++) {
              if (game_map[i + tunnel_num][j] != TileType::Tunnel) {
                is_tunnel_length_correct = false;
              }
            }
            
          } else if (game_map[i][j + 1] == TileType::Tunnel) {
            for (size_t tunnel_num = 1; tunnel_num < 3; tunnel_num++) {
              if (game_map[i][j + tunnel_num] != TileType::Tunnel) {
                is_tunnel_length_correct = false;
              }
            }
            
          } else {
            is_tunnel_length_correct = false;
          }
        }
      }
    }
    
    REQUIRE(is_tunnel_length_correct);
  }
  
  SECTION("Check that the enemies and tiles do not coincide with the center point of the map for player") {
    size_t mid_value = 7;
    size_t tunnel_size = 3;

    bool is_enemy_in_center = false;
    for (size_t i = 0; i < game_map.size(); i++) {
      for (size_t j = 0; j < game_map[0].size(); j++) {
        if (game_map[i][j] == TileType::Pooka || game_map[i][j] == TileType::Fygar) {
          if (game_map[i + 1][j] == TileType::Tunnel) {
            if (i >= mid_value - tunnel_size && i <= mid_value + 1 && j <= mid_value + 1) {
              is_enemy_in_center = true;
            }

          } else {
            if (i >= mid_value - 1 && i <= mid_value + 1) {
              is_enemy_in_center = true;
            }
          }
        }
      }
    }

    REQUIRE_FALSE(is_enemy_in_center);
  }

  SECTION("Check whether correct number of rocks are on board") {
    size_t num_rocks = 0;
    for (size_t i = 0; i < game_map.size(); i++) {
      for (size_t j = 0; j < game_map[0].size(); j++) {
        if (game_map[i][j] == TileType::Rock) {
          num_rocks++;
        }
      }
    }

    bool is_num_rocks_correct = num_rocks == 3 || num_rocks == 4;
    REQUIRE(is_num_rocks_correct);
  }
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