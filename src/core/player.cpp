#include "core/player.h"

namespace dig_dug {

Player::Player(const vec2& position) {
  position_ = position;
}

void Player::Move(const vec2& velocity) {
  vec2 kZeroVector = {0, 0};
  position_ += velocity;

  if (velocity != kZeroVector) {
    prev_velocity_ = velocity;
  }
}

vec2 Player::GetPosition() {
  return position_;
}

vec2 Player::GetPrevVelocity() {
  return prev_velocity_;
}


} // namespace dig_dug