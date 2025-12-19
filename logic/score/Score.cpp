#include "Score.h"

#include "../utils/Stopwatch.h"

#include <algorithm>
#include <fstream>
#include <utility>

namespace pacman::logic {

/**
 * @brief Resets score state, including combo timing and decay timing.
 */
void Score::reset() noexcept {
    currentScore_ = 0;

    lastCollectTime_ = 0.0;
    hasLastCollectTime_ = false;

    lastTickTime_ = 0.0;
    hasLastTickTime_ = false;

    decayAccumulator_ = 0.0;
}

/**
 * @brief Processes logic events to update score (collection, decay ticks, and death penalty).
 *
 * - Collected: applies a time-based chain multiplier depending on time between pickups.
 * - Tick: decays score over time using the global stopwatch.
 * - Died: applies a score change based on the payload value (as implemented).
 *
 * @param event Incoming logic event.
 */
void Score::onEvent(const pacman::logic::Event& event) {
    switch (event.type) {
    case EventType::Collected: {
        const auto* payload = std::get_if<CollectedPayload>(&event.payload);
        if (!payload) {
            break;
        }

        auto& sw = Stopwatch::getInstance();
        const double now = sw.elapsed();

        double multiplier = 1.0;
        if (hasLastCollectTime_) {
            const double dt = now - lastCollectTime_;
            if (dt < 0.5) {
                multiplier = 4.0;
            } else if (dt < 1.0) {
                multiplier = 3.0;
            } else if (dt < 2.0) {
                multiplier = 2.0;
            }
        }

        const int base = payload->value;
        const int bonusScore = static_cast<int>(base * multiplier);

        add(bonusScore);

        lastCollectTime_ = now;
        hasLastCollectTime_ = true;
        break;
    }

    case EventType::Tick: {
        auto& sw = Stopwatch::getInstance();
        const double now = sw.elapsed();

        if (!hasLastTickTime_) {
            lastTickTime_ = now;
            hasLastTickTime_ = true;
            break;
        }

        const double dt = now - lastTickTime_;
        if (dt <= 0.0) {
            break;
        }

        lastTickTime_ = now;

        decayAccumulator_ += decayRatePerSecond_ * dt;

        const int wholePoints = static_cast<int>(decayAccumulator_);
        if (wholePoints > 0) {
            decayAccumulator_ -= wholePoints;

            currentScore_ -= wholePoints;
            if (currentScore_ < 0) {
                currentScore_ = 0;
            }
        }
        break;
    }

    case EventType::Died: {
        const auto* payload = std::get_if<CollectedPayload>(&event.payload);
        if (!payload) {
            break;
        }

        const int deathPenalty = payload->value;
        add(deathPenalty);

        if (currentScore_ < 0) {
            currentScore_ = 0;
        }
        break;
    }

    case EventType::Moved:
    case EventType::StateChanged:
        break;
    }
}

/**
 * @brief Loads highscores from a file and normalizes the result to exactly five entries.
 * @param path Path to the highscore file.
 * @return A vector of exactly five scores sorted in descending order.
 */
std::vector<int> Score::loadHighscores(const std::string& path) {
    std::vector<int> scores;

    std::ifstream in(path);
    if (in) {
        int value = 0;
        while (in >> value) {
            scores.push_back(value);
        }
    }

    if (scores.empty()) {
        scores.assign(5, 0);
    }

    std::sort(scores.begin(), scores.end(), std::greater<int>());

    if (scores.size() < 5) {
        scores.resize(5, 0);
    } else if (scores.size() > 5) {
        scores.resize(5);
    }

    return scores;
}

/**
 * @brief Saves highscores to a file, writing exactly five lines.
 * @param path Path to the highscore file.
 * @param scores Scores to write.
 */
void Score::saveHighscores(const std::string& path, const std::vector<int>& scores) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        return;
    }

    std::vector<int> copy = scores;
    if (copy.size() < 5) {
        copy.resize(5, 0);
    }

    for (std::size_t i = 0; i < 5 && i < copy.size(); ++i) {
        out << copy[i] << '\n';
    }
}

/**
 * @brief Inserts a new score into an existing highscore list and returns the top five.
 * @param current The current highscores.
 * @param newScore The new score to insert.
 * @return Updated highscores (max 5) sorted in descending order.
 */
std::vector<int> Score::updateHighscores(const std::vector<int>& current, int newScore) {
    std::vector<int> result = current;
    result.push_back(newScore);

    std::sort(result.begin(), result.end(), std::greater<int>());

    if (result.size() > 5) {
        result.resize(5);
    }

    return result;
}

} // namespace pacman::logic