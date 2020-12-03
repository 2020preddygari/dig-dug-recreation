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

  vec2 GetPosition();

  void SetPosition(const vec2& position);

  vec2 GetVelocity();

  void SetVelocity(const vec2& velocity);

  bool GetIsGhost();

  void SetIsGhost();

  TileType GetType();

  bool GetIsHurt();

  void SetIsHurt(bool val);

  CharacterOrientation GetOrientation();

  bool GetIsInDirt();

  void SetIsInDirt(bool in_dirt);

 private:
  vec2 position_;
  vec2 velocity_;
  bool is_ghost_ = false;
  bool is_in_dirt_ = false;
  bool is_hurt_ = false;
  TileType type_;
  CharacterOrientation orientation_ = CharacterOrientation::Right;
};
}