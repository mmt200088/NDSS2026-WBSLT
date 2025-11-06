#include "../libs/random/random.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace myrandom;

int main() {
  for (int i = 0; i < 30; ++i) {
    cout << Random::RandomGen(0, 10) << " ";
  }
  cout << endl;
  vector<uint16_t> a, b;
  Random::GenPerm(16, &a, &b);
  for (int i = 0; i < 16; ++i) {
    cout << a[i] << " ";
  }
  cout << endl;
  for (int i = 0; i < 16; ++i) {
    cout << b[i] << " ";
  }
  cout << endl;
  return 0;
}