#include "visualizer/dig_dug_app.h"

namespace dig_dug {

DigDugApp::DigDugApp() {
  srand((unsigned int) (time(NULL)));
  ci::app::setWindowSize((int) (kWindowSize), (int) (kWindowSize));

  generator_.Generate();
  engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
}

void DigDugApp::draw() {
  ci::Color8u background_color(0, 0, 0);
  ci::gl::clear(background_color);
  Rectf game_board_background({kMargin, kMargin},{kMargin + kBoardToWindowRatio * kWindowSize,
                                                      kMargin + kBoardToWindowRatio * kWindowSize});
  ci::gl::color(ci::Color("white"));
  ci::gl::drawStrokedRect(game_board_background);

  DrawBoard();
  DrawPlayer();
  DrawEnemies();
}

void DigDugApp::update() {

}

void DigDugApp::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_SPACE:
      engine_.AttackEnemy();
      break;

    case KeyEvent::KEY_RIGHT:
      engine_.MovePlayer({1, 0});
      break;

    case KeyEvent::KEY_DOWN:
      engine_.MovePlayer({0, 1});
      break;

    case KeyEvent::KEY_LEFT:
      engine_.MovePlayer({-1, 0});
      break;

    case KeyEvent::KEY_UP:
      engine_.MovePlayer({0, -1});
      break;
  }
}

void DigDugApp::DrawBoard() {
  vector<vector<TileType>> game_map = engine_.GetGameMap();
  size_t size = game_map.size();

  for (size_t x = 0; x < size; x++) {
    for (size_t y = 0; y < size; y++) {
      if (game_map[x][y] == TileType::Dirt || game_map[x][y] == TileType::Rock) {
        size_t x_pixel_val = x * kTileSize;
        size_t y_pixel_val = y * kTileSize;
        Rectf block({x_pixel_val + kMargin, y_pixel_val + kMargin},
                    {x_pixel_val + kTileSize + kMargin, y_pixel_val + kTileSize + kMargin});

        ci::gl::draw(kDirtTexture, block);

        if (game_map[x][y] == TileType::Rock) {
          ci::gl::draw(kRockTexture, block);
        }
      }
    }
  }
}

void DigDugApp::DrawPlayer() {
  Player player = engine_.GetPlayer();
  vec2 position = player.GetPosition();

  Rectf player_rect({position.x + kMargin, position.y + kMargin},
                    {position.x + kTileSize + kMargin, position.y + kTileSize + kMargin});

  if (player.GetOrientation() == CharacterOrientation::Right) {
    ci::gl::draw(kPlayerRightTexture, player_rect);
  } else {
    ci::gl::draw(kPlayerLeftTexture, player_rect);
  }

  if (engine_.GetIsAttacking()) {
    DrawHarpoon();
  }
}

void DigDugApp::DrawEnemies() {

}

void DigDugApp::DrawHarpoon() {
  Player player = engine_.GetPlayer();
  vec2 position = player.GetPosition();

  Harpoon harpoon = engine_.GetHarpoon();
  vec2 velocity = harpoon.GetVelocity();
  vec2 arrow_pos = harpoon.GetArrowPosition();

  if (velocity.x > 0 && velocity.y == 0) {
    Rectf harpoon_rect ({position.x + kTileSize + kMargin, position.y + kMargin},
                        {arrow_pos.x + kTileSize + kMargin, arrow_pos.y + kTileSize + kMargin});
    ci::gl::draw(kHarpoonRightTexture, harpoon_rect);

  } else if (velocity.x < 0 && velocity.y == 0) {
    Rectf harpoon_rect ({arrow_pos.x + kMargin, arrow_pos.y + kMargin},
                        {position.x + kMargin, position.y + kTileSize + kMargin});
    ci::gl::draw(kHarpoonLeftTexture, harpoon_rect);

  } else if (velocity.y > 0 && velocity.x == 0) {
    Rectf harpoon_rect({position.x + kMargin, position.y + kTileSize + kMargin},
                       {arrow_pos.x + kTileSize + kMargin, arrow_pos.y + kTileSize + kMargin});
    ci::gl::draw(kHarpoonDownTexture, harpoon_rect);

  } else {
    Rectf harpoon_rect({arrow_pos.x + kMargin, arrow_pos.y + kMargin},
                       {position.x + kTileSize + kMargin, position.y + kTileSize + kMargin});
    ci::gl::draw(kHarpoonUpTexture, harpoon_rect);
  }
}

} // namespace dig_dug