#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

namespace pacman::app {
    class StateManager; // Forward declaration, prevents include loop with StateManager

    class State {
    public:
        using Id = std::string; // Every state type has his own string ID

        explicit State(StateManager& m) : manager_(m) {} // Every state gets a reference to the StateManager
        virtual ~State() = default; // Virtual destructor needed

        void handleEvent(const sf::Event&) {} // Basic behavior
        void update(double /*dt*/) {} // Gets called per frame for animations/timers and so on
        void draw(sf::RenderWindow& /*w*/) {} // Gets called to draw the state itself

    protected:
        // Help functions
        void push(const Id& id);
        void replace(const Id& id);
        void pop();
        void clear();

        StateManager& manager_; // Central access to the state stack
    };
}