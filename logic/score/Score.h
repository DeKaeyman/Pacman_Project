#pragma once

#include "../observer/Event.h"
#include "../observer/Observer.h"

#include <string>
#include <vector>

namespace pacman::logic {

/**
 * @brief Score tracker that observes domain events and maintains a running score.
 *
 * The score supports:
 * - collection chain multipliers based on time between pickups,
 * - score decay over time driven by Tick events,
 * - score adjustments on death events (as implemented).
 */
class Score : public Observer {
public:
    /**
     * @brief Constructs a score tracker.
     */
    Score() = default;

    /**
     * @brief Resets score state, including combo timing and decay timing.
     */
    void reset() noexcept;

    /**
     * @brief Adds the given amount to the current score.
     * @param amount Points to add (may be negative depending on caller/event design).
     */
    void add(int amount) { currentScore_ += amount; }

    /**
     * @brief Returns the current score value.
     * @return Current score.
     */
    int value() const noexcept { return currentScore_; }

    /**
     * @brief Receives and processes logic events that influence the score.
     * @param event Incoming logic event.
     */
    void onEvent(const Event& event) override;

    /**
     * @brief Loads highscores from a file and normalizes to exactly five entries.
     * @param path Path to the highscore file.
     * @return Vector of five scores sorted in descending order.
     */
    static std::vector<int> loadHighscores(const std::string& path);

    /**
     * @brief Saves highscores to a file, writing exactly five lines.
     * @param path Path to the highscore file.
     * @param scores Scores to write.
     */
    static void saveHighscores(const std::string& path, const std::vector<int>& scores);

    /**
     * @brief Inserts a new score into an existing highscore list and returns the top five.
     * @param current Current highscore list.
     * @param newScore Score to insert.
     * @return Updated highscore list (max 5), sorted descending.
     */
    static std::vector<int> updateHighscores(const std::vector<int>& current, int newScore);

private:
    int currentScore_{0};

    double lastCollectTime_{0.0};
    bool hasLastCollectTime_{false};

    double lastTickTime_{0.0};
    bool hasLastTickTime_{false};
    double decayAccumulator_{0.0};
    double decayRatePerSecond_{1.0};
};

} // namespace pacman::logic