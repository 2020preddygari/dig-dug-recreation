#include "core/enemy.h"

namespace dig_dug {

Enemy::Enemy(const vec2& position, const vec2& velocity, TileType enemy_type) {
  position_ = position;
  velocity_ = velocity;
  type_ = enemy_type;
}

void Enemy::Move() {
  position_ += velocity_;
}

vec2 Enemy::GetPosition() {
  return position_;
}

void Enemy::SetPosition(const vec2& position) {
  position_ = position;
}

vec2 Enemy::GetVelocity() {
  return velocity_;
}

void Enemy::SetVelocity(const vec2& velocity) {
  velocity_ = velocity;

  if (velocity.x > 0) {
    orientation_ = CharacterOrientation::Right;
  } else if (velocity.x < 0) {
    orientation_ = CharacterOrientation::Left;
  }
}

bool Enemy::GetIsGhost() {
  return is_ghost_;
}

void Enemy::SetIsGhost() {
  is_ghost_ = !is_ghost_;

  if (!is_ghost_) {
    is_in_dirt_ = false;
  }
}

TileType Enemy::GetType() {
  return type_;
}

bool Enemy::GetIsHurt() {
  return is_hurt_;
}

void Enemy::SetIsHurt(bool val) {
  is_hurt_ = val;
}

CharacterOrientation Enemy::GetOrientation() {
  return orientation_;
}

bool Enemy::GetIsInDirt() {
  return is_in_dirt_;
}

void Enemy::SetIsInDirt(bool in_dirt) {
  is_in_dirt_ = in_dirt;
}

} // namespace dig_dug