#include "core/enemy.h"

namespace dig_dug {

Enemy::Enemy(const vec2& position, const vec2& velocity) {
  position_ = position;
  velocity_ = velocity;
}

void Enemy::Move() {
  position_ += velocity_;
}

vec2 Enemy::GetPosition() {
  return position_;
}

void Enemy::SetVelocity(const vec2& velocity) {
  velocity_ = velocity;
}

} // namespace dig_dug