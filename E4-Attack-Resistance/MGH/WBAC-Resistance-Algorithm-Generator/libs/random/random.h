#ifndef WBRAG_LIBS_RANDOM_RANDOM_H
#define WBRAG_LIBS_RANDOM_RANDOM_H

#include <random>
#include <cmath>
#include <stdint.h>

namespace myrandom {
// Generate random integer number between l_bound to u_bound (inclusive)
// Example:
//    Random::RandomGen(0, 100);
class Random {
 public:
  static uint64_t RandomGen(const uint64_t &l_bound, const uint64_t &u_bound);
  static void GenPerm(const size_t &len, std::vector<uint16_t> *perm, std::vector<uint16_t> *perm_inv);
};
} // namespace random
#endif