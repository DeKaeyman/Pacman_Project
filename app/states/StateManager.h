#pragma once

#include "AppContext.h"
#include "State.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pacman::app {

/**
 * @brief Manages a stack of application states and supports deferred transitions.
 *
 * State transitions (push/replace/pop/clear) are queued and applied after the
 * current state finishes handling events or updating, preventing mutation during callbacks.
 */
class StateManager {
public:
    /**
     * @brief Identifier type used for state factories.
     */
    using Id = std::string;

    /**
     * @brief Factory function type that constructs a state instance.
     */
    using Factory = std::function<std::unique_ptr<State>(StateManager&)>;

    /**
     * @brief Registers a factory under a given identifier.
     * @param id The identifier used to create the state later.
     * @param factory A callable that constructs the state.
     */
    void registerFactory(const Id& id, Factory factory);

    /**
     * @brief Schedules pushing a new state on top of the stack.
     * @param id The identifier of the state to push.
     */
    void push(const Id& id);

    /**
     * @brief Schedules replacing the current top state with a new one.
     * @param id The identifier of the state to replace with.
     */
    void replace(const Id& id);

    /**
     * @brief Schedules popping the current top state.
     */
    void pop();

    /**
     * @brief Schedules clearing the full state stack.
     */
    void clear();

    /**
     * @brief Forwards an event to the active state and applies pending transitions.
     * @param event The SFML event.
     */
    void handleEvent(const sf::Event& event);

    /**
     * @brief Updates the active state and applies pending transitions.
     * @param dt Fixed timestep in seconds.
     */
    void update(double dt);

    /**
     * @brief Draws the active state.
     * @param window The render window to draw to.
     */
    void draw(sf::RenderWindow& window);

    /**
     * @brief Checks whether the state stack is empty.
     * @return True if there are no states on the stack.
     */
    bool empty() const { return stack_.empty(); }

    /**
     * @brief Returns the number of states currently on the stack.
     * @return The stack size.
     */
    std::size_t size() const { return stack_.size(); }

public:
    /**
     * @brief Shared context accessible to all states.
     */
    AppContext ctx;

private:
    /**
     * @brief Type of deferred stack mutation to apply.
     */
    enum class ActionType { Push, Replace, Pop, Clear };

    /**
     * @brief A deferred stack action.
     */
    struct Action {
        ActionType type;
        Id id;
    };

    /**
     * @brief Applies all queued actions to the state stack.
     */
    void applyPending();

    /**
     * @brief Creates a new state instance via the registered factory for the given identifier.
     * @param id The identifier of the state to create.
     * @return A newly constructed state instance.
     */
    std::unique_ptr<State> make(const Id& id);

private:
    std::vector<Action> pending_;
    std::vector<std::unique_ptr<State>> stack_;
    std::unordered_map<Id, Factory> factories_;
};

} // namespace pacman::app