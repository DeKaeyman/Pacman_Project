// logic/entities/PacMan.cpp
#include "PacMan.h"
#include "../observer/Event.h"

namespace pacman::logic {

    PacMan::PacMan(const pacman::logic::Rect &startBounds, double speed) : bounds_(startBounds), speed_(speed) {
        solid = true; // Pacman collides with walls/ghosts
        active = true; // Pacman is alive/visible
    }

    void PacMan::update(double dt) {
        if (!active) return;
        if (direction_ == Direction::None) return;

        // Unit direction vector in world space
        float vx = dirToDx(direction_);
        float vy = dirToDy(direction_);

        // Distance travelled this frame
        float dist = static_cast<float>(speed_ * dt);

        bounds_.x += vx * dist;
        bounds_.y += vy * dist;
    }

    void PacMan::setDirection(Direction dir) noexcept {
        if (direction_ == dir) return;
        direction_ = dir;

        using pacman::logic::EventType;
        using pacman::logic::StateChangedPayload;
        using pacman::logic::Event;

        StateChangedPayload payload{};
        switch (direction_) {
            case Direction::Right:
                payload.code = 0;
                break;
            case Direction::Left:
                payload.code = 1;
                break;
            case Direction::Up:
                payload.code = 2;
                break;
            case Direction::Down:
                payload.code = 3;
                break;
            default:
                return;
        }

        Event e{};
        e.type = EventType::StateChanged;
        e.payload = payload;

        notify(e);
    }
}