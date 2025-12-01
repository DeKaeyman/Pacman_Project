// logic/score/Score.cpp
#include "Score.h"
#include "../utils/Stopwatch.h"

#include <fstream>
#include <algorithm>

namespace pacman::logic {

    void Score::onEvent(const pacman::logic::Event &e) {
        switch (e.type) {
            case EventType::Collected: { // A collectable (coin/fruit) has been picked up
                if (auto payload = std::get_if<CollectedPayload>(&e.payload)) {

                    auto& sw = Stopwatch::getInstance(); // Global game timer
                    double now = sw.elapsed(); // Moment of this pickup

                    double multiplier = 1.0; // Default value

                    if (hasLastCollectTime_) { // If we previously collected something, give a bonus
                        double dt = now - lastCollectTime_; // time between pickups
                        if (dt < 0.5) multiplier = 4.0; // Extreme speed chain
                        else if (dt < 1.0) multiplier = 3.0; // Very fast chain
                        else if (dt < 2.0) multiplier = 2.0; // Normal chain
                    }

                    int base = payload->value; // Score value from model
                    int bonusScore = static_cast<int>(base * multiplier); // Apply chain multiplier

                    add(bonusScore); // Update running score

                    lastCollectTime_ = now; // Remember when this pickup occurred
                    hasLastCollectTime_ = true; // Mark that chain logic is active
                }
                break;
            }

            case EventType::Tick:
                break;

            case EventType::Died:
                break;

            case EventType::Moved:
            case EventType::StateChanged:
                break;
        }
    }

    std::vector<int> Score::loadHighscores(const std::string &path) {
        std::vector<int> scores; // Local container for loaded scores

        std::ifstream in(path); // Attempt to open highscore file
        if (in) { // If the file exists and is readable
            int value = 0;
            while (in >> value) { // Read integers line by line
                scores.push_back(value); // Store each score
            }
        }

        if (scores.empty()) { // If no scores were loaded
            scores.assign(5, 0); // Fill with five default zero scores
        }

        std::sort(scores.begin(), scores.end(), std::greater<int>()); // Sort highest to lowest

        if (scores.size() < 5) { // Ensure exactly 5 entries
            scores.resize(5, 0); // Fill with zeros if too few
        } else if (scores.size() > 5) {
            scores.resize(5); // Trim extra entries
        }

        return scores; // Return the validated, sorted list
    }

    void Score::saveHighscores(const std::string &path, const std::vector<int> &scores) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) {
            return; // Fail silently if write operation is not possible
        }

        std::vector<int> copy = scores; // Copy scores to local list for normalization
        if (copy.size() < 5) {
            copy.resize(5, 0); // Ensure file always contains exactly 5 lines
        }

        for (auto i = 0; i < 5 && i < copy.size(); i++) {
            out << copy[i] << '\n'; // Write each score on a new line
        }
    }

    std::vector<int> Score::updateHighscores(const std::vector<int> &current, int newScore) {
        std::vector<int> result = current; // Start from existing scores
        result.push_back(newScore); // Add the newest score to the list

        std::sort(result.begin(), result.end(), std::greater<int>()); // Sort descending

        if (result.size() > 5) {
            result.resize(5); // Keep only top 5 results
        }

        return result; // Return updated high score list
    }
}