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

TEST_CASE("Moving the player") {
  GameStateGenerator generator;
  generator.Generate();
  GameEngine engine (generator.GetGameMap(), 100);

  SECTION("Test player position") {
    SECTION("Move right") {
      vec2 new_position {701, 700};
      engine.MovePlayer({1, 0});
      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);

      SECTION("Don't move right at boundary") {
        for (size_t move = 0; move < 700; move++) {
          engine.MovePlayer({1, 0});
        }
        engine.MovePlayer({1, 0});

        player = engine.GetPlayer();
        new_position = {1400, 700};
        REQUIRE(player.GetPosition() == new_position);
      }
    }

    SECTION("Move down") {
      vec2 new_position {700, 701};
      engine.MovePlayer({0, 1});
      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);

      SECTION("Don't move down at boundary") {
        for (size_t move = 0; move < 700; move++) {
          engine.MovePlayer({0, 1});
        }
        engine.MovePlayer({0, 1});

        player = engine.GetPlayer();
        new_position = {700, 1400};
        REQUIRE(player.GetPosition() == new_position);
      }
    }

    SECTION("Move left") {
      vec2 new_position {699, 700};
      engine.MovePlayer({-1, 0});
      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);

      SECTION("Don't move left at boundary") {
        for (size_t move = 0; move < 700; move++) {
          engine.MovePlayer({-1, 0});
        }
        engine.MovePlayer({-1, 0});

        player = engine.GetPlayer();
        new_position = {0, 700};
        REQUIRE(player.GetPosition() == new_position);
      }
    }

    SECTION("Move up") {
      vec2 new_position {700, 699};
      engine.MovePlayer({0, -1});
      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);

      SECTION("Don't move up at boundary") {
        for (size_t move = 0; move < 700; move++) {
          engine.MovePlayer({0, -1});
        }
        engine.MovePlayer({0, -1});

        player = engine.GetPlayer();
        new_position = {700, 0};
        REQUIRE(player.GetPosition() == new_position);
      }
    }

    SECTION("Player tries to turn in the middle of tiles") {
      vec2 new_position {702, 700};
      engine.MovePlayer({1, 0});
      engine.MovePlayer({0, 1});
      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);
    }

    SECTION("Player tries to turn in middle of tiles, then turns when it reaches a tile boundary") {
      vec2 new_position {800, 701};

      engine.MovePlayer({1, 0});
      engine.MovePlayer({0, 1});
      for (size_t move = 0; move < 99; move++) {
        engine.MovePlayer({0, 0});
      }

      Player player = engine.GetPlayer();
      REQUIRE(player.GetPosition() == new_position);
    }
  }

  SECTION("Test if player digs through dirt") {
    SECTION("Player digs right") {
      engine.MovePlayer({1, 0});
      vector<vector<TileType>> game_map = engine.GetGameMap();
      REQUIRE(game_map[8][7] == TileType::Tunnel);
    }

    SECTION("Player digs down") {
      engine.MovePlayer({0, 1});
      vector<vector<TileType>> game_map = engine.GetGameMap();
      REQUIRE(game_map[7][8] == TileType::Tunnel);
    }

    SECTION("Player digs left") {
      engine.MovePlayer({-1, 0});
      vector<vector<TileType>> game_map = engine.GetGameMap();
      REQUIRE(game_map[6][7] == TileType::Tunnel);
    }

    SECTION("Player digs up") {
      engine.MovePlayer({0, -1});
      vector<vector<TileType>> game_map = engine.GetGameMap();
      REQUIRE(game_map[7][6] == TileType::Tunnel);
    }
  }
}

TEST_CASE("Testing player death") {
  GameStateGenerator generator;
  generator.Generate();
  GameEngine engine (generator.GetGameMap(), 100);

  SECTION("Testing when player is still alive") {
    REQUIRE_FALSE(engine.CheckPlayerDeath());
  }

  SECTION("Testing player death") {
    Enemy enemy = engine.GetEnemies()[0];
    vec2 enemy_position = enemy.GetPosition();

    for (size_t move = 0; move < 700; move++) {
      engine.MovePlayer({-1, 0});
      engine.MovePlayer({0, -1});
    }

    for (size_t player_x = 0; player_x < enemy_position.x - 50; player_x++) {
      engine.MovePlayer({1, 0});
    }

    for (size_t player_y = 0; player_y <= enemy_position.y; player_y++) {
      engine.MovePlayer({0, 1});
    }

    REQUIRE(engine.CheckPlayerDeath());

    SECTION("Checking if lives were decremented") {
      REQUIRE(engine.GetLives() == 2);
    }
  }
}

TEST_CASE("Testing player attacking") {
  GameStateGenerator generator;
  generator.Generate();
  GameEngine engine (generator.GetGameMap(), 100);

  SECTION("Harpoon extends until length limit") {
    engine.AttackEnemy();

    SECTION("Check that harpoon has launched") {
      REQUIRE(engine.GetIsAttacking());
    }

    engine.AttackEnemy();
    engine.AttackEnemy();
    engine.AttackEnemy();

    SECTION("Check that harpoon has went away") {
      REQUIRE_FALSE(engine.GetIsAttacking());
    }
  }

  SECTION("Harpoon hurts enemy") {
    Enemy enemy = engine.GetEnemies()[0];
    vec2 enemy_position = enemy.GetPosition();

    for (size_t move = 0; move < 700; move++) {
      engine.MovePlayer({-1, 0});
      engine.MovePlayer({0, -1});
    }

    for (size_t player_x = 0; player_x < enemy_position.x; player_x++) {
      engine.MovePlayer({1, 0});
    }

    for (size_t player_y = 0; player_y < enemy_position.y - 200; player_y++) {
      engine.MovePlayer({0, 1});
    }

    engine.AttackEnemy();
    engine.AttackEnemy();
    engine.AttackEnemy();

    enemy = engine.GetEnemies()[0];
    REQUIRE(enemy.GetIsHurt());
  }

  SECTION("Harpoon kills enemy") {
    Enemy enemy = engine.GetEnemies()[0];
    vec2 enemy_position = enemy.GetPosition();

    for (size_t move = 0; move < 700; move++) {
      engine.MovePlayer({-1, 0});
      engine.MovePlayer({0, -1});
    }

    for (size_t player_x = 0; player_x < enemy_position.x; player_x++) {
      engine.MovePlayer({1, 0});
    }

    for (size_t player_y = 0; player_y < enemy_position.y - 200; player_y++) {
      engine.MovePlayer({0, 1});
    }

    for (size_t attack = 0; attack < 5; attack++) {
      engine.AttackEnemy();
    }

    vector<Enemy> enemies = engine.GetEnemies();
    REQUIRE(enemies.size() == 3);
    REQUIRE(engine.GetIsAttacking() == false);
  }
}