// logic/score/Score.hpp
#pragma once

#include <string>
#include <vector>

#include "../observer/Event.h"
#include "../observer/Observer.h"

namespace pacman::logic {

class Score : public Observer {
public:
    Score() = default; // Simple score tracker

    void reset() noexcept;                               // Reset player score to zero
    void add(int amount) { currentScore_ += amount; }    // Add points
    int value() const noexcept { return currentScore_; } // Read current score

    void onEvent(const Event& e) override; // Allow Score to observe game events (coins, fruits, etc.)

    static std::vector<int> loadHighscores(const std::string& path); // Read top 5 scores from file
    static void saveHighscores(const std::string& path,
                               const std::vector<int>& scores); // Write scores to file
    static std::vector<int> updateHighscores(const std::vector<int>& current,
                                             int newScore); // Insert + sort new highscore

private:
    int currentScore_{0};            // Player's current running score
    double lastCollectTime_{0.0};    // Timestamp of last collected coin/fruit
    bool hasLastCollectTime_{false}; // True once first collect happens

    // Basis-score-decrement over time
    double lastTickTime_{0.0};       // Last time we pressed a tick
    bool hasLastTickTime_{false};    // Only be active after first tick
    double decayAccumulator_{0.0};   // Fractions that do not lead tot full points
    double decayRatePerSecond_{1.0}; // Amount of points that disappear
};
} // namespace pacman::logic