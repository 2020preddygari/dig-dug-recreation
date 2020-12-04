#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;

enum class CharacterOrientation {
  Left,
  Right
};

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

  CharacterOrientation GetOrientation();

 private:
  constexpr static double kSpeed = 5;
  vec2 position_;
  vec2 prev_velocity_ {kSpeed, 0};
  CharacterOrientation orientation_ = CharacterOrientation::Right;
};
}