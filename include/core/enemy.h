#pragma once

#include <cinder/gl/gl.h>
#include "game_state_generator.h"

namespace dig_dug {

using glm::vec2;

class Enemy {
 public:

  /**
   * Initializes an enemy
   */
  Enemy(const vec2& position, const vec2& velocity, TileType enemy_type);

  /**
   * Moves the enemy
   */
  void Move();

  vec2 GetPosition();

  void SetPosition(const vec2& position);

  vec2 GetVelocity();

  void SetVelocity(const vec2& velocity);

  bool GetIsGhost();

  void SetIsGhost();

  TileType GetType();

 private:
  vec2 position_;
  vec2 velocity_;
  bool is_ghost_;
  TileType type_;
};
}