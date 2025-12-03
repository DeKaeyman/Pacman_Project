// logic/world/TileMap.hpp
#pragma once

#include "../entities/Entity.h"
#include <vector>

namespace pacman::logic {

enum class TileType : std::uint8_t { // Defines all possible tile contents in
                                     // the logical grid
    Empty,                           // Walkable space, nothing on it
    Wall,                            // Solid wall tile
    Coin,                            // Collectable coin tile
    Fruit,                           // Collectable fruit tile
    PacManSpawn,                     // Starting position for Pacman
    GhostSpawn                       // Starting position for ghost
};

class TileMap {
public:
    static constexpr int Width = 20;  // Logical width of the tile grid
    static constexpr int Height = 11; // Logical height of the tile grid

    TileMap(); // Constructor build the grid from ASCII layout below

    bool inBounds(int x, int y) const noexcept { // Check if the tile coordinate lies within map
        return x >= 0 && x < Width && y >= 0 && y < Height;
    }

    TileType at(int x, int y) const noexcept { // Get the tile type at (x,y)
        if (!inBounds(x, y))
            return TileType::Empty;   // Out of bounds
        return tiles_[y * Width + x]; // Convert 2D coords to index in 1D vector
    }

    Rect tileRect(int x, int y) const noexcept { // Convert tile coords to world space rectangle
        Rect r{};
        // Instead of separate tileW / tileH
        const float tileSize = 2.0f / static_cast<float>(std::max(Width, Height));

        // Total size of the map in world units
        const float worldW = tileSize * static_cast<float>(Width);
        const float worldH = tileSize * static_cast<float>(Height);

        // Center the map in world space [-1,1] x [-1,1]
        const float startX = -worldW * 0.5f; // left edge of map
        const float startY = worldH * 0.5f;  // top edge of map

        r.x = startX + static_cast<float>(x) * tileSize;
        r.y = startY - static_cast<float>(y + 1) * tileSize;

        r.w = tileSize;
        r.h = tileSize;

        return r; // Return world space AABB for this tile
    }

private:
    std::vector<TileType> tiles_; // 1D array storing tile content for each cell
};

inline TileMap::TileMap() : tiles_(Width * Height,
                                   TileType::Empty) { // ASCII representation of the map layout

    static const char* layout[Height] = {"####################", "#...G#........#...F#", "#.##.#.######.#.##.#",
                                         "#.#..............#.#", "#.#.##.######.##.#.#", "#.P....#    #......#",
                                         "#.#.##.##  ##.##.#.#", "#.#..............#.#", "#.##.#G######.#.##.#",
                                         "#F...#........#....#", "####################"};

    for (int y = 0; y < Height; y++) {    // Iterate trough each row
        for (int x = 0; x < Width; x++) { // Iterate trough each column
            char c = layout[y][x];        // Character describing current tile
            TileType t = TileType::Empty; // Default type

            switch (c) {
            case '#': // Wall
                t = TileType::Wall;
                break;
            case '.': // Coin
                t = TileType::Coin;
                break;
            case 'F': // Fruit
                t = TileType::Fruit;
                break;
            case 'P': // Pacman spawn position
                t = TileType::PacManSpawn;
                break;
            case 'G': // Ghost spawn position
                t = TileType::GhostSpawn;
                break;
            default: // Unknown char treat as empty
                t = TileType::Empty;
                break;
            }

            tiles_[y * Width + x] = t; // Store tile in 1D array at its flattened index
        }
    }
}
} // namespace pacman::logic