#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;

class Player {
 public:
  /**
   * Initializes empty player
   */
  Player() = default;

  /**
   * Initializes a player at the specified coordinate
   */
  Player(const vec2& position);

  /**
   * Moves the player using the specified velocity
   */
  void Move(const vec2& velocity);

  vec2 GetPosition();

  vec2 GetPrevVelocity();

 private:
  vec2 position_;
  vec2 prev_velocity_ {1, 0};
};
}