#include "StateManager.h"
#include "State.h"
#include <stdexcept>

namespace pacman::app {

void StateManager::registerFactory(const Id& id, Factory f) {
    factories_[id] = std::move(f); // Registers a construction function for a given ID
}

std::unique_ptr<State> StateManager::make(const Id& id) {
    auto it = factories_.find(id);
    if (it == factories_.end())
        throw std::runtime_error("Unknown state id: " + id); // If id not found, return error message
    return (it->second)(*this);                              // Calls the saved factory function to create a new state
}

void StateManager::push(const Id& id) {
    pending_.push_back(Action{ActionType::Push, id}); // Make a new state via the factory and push them
                                // on top of the stack
}

void StateManager::replace(const Id& id) {
    pending_.push_back(Action{ActionType::Replace, id}); // Place the new state on top of the stack via the make function
}

void StateManager::pop() {
    pending_.push_back(Action{ActionType::Pop, {}}); // Remove the top state and return to the previous one
}

void StateManager::clear() {
    pending_.push_back(Action{ActionType::Clear, {}}); // Clear the full stack
}

void StateManager::applyPending() {
    for (auto& a : pending_) {
        switch (a.type) {
        case ActionType::Clear:
            stack_.clear();
            break;
        case ActionType::Pop:
            if (!stack_.empty())
                stack_.pop_back();
            break;
        case ActionType::Replace:
            if (!stack_.empty())
                stack_.pop_back();
            stack_.push_back(make(a.id));
            break;
        case ActionType::Push:
            stack_.push_back(make(a.id));
            break;
        }
    }
    pending_.clear();
}

void StateManager::handleEvent(const sf::Event& e) {
    if (!stack_.empty())
        stack_.back()->handleEvent(e); // Send the user input to the current top stack

    applyPending();
}

void StateManager::update(double dt) {
    if (!stack_.empty())
        stack_.back()->update(dt); // Only allow the top state to execute logic

    applyPending();
}

void StateManager::draw(sf::RenderWindow& w) {
    if (!stack_.empty())
        stack_.back()->draw(w); // only draw the top state
}
} // namespace pacman::app