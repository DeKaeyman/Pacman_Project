// logic/score/Score.hpp
#pragma once

#include <vector>
#include <string>

namespace pacman::logic {

    class Score {
    public:
        Score() = default;

        void reset() noexcept { currentScore_ = 0; }
        void add(int amount) { currentScore_ += amount; }
        int value() const noexcept { return currentScore_; }

        static std::vector<int> loadHighscores(const std::string& path);
        static void saveHighscores(const std::string& path, const std::vector<int>& scores);
        static std::vector<int> updateHighscores(const std::vector<int>& current, int newScore);

    private:
        int currentScore_{0};
    };
}