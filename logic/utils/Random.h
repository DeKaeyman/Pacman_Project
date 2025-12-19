#pragma once

#include <cstdint>
#include <random>
#include <vector>

namespace pacman::logic {

    /**
     * @brief Singleton random number generator wrapper around std::mt19937.
     *
     * Provides helper methods for common distributions and deterministic seeding.
     */
    class Random {
    public:
        /**
         * @brief Returns the singleton Random instance.
         * @return Reference to the global Random instance.
         */
        static Random& getInstance();

        /**
         * @brief Seeds the generator with a fixed integer for reproducible sequences.
         * @param seedValue Seed value to initialize the PRNG.
         */
        void seed(std::uint32_t seedValue);

        /**
         * @brief Seeds the generator using entropy from std::random_device.
         */
        void seedFromDevice();

        /**
         * @brief Returns a random integer uniformly distributed in [a, b].
         * @param a Inclusive lower bound.
         * @param b Inclusive upper bound.
         * @return Random integer in [a, b].
         */
        int uniformInt(int a, int b);

        /**
         * @brief Returns a random double uniformly distributed in [a, b).
         * @param a Inclusive lower bound.
         * @param b Exclusive upper bound.
         * @return Random double in [a, b).
         */
        double uniformReal(double a, double b);

        /**
         * @brief Returns true with probability p.
         * @param p Probability in the range [0, 1].
         * @return Boolean sample drawn from a Bernoulli distribution.
         */
        bool bernoulli(double p);

        /**
         * @brief Returns a random value sampled from a normal distribution.
         * @param mean Mean of the normal distribution.
         * @param stddev Standard deviation of the normal distribution.
         * @return Sample drawn from N(mean, stddev).
         */
        double normal(double mean, double stddev);

        /**
         * @brief Chooses a random index in the range [0, count - 1].
         * @param count Size of the range.
         * @return Random index in [0, count - 1].
         * @throws std::out_of_range If count is zero.
         */
        std::size_t choiceIndex(std::size_t count);

        /**
         * @brief Shuffles a vector of indices in place.
         * @param indices Vector to shuffle.
         */
        void shuffleIndices(std::vector<std::size_t>& indices);

    private:
        /**
         * @brief Constructs the RNG with a deterministic default seed.
         */
        Random();

        /**
         * @brief Default destructor.
         */
        ~Random() = default;

        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;

    private:
        std::mt19937 engine_;
    };

} // namespace pacman::logic