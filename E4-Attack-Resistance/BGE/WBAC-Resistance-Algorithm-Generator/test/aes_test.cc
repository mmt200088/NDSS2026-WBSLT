#include "../src/aes/aes.h"
#include <iostream>
using namespace std;
using namespace aes;
int main() {
  vector<uint16_t> v(16);
  vector<uint8_t> k(16);
  for (int i = 0; i < 16; ++i) {
    v[i] = i;
    k[i] = i;
  }
  k[0] = 0x2b, k[1] = 0x7e, k[2] = 0x15, k[3] = 0x16;
  k[4] = 0x28, k[5] = 0xae, k[6] = 0xd2, k[7] = 0xa6;
  k[8] = 0xab, k[9] = 0xf7, k[10] = 0x15, k[11] = 0x88;
  k[12] = 0x09, k[13] = 0xcf, k[14] = 0x4f, k[15] = 0x3c;
  // v[0] = 0x32, v[1] = 0x43, v[2] = 0xf6, v[3] = 0xa8;
  // v[4] = 0x88, v[5] = 0x5a, v[6] = 0x30, v[7] = 0x8d;
  // v[8] = 0x31, v[9] = 0x31, v[10] = 0x98, v[11] = 0xa2;
  // v[12] = 0xe0, v[13] = 0x37, v[14] = 0x07, v[15] = 0x34;
  State *state = new State(v);
  // printf("before address %d\n", state);
  // state->Print();
  Aes aes(k);
  aes.Encrypt(state);
  // printf("after address %d\n", state);
  state->Print();
  // aes.Decrypt(state);
  // state->Print();
  return 0;
}