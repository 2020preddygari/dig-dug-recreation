#pragma once

#include <cinder/gl/gl.h>

namespace dig_dug {

using glm::vec2;
using ci::Rectf;
using std::vector;

class Harpoon {
 public:

  /**
   * Initializes empty harpoon
   */
  Harpoon() = default;

  /**
   * Initializes the harpoon
   *
   * @param start_pos position of player
   * @param velocity velocity of harpoon
   */
  Harpoon(const vec2& start_pos, const vec2& velocity);

  /**
   * Moves the harpoon based on the velocity of it
   */
  void Move();

  vec2 GetArrowPosition() const;

  double GetDistanceTraveled() const;

  vec2 GetVelocity() const;

 private:
  vec2 arrow_;
  vec2 velocity_;
  double distance_traveled_ = 0;
};

} //namespace dig_dug