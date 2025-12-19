#include "Random.h"

#include <algorithm>
#include <stdexcept>

namespace pacman::logic {

/**
 * @brief Constructs the RNG with a fixed default seed for deterministic behavior.
 */
Random::Random() : engine_(std::mt19937{5489u}) {}

/**
 * @brief Returns the singleton Random instance.
 * @return Reference to the global Random instance.
 */
Random& Random::getInstance() {
    static Random rng;
    return rng;
}

/**
 * @brief Seeds the generator with a fixed integer for reproducible sequences.
 * @param seedValue Seed value to initialize the PRNG.
 */
void Random::seed(std::uint32_t seedValue) { engine_.seed(seedValue); }

/**
 * @brief Seeds the generator using entropy from std::random_device.
 */
void Random::seedFromDevice() {
    std::random_device rd;
    std::seed_seq seq{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    engine_.seed(seq);
}

/**
 * @brief Returns a random integer uniformly distributed in [a, b].
 * @param a Inclusive lower bound.
 * @param b Inclusive upper bound.
 * @return Random integer in [a, b].
 */
int Random::uniformInt(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(engine_);
}

/**
 * @brief Returns a random double uniformly distributed in [a, b).
 * @param a Inclusive lower bound.
 * @param b Exclusive upper bound.
 * @return Random double in [a, b).
 */
double Random::uniformReal(double a, double b) {
    std::uniform_real_distribution<double> dist(a, b);
    return dist(engine_);
}

/**
 * @brief Returns true with probability p.
 * @param p Probability in the range [0, 1].
 * @return Boolean sample drawn from a Bernoulli distribution.
 */
bool Random::bernoulli(double p) {
    std::bernoulli_distribution dist(p);
    return dist(engine_);
}

/**
 * @brief Returns a random value sampled from a normal distribution.
 * @param mean Mean of the normal distribution.
 * @param stddev Standard deviation of the normal distribution.
 * @return Sample drawn from N(mean, stddev).
 */
double Random::normal(double mean, double stddev) {
    std::normal_distribution<double> dist(mean, stddev);
    return dist(engine_);
}

/**
 * @brief Chooses a random index in the range [0, count - 1].
 * @param count Size of the range.
 * @return Random index in [0, count - 1].
 * @throws std::out_of_range If count is zero.
 */
std::size_t Random::choiceIndex(std::size_t count) {
    if (count == 0) {
        throw std::out_of_range("choiceIndex on empty range");
    }

    std::uniform_int_distribution<std::size_t> dist(0, count - 1);
    return dist(engine_);
}

/**
 * @brief Shuffles a vector of indices in place.
 * @param indices Vector to shuffle.
 */
void Random::shuffleIndices(std::vector<std::size_t>& indices) {
    std::shuffle(indices.begin(), indices.end(), engine_);
}

} // namespace pacman::logic