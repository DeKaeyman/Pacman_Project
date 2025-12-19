#include "State.h"
#include "StateManager.h"

namespace pacman::app {

/**
 * @brief Constructs a state with access to the StateManager.
 * @param manager Reference to the central StateManager.
 */
State::State(StateManager& manager) : manager_(manager) {}

/**
 * @brief Default event handler (does nothing).
 * @param event Incoming SFML event.
 */
void State::handleEvent(const sf::Event& event) { (void)event; }

/**
 * @brief Default update handler (does nothing).
 * @param dt Fixed time step in seconds.
 */
void State::update(double dt) { (void)dt; }

/**
 * @brief Default draw handler (does nothing).
 * @param window Render target.
 */
void State::draw(sf::RenderWindow& window) { (void)window; }

/**
 * @brief Pushes a new state onto the stack.
 * @param id Identifier of the state to push.
 */
void State::push(const Id& id) { manager_.push(id); }

/**
 * @brief Replaces the current state with a new one.
 * @param id Identifier of the state to replace with.
 */
void State::replace(const Id& id) { manager_.replace(id); }

/**
 * @brief Pops the current state from the stack.
 */
void State::pop() { manager_.pop(); }

/**
 * @brief Clears all states from the stack.
 */
void State::clear() { manager_.clear(); }

} // namespace pacman::app