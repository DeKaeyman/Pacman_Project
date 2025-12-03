// app/views/GhostView.cpp
#include "GhostView.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>

namespace pacman::app {

sf::Texture
    GhostView::texture_{}; // Static sprite sheet texture shared by all ghosts
bool GhostView::textureLoaded_{false}; // Ensures texture loads only once

namespace {
constexpr const char *SPRITE_SHEET_PATH = "assets/sprites/sprite.png";

constexpr unsigned int SHEET_COLS = 19;
constexpr unsigned int SHEET_ROWS = 19;

// Each ghost type has its own sprite column
unsigned int columnForKind(pacman::logic::GhostKind kind) {
  using pacman::logic::GhostKind;
  switch (kind) {
  case GhostKind::A:
    return 13;
  case GhostKind::B:
    return 14;
  case GhostKind::C:
    return 15;
  case GhostKind::D:
    return 16;
  }
  return 13;
}

// Animation rows for each movement direction
constexpr unsigned int ROW_RIGHT_1 = 0;
constexpr unsigned int ROW_RIGHT_2 = 1;
constexpr unsigned int ROW_DOWN_1 = 2;
constexpr unsigned int ROW_DOWN_2 = 3;
constexpr unsigned int ROW_LEFT_1 = 4;
constexpr unsigned int ROW_LEFT_2 = 5;
constexpr unsigned int ROW_UP_1 = 6;
constexpr unsigned int ROW_UP_2 = 7;

// Fear mode sprites
constexpr unsigned int FEAR_COL_1 = 0;
constexpr unsigned int FEAR_COL_2 = 1;
constexpr unsigned int FEAR_ROW_1 = 11;
constexpr unsigned int FEAR_ROW_2 = 12;

// Convert a (col,row) index to an SFML IntRect representing that frame
sf::IntRect spriteRectFromGrid(const sf::Texture &tex, unsigned int col,
                               unsigned int row) {
  const auto size = tex.getSize();
  const float cellW =
      static_cast<float>(size.x) / static_cast<float>(SHEET_COLS);
  const float cellH =
      static_cast<float>(size.y) / static_cast<float>(SHEET_ROWS);

  const int left = static_cast<int>(std::round(col * cellW));
  const int top = static_cast<int>(std::round(row * cellH));
  const int right = static_cast<int>(std::round((col + 1u) * cellW));
  const int bottom = static_cast<int>(std::round((row + 1u) * cellH));

  return {left, top, right - left, bottom - top};
}

// Select body animation row based on direction + elapsed animation time
unsigned int pickRowFor(logic::Direction dir, double t) {
  const double speed = 8.0;
  const double phase =
      std::fmod(t * speed, 2.0); // Cycles between two frames [0,2)

  const bool firstFrame =
      (phase < 1.0); // Decide which frame of the pair to use

  switch (dir) {
  case logic::Direction::Right:
    return firstFrame ? ROW_RIGHT_1 : ROW_RIGHT_2;
  case logic::Direction::Down:
    return firstFrame ? ROW_DOWN_1 : ROW_DOWN_2;
  case logic::Direction::Left:
    return firstFrame ? ROW_LEFT_1 : ROW_LEFT_2;
  case logic::Direction::Up:
    return firstFrame ? ROW_UP_1 : ROW_UP_2;
  case logic::Direction::None:
  default:
    return ROW_RIGHT_1; // Idle default frame
  }
}

// Fear mode color alternates between two columns to simulate flashing
unsigned int pickFearColumn(double t) {
  const double flashSpeed = 2.0;
  const double phase = std::fmod(t * flashSpeed, 2.0); // [0,2)
  return (phase < 1.0) ? FEAR_COL_1 : FEAR_COL_2;
}

// Fear mode mouth animation rows
unsigned int pickFearRow(double t) {
  const double animSpeed = 8.0;
  const double phase = std::fmod(t * animSpeed, 2.0); // [0,2)
  return (phase < 1.0) ? FEAR_ROW_1 : FEAR_ROW_2;
}
} // namespace

void GhostView::ensureTextureLoaded() {
  if (textureLoaded_)
    return; // Already loaded
  if (!texture_.loadFromFile(SPRITE_SHEET_PATH))
    return;              // Fail silently if missing
  textureLoaded_ = true; // mark texture as accessible
}

GhostView::GhostView(const std::shared_ptr<pacman::logic::Ghost> &model)
    : model_(model) {
  ensureTextureLoaded();

  if (textureLoaded_) {
    sprite_.setTexture(texture_); // Assign sprite sheet
    updateSpriteFrame();          // Set initial frame
  }
}

void GhostView::onEvent(const pacman::logic::Event &e) {
  using pacman::logic::EventType;
  using pacman::logic::StateChangedPayload;

  if (e.type ==
      EventType::StateChanged) { // Only listen for direction/fear changes
    if (const auto *payload = std::get_if<StateChangedPayload>(&e.payload)) {
      switch (payload->code) { // Model encodes direction or fear trigger as
                               // integer codes

      // Movement direction changes
      case 0:
        direction_ = logic::Direction::Right;
        break;
      case 1:
        direction_ = logic::Direction::Left;
        break;
      case 2:
        direction_ = logic::Direction::Up;
        break;
      case 3:
        direction_ = logic::Direction::Down;
        break;

      // Enter/exit fear mode
      case 100:
        fearMode_ = true;
        break;
      case 101:
        fearMode_ = false;
        break;

      default:
        break;
      }
    }
  }
}

void GhostView::updateSpriteFrame() {
  if (!textureLoaded_)
    return; // No sprite sheet loaded
  if (!model_)
    return; // Safety check

  auto &sw = pacman::logic::Stopwatch::getInstance(); // Global stopwatch for
                                                      // animation time
  const double t = sw.elapsed();                      // Time since game start

  unsigned int col = 0; // Sprite grid column
  unsigned int row = 0; // Sprite grid row

  if (fearMode_) { // Use fear mode animation
    col = pickFearColumn(t);
    row = pickFearRow(t);
  } else {
    row = pickRowFor(direction_, t);     // Pick animation by direction
    col = columnForKind(model_->kind()); // Pick ghost color by type
  }

  const auto rect =
      spriteRectFromGrid(texture_, col, row); // Get final sprite frame
  sprite_.setTextureRect(rect);               // Apply sprite rectangle

  sprite_.setColor(sf::Color::White); // Ensure no tint is applied
}

void GhostView::draw(sf::RenderWindow &window) {
  if (!model_)
    return; // No model available
  if (!model_->active)
    return; // Don't draw inactive ghost
  if (!camera_)
    return; // Need camera for projection
  if (!textureLoaded_)
    return; // Texture missing

  updateSpriteFrame(); // Calculate latest animation frame

  pacman::logic::Rect r = model_->bounds();  // world space bounding box
  auto pixelRect = camera_->worldToPixel(r); // Convert to pixel space

  const auto &texRect = sprite_.getTextureRect(); // Size of sprite frame

  float scale =
      static_cast<float>(pixelRect.w) /
      static_cast<float>(texRect.width); // Uniform scale to fit tile width

  float finalW = texRect.width * scale;  // Actual drawn width
  float finalH = texRect.height * scale; // Actual drawn height

  float posX = static_cast<float>(pixelRect.x) +
               (pixelRect.w - finalW) * 0.5f; // Center horizontally
  float posY = static_cast<float>(pixelRect.y) +
               (pixelRect.h - finalH) * 0.5f; // Center vertically

  sprite_.setPosition(posX, posY); // Move sprite to screen position
  sprite_.setScale(scale, scale);  // Apply scale

  window.draw(sprite_); // Render ghost
}
} // namespace pacman::app