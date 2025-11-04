#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pacman");
    while (window.isOpen()) { // Main loop of the app, keeps running till closed

        sf::Event e{};
        while (window.pollEvent(e)) { // Get all events from this frame
            if (e.type == sf::Event::Closed) window.close(); // React on event of closing the window
        }

        window.clear(); // Clear the back buffer
        window.display(); // Swap back with front buffer
    }
    return 0;
}