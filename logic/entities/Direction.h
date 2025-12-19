#pragma once

namespace pacman::logic {

/**
 * @brief Enumeration of movement directions used by entities.
 */
enum class Direction {
    None, ///< No movement
    Up,   ///< Upward movement (negative Y)
    Down, ///< Downward movement (positive Y)
    Left, ///< Leftward movement (negative X)
    Right ///< Rightward movement (positive X)
};

/**
 * @brief Returns the X-axis unit step for a given direction.
 * @param direction Movement direction.
 * @return -1.0 for Left, +1.0 for Right, 0.0 otherwise.
 */
inline float dirToDx(Direction direction) {
    switch (direction) {
    case Direction::Left:
        return -1.0f;
    case Direction::Right:
        return 1.0f;
    default:
        return 0.0f;
    }
}

/**
 * @brief Returns the Y-axis unit step for a given direction.
 * @param direction Movement direction.
 * @return -1.0 for Up, +1.0 for Down, 0.0 otherwise.
 */
inline float dirToDy(Direction direction) {
    switch (direction) {
    case Direction::Up:
        return -1.0f;
    case Direction::Down:
        return 1.0f;
    default:
        return 0.0f;
    }
}

} // namespace pacman::logic