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
using ci::app::KeyEvent;

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

   const Texture2dRef kDirtTexture = Texture2d::create(loadImage("../../../images/dirt_block.png"));
   const Texture2dRef kRockTexture = Texture2d::create(loadImage("../../../images/rock.png"));
   const Texture2dRef kPlayerLeftTexture = Texture2d::create(loadImage("../../../images/dig_dug_player_left.png"));
   const Texture2dRef kPlayerRightTexture = Texture2d::create(loadImage("../../../images/dig_dug_player_right.png"));
   const Texture2dRef kHarpoonDownTexture = Texture2d::create(loadImage("../../../images/harpoon_down.png"));
   const Texture2dRef kHarpoonLeftTexture = Texture2d::create(loadImage("../../../images/harpoon_left.png"));
   const Texture2dRef kHarpoonRightTexture = Texture2d::create(loadImage("../../../images/harpoon_right.png"));
   const Texture2dRef kHarpoonUpTexture = Texture2d::create(loadImage("../../../images/harpoon_up.png"));
   const Texture2dRef kFygarLeftTexture = Texture2d::create(loadImage("../../../images/fygar_left.png"));
   const Texture2dRef kFygarRightTexture = Texture2d::create(loadImage("../../../images/fygar_right.png"));
   const Texture2dRef kPookaLeftTexture = Texture2d::create(loadImage("../../../images/pooka_left.png"));
   const Texture2dRef kPookaRightTexture = Texture2d::create(loadImage("../../../images/pooka_right.png"));
   const Texture2dRef kGhostTexture = Texture2d::create(loadImage("../../../images/ghost.png"));


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

   /**
    * Draws the harpoon
    */
   void DrawHarpoon();
};
}