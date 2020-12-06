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

  if (velocity.x > 0) {
    orientation_ = CharacterOrientation::Right;
  } else if (velocity.x < 0) {
    orientation_ = CharacterOrientation::Left;
  }
}

vec2 Player::GetPosition() const {
  return position_;
}

vec2 Player::GetPrevVelocity() const {
  return prev_velocity_;
}

CharacterOrientation Player::GetOrientation() const {
  return orientation_;
}


} // namespace dig_dug