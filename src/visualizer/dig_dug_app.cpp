#include "visualizer/dig_dug_app.h"

namespace dig_dug {

DigDugApp::DigDugApp() {
  srand((unsigned int) (time(0)));
  ci::app::setWindowSize((int) (kWindowSize), (int) (kWindowSize));

  generator_.Generate();
  engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
}

void DigDugApp::draw() {
  const double kGameOverScreenXFraction = 1.0 / 2.0;
  const double kGameOverScreenYFraction = 3.0 / 8.0;
  const double kStartNewGameXFraction = 1.0 / 2.0;
  const double kStartNewGameYFraction = 4.0 / 5.0;
  const double kGameOverSize = 1.0 / 5.0;
  const double kStartNewGameSize = 1.0 / 16.0;

  const double kLevelScreenXFraction = 1.0 / 2.0;
  const double kLevelScreenYFraction = 1.0 / 4.0;
  const double kScoreScreenXFraction = 1.0 / 2.0;
  const double kScoreScreenYFraction = 3.0 / 8.0;
  const double kLevelSize = 3.0 / 4.0;
  const double kScoreSize = 3.0 / 4.0;

  ci::Color8u background_color(0, 0, 0);
  ci::gl::clear(background_color);

  // Draws the game over screen
  if (game_over_) {
    ci::gl::drawStringCentered("Game Over",
                               {kWindowSize * kGameOverScreenXFraction, kWindowSize * kGameOverScreenYFraction},
                               ci::Color("red"),
                               ci::Font("Helvetica Neue", (float) (kWindowSize * kGameOverSize)));

    ci::gl::drawStringCentered("Press enter to start a new game",
                               {kWindowSize * kStartNewGameXFraction, kWindowSize * kStartNewGameYFraction},
                               ci::Color("white"),
                               ci::Font("Helvetica Neue", (float) (kWindowSize * kStartNewGameSize)));
  } else {
    Rectf game_board_background({kMargin, kMargin},{kMargin + kBoardToWindowRatio * kWindowSize,
                                                    kMargin + kBoardToWindowRatio * kWindowSize});
    ci::gl::color(ci::Color("white"));
    ci::gl::drawStrokedRect(game_board_background);
    ci::gl::drawStringCentered("Level " + std::to_string(generator_.GetLevel()),
                               {kWindowSize * kLevelScreenXFraction, kMargin * kLevelScreenYFraction},
                               ci::Color("white"),
                               ci::Font("Helvetica Neue", (float) (kMargin * kLevelSize)));

    ci::gl::drawStringCentered("Score: " + std::to_string(engine_.GetScore()),
                               {kWindowSize - (kWindowSize - kWindowSize * kBoardToWindowRatio)
                               * kScoreScreenXFraction,kWindowSize * kScoreScreenYFraction},
                               ci::Color("white"),
                               ci::Font("Helvetica Neue", (float) (kMargin * kScoreSize)));

    DrawLives();
    DrawBoard();
    DrawPlayer();
    DrawEnemies();
  }
}

void DigDugApp::update() {
  if (game_over_) {
    return;
  }

  vector<Enemy> enemies = engine_.GetEnemies();

  if (enemies.empty()) {
    size_t num_lives = engine_.GetNumLives();
    size_t score = engine_.GetScore();
    live_lost_num_frames_ = 0;
    generator_.IncreaseLevel();
    generator_.Generate();
    engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
    engine_.SetNumLives(num_lives);
    engine_.SetScore(score + kLevelUpScore);

  } else if (engine_.GetNumLives() > 0 && live_lost_num_frames_ == 0) {
    if (engine_.IsPlayerDead()) {
      live_lost_num_frames_++;
    } else {
      engine_.MoveEnemies();
    }

  } else if (live_lost_num_frames_ > 0){
    live_lost_num_frames_++;

    if (live_lost_num_frames_ > kMaxLiveLostFrames) {
      live_lost_num_frames_ = 0;
      size_t new_lives = engine_.GetNumLives();
      size_t score = engine_.GetScore();
      generator_.Generate();
      engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
      engine_.SetNumLives(new_lives);
      engine_.SetScore(score);
    }

  } else {
    game_over_ = true;
  }
}

void DigDugApp::keyDown(KeyEvent event) {
  vec2 right_velocity {1, 0};
  vec2 left_velocity {-1, 0};
  vec2 up_velocity {0, -1};
  vec2 down_velocity {0, 1};

  switch (event.getCode()) {
    case KeyEvent::KEY_SPACE:
      engine_.AttackEnemy();
      break;

    case KeyEvent::KEY_RIGHT:
      engine_.MovePlayer(right_velocity);
      break;

    case KeyEvent::KEY_DOWN:
      engine_.MovePlayer(down_velocity);
      break;

    case KeyEvent::KEY_LEFT:
      engine_.MovePlayer(left_velocity);
      break;

    case KeyEvent::KEY_UP:
      engine_.MovePlayer(up_velocity);
      break;

    case KeyEvent::KEY_RETURN:
      generator_ = GameStateGenerator();
      generator_.Generate();
      engine_ = GameEngine(generator_.GetGameMap(), kTileSize);
      game_over_ = false;
      break;
  }
}

void DigDugApp::DrawBoard() const {
  vector<vector<TileType>> game_map = engine_.GetGameMap();
  size_t size = game_map.size();

  for (size_t x = 0; x < size; x++) {
    for (size_t y = 0; y < size; y++) {
      TileType tile = game_map[x][y];
      if (tile == TileType::Dirt || tile == TileType::Rock) {
        size_t x_pixel_val = x * kTileSize;
        size_t y_pixel_val = y * kTileSize;
        Rectf block({x_pixel_val + kMargin, y_pixel_val + kMargin},
                    {x_pixel_val + kTileSize + kMargin, y_pixel_val + kTileSize + kMargin});

        ci::gl::draw(kDirtTexture, block);

        if (tile == TileType::Rock) {
          ci::gl::draw(kRockTexture, block);
        }
      }
    }
  }
}

void DigDugApp::DrawPlayer() const {
  Player player = engine_.GetPlayer();
  vec2 position = player.GetPosition();

  Rectf player_rect({position.x + kMargin, position.y + kMargin},
                    {position.x + kTileSize + kMargin, position.y + kTileSize + kMargin});

  if (player.GetOrientation() == CharacterOrientation::Right) {
    ci::gl::draw(kPlayerRightTexture, player_rect);
  } else {
    ci::gl::draw(kPlayerLeftTexture, player_rect);
  }

  if (engine_.IsPlayerAttacking()) {
    DrawHarpoon();
  }
}

void DigDugApp::DrawEnemies() const {
  vector<Enemy> enemies = engine_.GetEnemies();

  for (Enemy enemy : enemies)  {
    vec2 position = enemy.GetPosition();
    TileType type = enemy.GetType();
    CharacterOrientation orientation = enemy.GetOrientation();
    Rectf enemy_rect({position.x + kMargin, position.y + kMargin},
                     {position.x + kTileSize + kMargin, position.y + kTileSize + kMargin});

    if (enemy.IsGhost()) {
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

void DigDugApp::DrawHarpoon() const {
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

void DigDugApp::DrawLives() const {
  const size_t kPlayerWidth = 90;
  const size_t kPlayerHeight = 90;
  const double kDifferenceBetweenPlayerImages = 100.0;
  const size_t kMarginDivisor = 2;
  double end_of_game_board = kMargin + kBoardToWindowRatio * kWindowSize;

  for (size_t life = 0; life < engine_.GetNumLives(); life++) {
    double start_x = end_of_game_board + kMargin / kMarginDivisor + life * kDifferenceBetweenPlayerImages;
    Rectf player_rect({start_x, kMargin}, {start_x + kPlayerWidth, kMargin + kPlayerHeight});
    ci::gl::draw(kPlayerRightTexture, player_rect);
  }
}

} // namespace dig_dug