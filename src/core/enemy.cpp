#include "core/enemy.h"

namespace dig_dug {

Enemy::Enemy(const vec2& position, const vec2& velocity, TileType enemy_type) {
  position_ = position;
  velocity_ = velocity;
  is_ghost_ = false;
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
}

bool Enemy::GetIsGhost() {
  return is_ghost_;
}

void Enemy::SetIsGhost() {
  is_ghost_ = !is_ghost_;
}

TileType Enemy::GetType() {
  return type_;
}

} // namespace dig_dug