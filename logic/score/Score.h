// logic/score/Score.hpp
#pragma once

#include <vector>
#include <string>

namespace pacman::logic {

    class Score {
    public:
        Score() = default; // Simple score tracker

        void reset() noexcept { currentScore_ = 0; } // Reset player score to zero
        void add(int amount) { currentScore_ += amount; } // Add points
        int value() const noexcept { return currentScore_; } // Read current score

        static std::vector<int> loadHighscores(const std::string& path); // Read top 5 scores from file
        static void saveHighscores(const std::string& path, const std::vector<int>& scores); // Write scores to file
        static std::vector<int> updateHighscores(const std::vector<int>& current, int newScore); // Insert + sort new highscore

    private:
        int currentScore_{0}; // Player's current running score
    };
}