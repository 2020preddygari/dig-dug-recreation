#pragma once

#include <cinder/gl/gl.h>
#include "game_state_generator.h"
#include "player.h"

namespace dig_dug {

using glm::vec2;
using dig_dug::CharacterOrientation;

class Enemy {
 public:

  /**
   * Initializes an enemy
   */
  Enemy(const vec2& position, const vec2& velocity, TileType enemy_type);

  /**
   * Moves the enemy
   */
  void Move();

  vec2 GetPosition() const;

  void SetPosition(const vec2& position);

  vec2 GetVelocity() const;

  void SetVelocity(const vec2& velocity);

  bool IsGhost() const;

  void SetGhost();

  TileType GetType() const;

  bool IsHurt() const;

  void SetHurt(bool val);

  CharacterOrientation GetOrientation() const;

  bool IsInDirt() const;

  void SetInDirt(bool in_dirt);

 private:
  vec2 position_;
  vec2 velocity_;
  bool ghost_ = false;
  bool in_dirt_ = false;
  bool hurt_ = false;
  TileType type_;
  CharacterOrientation orientation_ = CharacterOrientation::Right;
};
}