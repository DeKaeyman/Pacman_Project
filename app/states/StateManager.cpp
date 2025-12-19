#include "StateManager.h"

#include "State.h"

#include <stdexcept>
#include <utility>

namespace pacman::app {

/**
 * @brief Registers a factory function under a given state identifier.
 * @param id The identifier used to create the state later.
 * @param factory The factory function that constructs the state.
 */
void StateManager::registerFactory(const Id& id, Factory factory) { factories_[id] = std::move(factory); }

/**
 * @brief Creates a state instance using the factory registered for the given identifier.
 * @param id The identifier of the state to create.
 * @return A newly constructed state instance.
 * @throws std::runtime_error If no factory is registered for the given identifier.
 */
std::unique_ptr<State> StateManager::make(const Id& id) {
    const auto it = factories_.find(id);
    if (it == factories_.end()) {
        throw std::runtime_error("Unknown state id: " + id);
    }
    return (it->second)(*this);
}

/**
 * @brief Schedules a push of a new state on top of the stack.
 * @param id The identifier of the state to push.
 */
void StateManager::push(const Id& id) { pending_.push_back(Action{ActionType::Push, id}); }

/**
 * @brief Schedules a replacement of the current top state with a new state.
 * @param id The identifier of the state to replace with.
 */
void StateManager::replace(const Id& id) { pending_.push_back(Action{ActionType::Replace, id}); }

/**
 * @brief Schedules a pop of the current top state.
 */
void StateManager::pop() { pending_.push_back(Action{ActionType::Pop, {}}); }

/**
 * @brief Schedules clearing of the entire state stack.
 */
void StateManager::clear() { pending_.push_back(Action{ActionType::Clear, {}}); }

/**
 * @brief Applies all pending stack actions in order and clears the pending list.
 */
void StateManager::applyPending() {
    for (const auto& action : pending_) {
        switch (action.type) {
        case ActionType::Clear:
            stack_.clear();
            break;

        case ActionType::Pop:
            if (!stack_.empty()) {
                stack_.pop_back();
            }
            break;

        case ActionType::Replace:
            if (!stack_.empty()) {
                stack_.pop_back();
            }
            stack_.push_back(make(action.id));
            break;

        case ActionType::Push:
            stack_.push_back(make(action.id));
            break;
        }
    }

    pending_.clear();
}

/**
 * @brief Forwards an input event to the active (top) state and applies pending actions.
 * @param event The SFML event to forward.
 */
void StateManager::handleEvent(const sf::Event& event) {
    if (!stack_.empty()) {
        stack_.back()->handleEvent(event);
    }
    applyPending();
}

/**
 * @brief Updates the active (top) state and applies pending actions.
 * @param dt Fixed timestep in seconds.
 */
void StateManager::update(double dt) {
    if (!stack_.empty()) {
        stack_.back()->update(dt);
    }
    applyPending();
}

/**
 * @brief Draws the active (top) state.
 * @param window The render window to draw to.
 */
void StateManager::draw(sf::RenderWindow& window) {
    if (!stack_.empty()) {
        stack_.back()->draw(window);
    }
}

} // namespace pacman::app