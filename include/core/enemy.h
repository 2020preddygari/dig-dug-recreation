#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;

class Enemy {
 public:

  /**
   * Initializes an enemy at the specified coordinate
   */
  Enemy(const vec2& position, const vec2& velocity);

  /**
   * Moves the enemy using the specified velocity
   */
  void Move();

  vec2 GetPosition();

  vec2 GetVelocity();

  bool GetIsGhost();

  void SetVelocity(const vec2& velocity);

  void SetIsGhost();

 private:
  vec2 position_;
  vec2 velocity_;
  bool is_ghost_;
};
}