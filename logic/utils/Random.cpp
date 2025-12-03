#include "Random.h"
#include <algorithm>
#include <stdexcept>

namespace pacman::logic {

Random::Random()
    : engine_(std::mt19937{5489u}) {} // Initialize RNG with fixed seed

Random &Random::getInstance() {
  static Random rng;
  return rng;
}

void Random::seed(std::uint32_t s) {
  engine_.seed(s); // Seed RNG with give constant for reproducible sequences
}

void Random::seedFromDevice() {
  std::random_device rd; // Hardware source
  std::seed_seq seq{rd(), rd(), rd(), rd(),
                    rd(), rd(), rd(), rd()}; // Build complex seed sequence
  engine_.seed(seq);                         // Seed RNG with non det values
}

int Random::uniformInt(int a, int b) {
  std::uniform_int_distribution<int> dist(a, b); // integer uniform distribution
  return dist(engine_);                          // Generate number in [a, b]
}

double Random::uniformReal(double a, double b) {
  std::uniform_real_distribution<double> dist(
      a, b);            // Real valued uniform distribution
  return dist(engine_); // Generate double in [a, b)
}

bool Random::bernoulli(double p) {
  std::bernoulli_distribution dist(p); // Returns true with probability p
  return dist(engine_);                // Draw boolean outcome
}

double Random::normal(double mean, double stddev) {
  std::normal_distribution<double> dist(mean, stddev); // Normal distribution
  return dist(engine_);                                // Generate sample value
}

std::size_t Random::choiceIndex(std::size_t count) {
  if (count == 0)
    throw std::out_of_range(
        "choiceIndex on empty range"); // Prevents invalid range
  std::uniform_int_distribution<std::size_t> dist(
      0, count - 1);    // Uniform over valid
  return dist(engine_); // Return chosen index
}

void Random::shuffleIndices(std::vector<std::size_t> &indices) {
  std::shuffle(indices.begin(), indices.end(),
               engine_); // Randomly reorder elements using current engine
}
} // namespace pacman::logic