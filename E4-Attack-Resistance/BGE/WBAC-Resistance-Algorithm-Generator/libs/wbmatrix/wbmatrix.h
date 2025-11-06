#ifndef WBRAG_LIBS_WBMATRIX_WBMATRIX_H
#define WBRAG_LIBS_WBMATRIX_WBMATRIX_H

#include <stdint.h>
#include <vector>
#include <string>

namespace wbmatrix {
// 0/1 matrix and corresponding operations
// Row number from up to down, from left to right, starts at 0
// Example:
//      [0] [1] [2] [3] [4]
//  [0]  1   0   1   1   1
//  [1]  0   0   0   1   0
//  [2]  1   1   0   1   1
//  [3]  0   1   1   1   1
//  [4]  1   1   0   0   0
class BitMatrix {
 public:
  // Generates default matrix
  BitMatrix();
  // Generates matrix with dim_row rows and dim_col columns, types can be zero, identity and random
  BitMatrix(const uint16_t &dim_row,
            const uint16_t &dim_col,
            const std::string &type);
  // Left multiplies smat, stores result to dmat
  void MultMat(const BitMatrix &smat, BitMatrix *dmat);
  const std::vector<std::vector<uint8_t> > GetMat() const;
  const uint16_t GetDimRow() const;
  const uint16_t GetDimCol() const;
  // Prints current matrix in bit form
  void Print();
  // Gets certain bit at row row and column col
  const uint8_t GetBit(const uint16_t &row, const uint16_t &col) const;
  // Sets bit at row row and column col with value bit
  void SetBit(const uint16_t &row, const uint16_t &col, uint8_t bit);
  // Copies mat to current matrix
  void CopyMat(const BitMatrix &mat);
  // Generates the inversion of current matrix
  // If current matrix is not invertable, it will be modified to invertable
  void GetInv(BitMatrix *mat_inv);
  // Swaps row r1 and r2 of current matrix
  void SwapRow(const uint16_t &r1, const uint16_t &r2);
  // Bitwise Xors row r1 and r2 of current matrix
  void RowXor(const uint16_t &r1, const uint16_t &r2);
  // Sets current matrix as row of dim_row and column of dim_col with certain type
  void SetMat(size_t dim_row, size_t dim_col, std::string type);
  // Expands current matrix of dimention 8*8 to 12*12, 
  // right side of the original matrix filled with 0, 
  // below of the matrix filled with random bits
  void Expand();
  // Generates a pair of dim*dim invertable matrixs
  static void GetMatPair(BitMatrix *a, BitMatrix *b, size_t dim);
  // Concatenates matrixs in mats of diagonal form, stores to dmat
  static void Concatenate(const std::vector<BitMatrix> &mats, BitMatrix *dmat);
 private:
  // Stores row and column dimention
  uint16_t dim_row_, dim_col_;
  // Stores the matrix
  std::vector<std::vector<uint8_t> > mat_;
};
// 0/1 vector with corresponding operations
// the most significant bit stores at 0
// Example:
//    131 = 10000011
//    Vector form:
//    [0] [1] [2] [3] [4] [5] [6] [7]
//     1   0   0   0   0   0   1   1
class BitVector {
 public:
  // Generates a default vector
  BitVector();
  // Generate a vector of length len and val val
  BitVector(const uint16_t &len, const uint64_t &val);
  // Returns the value of current vector
  uint64_t VecToVal(const uint16_t &l, const uint16_t &r);
  uint8_t operator[] (const uint16_t &index) const;
  // Copies vec to current vector
  void CopyVec(const BitVector &vec);
  const uint16_t Length() const;
  const std::vector<uint8_t> GetVec() const;
  void SetBit(const uint16_t &index, const uint8_t &bit);
  // Concatenates current vector with vec at tail
  void Concatenate(const BitVector &vec);
  // Right multiplies matrix mat, stores result to dvec
  void MultMat(const BitMatrix &mat, BitVector *dvec);
  // Right multiplies part of the matrix smat, stores result to dvec, 
  // the upper left point of the matrix is at (ulr, ulc)
  // the lower right point of the matrix is at (drr, drc) (not inclusive)
  // Example:
  //    Vector: 1001
  //    Matrix: 101001
  //            010011
  //            111011
  //            111111
  //            001101
  //            101010
  //    MultiPartialMat(0, 0, 4, 4, Matrix, *dvec)
  //    1001 * 1010
  //           0100
  //           1110
  //           1111
  void MultPartialMat(const uint16_t &ulr, const uint16_t &ulc,
                      const uint16_t &drr, const uint16_t &drc,
                      const BitMatrix &smat, BitVector *dvec);
  // Part of current vector right multiplies the matrix smat, stores result to dvec,
  // the left most side is l (inclusive), right most side is r (not inclusive)
  // Example:
  //    Vector: 1001
  //    Matrix: 11
  //            01
  //    PartialMulMat(1, 3, Matrix, *dvec)
  //    00 * 11
  //         01
  void PartialMultMat(const uint16_t &l, const uint16_t &r,
                      const BitMatrix &smat, BitVector *dvec);
  void Print();
 private:
  // Stores the length of current vector
  uint16_t len_;
  // Stores the current vector
  std::vector<uint8_t> vec_;
};
} // namespace wbmatrix
#endif