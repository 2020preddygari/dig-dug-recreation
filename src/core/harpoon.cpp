#include "core/harpoon.h"

namespace dig_dug {


Harpoon::Harpoon(const vec2& start_pos, const vec2& velocity) {
  arrow_ = start_pos;
  start_position_ = start_pos;
  velocity_ = velocity;
}

void Harpoon::Move() {
  arrow_ += velocity_;
  num_frames_shot++;
}

vec2 Harpoon::GetArrowPosition() {
  return arrow_;
}

size_t Harpoon::GetNumFramesShot() {
  return num_frames_shot;
}

} //namespace dig_dug