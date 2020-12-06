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

vec2 Enemy::GetPosition() const {
  return position_;
}

void Enemy::SetPosition(const vec2& position) {
  position_ = position;
}

vec2 Enemy::GetVelocity() const {
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

bool Enemy::GetIsGhost() const {
  return is_ghost_;
}

void Enemy::SetIsGhost() {
  is_ghost_ = !is_ghost_;

  if (!is_ghost_) {
    is_in_dirt_ = false;
  }
}

TileType Enemy::GetType() const {
  return type_;
}

bool Enemy::GetIsHurt() const {
  return is_hurt_;
}

void Enemy::SetIsHurt(bool val) {
  is_hurt_ = val;
}

CharacterOrientation Enemy::GetOrientation() const{
  return orientation_;
}

bool Enemy::GetIsInDirt() const {
  return is_in_dirt_;
}

void Enemy::SetIsInDirt(bool in_dirt) {
  is_in_dirt_ = in_dirt;
}

} // namespace dig_dug