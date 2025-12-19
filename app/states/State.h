#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

namespace pacman::app {

class StateManager;

/**
 * @brief Abstract base class for all application states.
 *
 * States represent high-level screens such as menus, gameplay,
 * pause screens, or end screens. They are managed through a
 * stack-based StateManager.
 */
class State {
public:
    /**
     * @brief String identifier type used to reference state factories.
     */
    using Id = std::string;

    /**
     * @brief Constructs a state with access to the StateManager.
     * @param manager Reference to the central StateManager.
     */
    explicit State(StateManager& manager);

    /**
     * @brief Virtual destructor to allow safe polymorphic destruction.
     */
    virtual ~State() = default;

    /**
     * @brief Handles input events forwarded from the game loop.
     * @param event The SFML event to process.
     */
    virtual void handleEvent(const sf::Event& event);

    /**
     * @brief Updates the state logic.
     * @param dt Fixed time step in seconds.
     */
    virtual void update(double dt);

    /**
     * @brief Draws the state contents to the given render window.
     * @param window Render target for drawing.
     */
    virtual void draw(sf::RenderWindow& window);

protected:
    /**
     * @brief Pushes a new state on top of the state stack.
     * @param id Identifier of the state to push.
     */
    void push(const Id& id);

    /**
     * @brief Replaces the current top state with a new one.
     * @param id Identifier of the state to replace with.
     */
    void replace(const Id& id);

    /**
     * @brief Pops the current state from the state stack.
     */
    void pop();

    /**
     * @brief Clears all states from the state stack.
     */
    void clear();

protected:
    StateManager& manager_;
};

} // namespace pacman::app