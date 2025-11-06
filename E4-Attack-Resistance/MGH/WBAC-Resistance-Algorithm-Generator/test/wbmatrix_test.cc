#include "../libs/wbmatrix/wbmatrix.h"
#include <iostream>
using namespace std;
using namespace wbmatrix;

int main() {
  BitMatrix m(48, 48, "identity");
  m.Print();
  BitVector v(48, 1);
  v.Print();
  v.MultMat(m, &v);
  v.Print();
  return 0;
}