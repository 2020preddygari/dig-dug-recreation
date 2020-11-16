#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;

class Enemy {
 public:

  /**
   * Initializes an enemy at the specified coordinate
   */
  Enemy(const vec2& position);

  /**
   * Moves the enemy using the specified velocity
   */
  void Move(const vec2& velocity);

  vec2 GetPosition();

 private:
  vec2 position_;
};
}