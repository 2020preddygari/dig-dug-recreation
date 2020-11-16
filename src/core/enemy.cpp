#include "core/enemy.h"

namespace dig_dug {

Enemy::Enemy(const vec2 &position) {
  position_ = position;
}

void Enemy::Move(const vec2 &velocity) {
  position_ += velocity;
}

vec2 Enemy::GetPosition() {
  return position_;
}


} // namespace dig_dug