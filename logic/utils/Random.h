#pragma once
#include <cstdint>
#include <random>
#include <vector>

namespace pacman::logic {

    class Random {
    public:
        static Random& getInstance(); // Returns a global singleton instance

        void seed(std::uint32_t s); // Seeds the generator with a fixed integer
        void seedFromDevice(); // Seeds the generator using hardware randomness

        int uniformInt(int a, int b); // Returns random integer uniformly in [a, b]
        double uniformReal(double a, double b); // Returns random floating point uniformly in [a, b]
        bool bernoulli(double p); // Returns true with probability p
        double normal(double mean, double stddev); // Returns value from normal distribution N

        std::size_t choiceIndex(std::size_t count); // Returns random index in [0, count-1]
        void shuffleIndices(std::vector<std::size_t>& indices); // Shuffles a vector of indices in place

    private:
        Random(); // Private constructor to enforce singleton
        ~Random() = default; // Default constructor
        Random(const Random&) = delete; // Disable copy constructor
        Random operator=(const Random&) = delete; // Disable assignment

        std::mt19937 engine_; // Mersenne Twister PRNG engine providing deterministic sequences
    };
}