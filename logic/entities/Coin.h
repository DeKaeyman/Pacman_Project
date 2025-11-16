// logic/entities/Coin.h
#pragma once

#include "Entity.h"
#include "observer/Subject.h"

namespace pacman::logic {

    class Coin : public Entity, public Subject {
    public:
        explicit Coin(const Rect& area, int value = 10); // Define coin position and score value

        Rect bounds() const override { return area_; } // World space rectangle of coin tile
        void update(double /*dt*/) override {} // Coin has no per frame logic

        int value() const noexcept { return value_; } // Returns how many points this coin adds
        void setBounds(const Rect& r) { area_ = r; } // Move/resize coin if needed
        bool isCollected() const noexcept { return !active; } // A collected coin becomes inactive

        void collect(); // Marks coin collected and sends event

    private:
        Rect area_{}; // World space bounding box of the coin
        int value_{10}; // Score value of the coin
    };
}