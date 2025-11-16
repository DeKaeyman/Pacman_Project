// logic/entities/Direction.h
#pragma once

namespace pacman::logic {

    // Movement directions for Pacman and ghosts
    enum class Direction {
        None,
        Up,
        Down,
        Left,
        Right
    };

    // X component of a unit step in the given direction
    inline float dirToDx(Direction d) {
        switch (d) {
            case Direction::Left:
                return -1.0f;
            case Direction::Right:
                return 1.0f;
            default:
                return 0.0f;
        }
    }

    // Y component of a unit step in the given direction
    inline float dirToDy(Direction d) {
        switch (d) {
            case Direction::Up:
                return 1.0f;
            case Direction::Down:
                return -1.0f;
            default:
                return 0.0f;
        }
    }
}