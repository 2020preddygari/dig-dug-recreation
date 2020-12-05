#include "visualizer/dig_dug_app.h"

namespace dig_dug {

DigDugApp::DigDugApp() {
  srand(1);
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
  ci::gl::drawStringCentered("Level " + std::to_string(generator_.GetLevel()),
                             {kWindowSize / 2, kMargin / 4}, ci::Color("white"),
                             ci::Font("Helvetica Neue", (float) (kMargin * 3 / 4)));

  DrawLives();
  DrawBoard();
  DrawPlayer();
  DrawEnemies();
}

void DigDugApp::update() {
  vector<Enemy> enemies = engine_.GetEnemies();

  if (enemies.empty()) {
    live_lost_num_frames_ = 0;
    generator_.IncreaseLevel();
    generator_.Generate();
    engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
  }

  if (engine_.GetLives() > 0 && live_lost_num_frames_ == 0) {
    if (engine_.IsPlayerDead()) {
      live_lost_num_frames_++;
    } else {
      engine_.MoveEnemies();
    }

  } else if (live_lost_num_frames_ > 0){
    live_lost_num_frames_++;

    if (live_lost_num_frames_ > kMaxLiveLostFrames) {
      live_lost_num_frames_ = 0;
      size_t new_lives = engine_.GetLives();
      generator_.Generate();
      engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
      engine_.SetLives(new_lives);
    }

  } else {
    is_game_over_ = true;
  }
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
  vector<Enemy> enemies = engine_.GetEnemies();

  for (Enemy enemy : enemies)  {
    vec2 position = enemy.GetPosition();
    TileType type = enemy.GetType();
    CharacterOrientation orientation = enemy.GetOrientation();
    Rectf enemy_rect({position.x + kMargin, position.y + kMargin},
                     {position.x + kTileSize + kMargin, position.y + kTileSize + kMargin});

    if (enemy.GetIsGhost()) {
      ci::gl::draw(kGhostTexture, enemy_rect);

    } else if (type == TileType::Fygar) {
      if (orientation == CharacterOrientation::Left) {
        ci::gl::draw(kFygarLeftTexture, enemy_rect);
      } else {
        ci::gl::draw(kFygarRightTexture, enemy_rect);
      }

    } else {
      if (orientation == CharacterOrientation::Left) {
        ci::gl::draw(kPookaLeftTexture, enemy_rect);
      } else {
        ci::gl::draw(kPookaRightTexture, enemy_rect);
      }
    }
  }
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

void DigDugApp::DrawLives() {
  const size_t kPlayerWidth = 90;
  const size_t kPlayerHeight = 90;
  double end_of_game_board = kMargin + kBoardToWindowRatio * kWindowSize;

  for (size_t life = 0; life < engine_.GetLives(); life++) {
    double start_x = end_of_game_board + kMargin / 2 + life * 100;
    Rectf player_rect({start_x, kMargin}, {start_x + kPlayerWidth, kMargin + kPlayerHeight});
    ci::gl::draw(kPlayerRightTexture, player_rect);
  }
}

} // namespace dig_dug