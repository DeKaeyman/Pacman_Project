// logic/entities/Wall.h
#pragma once

#include "Entity.h"

namespace pacman::logic {

    class Wall : public Entity {
    public:
        explicit Wall(const Rect& bounds) : bounds_(bounds) {
            solid  = true;
            active = true;
        }

        Rect bounds() const override { return bounds_; }

        void update(double /*dt*/) override {}

        void setBounds(const Rect& r) noexcept { bounds_ = r; }

    private:
        Rect bounds_{};
    };
}
