#pragma once

#include "../entities/Entity.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace pacman::logic {

/**
 * @brief Enumeration describing the logical content of a single tile.
 */
enum class TileType : std::uint8_t {
    Empty,       ///< Walkable space with no entity
    Wall,        ///< Solid wall tile
    Coin,        ///< Collectable coin
    Fruit,       ///< Collectable fruit
    PacManSpawn, ///< Spawn position for Pac-Man
    GhostSpawn,  ///< Spawn position for ghosts
    GhostGate    ///< Gate restricting ghost movement
};

/**
 * @brief Logical tile map describing the level layout.
 *
 * The tile map is defined by a fixed ASCII layout that is converted
 * into a 2D grid stored internally as a 1D array.
 * Tile coordinates are mapped to world-space rectangles in the range [-1, 1].
 */
class TileMap {
public:
    static constexpr int Width = 20;  ///< Logical width of the grid in tiles
    static constexpr int Height = 11; ///< Logical height of the grid in tiles

    /**
     * @brief Constructs the tile map from the built-in ASCII layout.
     */
    TileMap();

    /**
     * @brief Checks whether the given tile coordinates are within map bounds.
     * @param x Tile X coordinate.
     * @param y Tile Y coordinate.
     * @return True if the coordinates are inside the map.
     */
    bool inBounds(int x, int y) const noexcept { return x >= 0 && x < Width && y >= 0 && y < Height; }

    /**
     * @brief Returns the tile type at the given coordinates.
     * @param x Tile X coordinate.
     * @param y Tile Y coordinate.
     * @return Tile type at (x, y), or Empty if out of bounds.
     */
    TileType at(int x, int y) const noexcept {
        if (!inBounds(x, y)) {
            return TileType::Empty;
        }
        return tiles_[y * Width + x];
    }

    /**
     * @brief Converts tile coordinates to a world-space axis-aligned rectangle.
     *
     * The map is centered in world space such that it fits inside
     * the square [-1, 1] × [-1, 1] while preserving square tiles.
     *
     * @param x Tile X coordinate.
     * @param y Tile Y coordinate.
     * @return World-space rectangle corresponding to the tile.
     */
    Rect tileRect(int x, int y) const noexcept {
        Rect r{};

        const float tileSize = 2.0f / static_cast<float>(std::max(Width, Height));

        const float worldW = tileSize * static_cast<float>(Width);
        const float worldH = tileSize * static_cast<float>(Height);

        const float startX = -worldW * 0.5f;
        const float startY = worldH * 0.5f;

        r.x = startX + static_cast<float>(x) * tileSize;
        r.y = startY - static_cast<float>(y + 1) * tileSize;
        r.w = tileSize;
        r.h = tileSize;

        return r;
    }

private:
    std::vector<TileType> tiles_; ///< Flattened 2D grid stored row-major
};

/**
 * @brief Constructs the tile map using a fixed ASCII layout.
 *
 * Characters in the layout map to tile types:
 * - '#' → Wall
 * - '.' → Coin
 * - 'F' → Fruit
 * - 'P' → Pac-Man spawn
 * - 'G' → Ghost spawn
 * - 'D' → Ghost gate
 */
inline TileMap::TileMap() : tiles_(Width * Height, TileType::Empty) {

    static const char* layout[Height] = {"####################", "#....#........#...F#", "#.##.#.######.#.##.#",
                                         "#.#..............#.#", "#.#.##.######.##.#.#", "#.P....# G  #......#",
                                         "#.#.##.##DD##.##.#.#", "#.#..............#.#", "#.##.#.######.#.##.#",
                                         "#F...#........#....#", "####################"};

    for (int y = 0; y < Height; ++y) {
        for (int x = 0; x < Width; ++x) {
            const char c = layout[y][x];
            TileType type = TileType::Empty;

            switch (c) {
            case '#':
                type = TileType::Wall;
                break;
            case '.':
                type = TileType::Coin;
                break;
            case 'F':
                type = TileType::Fruit;
                break;
            case 'P':
                type = TileType::PacManSpawn;
                break;
            case 'G':
                type = TileType::GhostSpawn;
                break;
            case 'D':
                type = TileType::GhostGate;
                break;
            default:
                type = TileType::Empty;
                break;
            }

            tiles_[y * Width + x] = type;
        }
    }
}

} // namespace pacman::logic