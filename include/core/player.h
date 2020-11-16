#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;

class Player {
 public:

  /**
   * Initializes a player at the specified coordinate
   */
  Player(const vec2& position);

  /**
   * Moves the player using the specified velocity
   */
  void Move(const vec2& velocity);

  vec2 GetPosition();

 private:
  vec2 position_;
};
}