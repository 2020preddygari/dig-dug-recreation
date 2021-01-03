#include "core/harpoon.h"

namespace dig_dug {


Harpoon::Harpoon(const vec2& start_pos, const vec2& velocity) {
  arrow_ = start_pos;
  velocity_ = velocity;
}

void Harpoon::Move() {
  arrow_ += velocity_;
  distance_traveled_ += glm::length(velocity_);
}

vec2 Harpoon::GetArrowPosition() const {
  return arrow_;
}

double Harpoon::GetDistanceTraveled() const {
  return distance_traveled_;
}

vec2 Harpoon::GetVelocity() const {
  return velocity_;
}

} //namespace dig_dug