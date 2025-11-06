#include "random.h"

namespace myrandom {
uint64_t Random::RandomGen(const uint64_t &l_bound, const uint64_t &u_bound) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<uint64_t> dist(l_bound, u_bound);
  return dist(gen);
}
void Random::GenPerm(const size_t &len, std::vector<uint16_t> *perm, std::vector<uint16_t> *perm_inv) {
  perm->resize(len);
  perm_inv->resize(len);
  for (int i = 0; i < len; ++i) {
    (*perm)[i] = i;
  }
  uint64_t j;
  uint16_t temp;
  for (int i = 0; i < len - 1; ++i) {
    j = RandomGen(0, 1000000007) % (len - i);
    temp = (*perm)[i];
    (*perm)[i] = (*perm)[i + j];
    (*perm)[i + j] = temp;
  }
  for (int i = 0; i < len; ++i) {
    (*perm_inv)[(*perm)[i]] = i;
  }
}
} // namespace random