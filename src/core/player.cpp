#include "core/player.h"

namespace dig_dug {

Player::Player(const vec2 &position) {
  position_ = position;
}

void Player::Move(const vec2 &velocity) {
  position_ += velocity;
}

vec2 Player::GetPosition() {
  return position_;
}


} // namespace dig_dug