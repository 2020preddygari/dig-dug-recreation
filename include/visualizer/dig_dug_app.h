#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "core/game_engine.h"
#include "core/game_state_generator.h"


namespace dig_dug {

using ci::gl::Texture2dRef;
using ci::gl::Texture2d;
using ci::loadImage;

 class DigDugApp : public ci::app::App {
  public:
   /**
    * Constructs the Dig Dug game
    */
   DigDugApp();

   /**
    * Draws the board
    */
   void draw() override;

   /**
    * Updates the board
    */
   void update() override;

   /**
    * Updates the player with respect to the key pressed
    *
    * @param event key pressed
    */
   void keyDown(ci::app::KeyEvent event);

  private:
   GameStateGenerator generator_;
   GameEngine engine_;
   size_t live_lost_num_frames_ = 0;
   bool is_game_over_ = false;
   const size_t kTileSize = 100;
   const size_t kMaxLiveLostFrames = 10;
   const double kWindowSize = 2000;
   const double kMargin = 100;
   const double kBoardToWindowRatio = 0.75;

   /**
    * Draws the dirt tiles, tunnels, and rocks
    */
   void DrawBoard();

   /**
    * Draws the player and the harpoon
    */
   void DrawPlayer();

   /**
    * Draws the enemies
    */
   void DrawEnemies();
};
}