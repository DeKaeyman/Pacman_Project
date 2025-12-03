// app/views/FruitView.h
#pragma once

#include "../logic/entities/Fruit.h"
#include "View.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace pacman::app {

class FruitView : public View {
public:
    explicit FruitView(const std::shared_ptr<pacman::logic::Fruit>& model); // Uses model to initialize

    void draw(sf::RenderWindow& window) override;         // Draw fruit sprite to window
    void onEvent(const pacman::logic::Event& e) override; // React to fruit collection

private:
    std::shared_ptr<pacman::logic::Fruit> model_; // Logical model this view represents
    sf::Sprite sprite_;                           // Visual representation of fruit
    bool visible_{true};                          // True until collected

    static sf::Texture texture_; // Shared fruit texture
    static bool textureLoaded_;  // Load-once guard

    static void ensureTextureLoaded(); // Loads sprite sheet if needed
};
} // namespace pacman::app