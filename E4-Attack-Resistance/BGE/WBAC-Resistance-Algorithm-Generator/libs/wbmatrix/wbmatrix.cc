#include "wbmatrix.h"
#include "../random/random.h"
#include <stdio.h>
#include <algorithm>

namespace wbmatrix {
using Random = myrandom::Random;
#define v8 std::vector<uint8_t>
BitMatrix::BitMatrix() { }
BitMatrix::BitMatrix(const uint16_t &dim_row,
                     const uint16_t &dim_col,
                     const std::string &type) {
  this->dim_col_ = dim_col;
  this->dim_row_ = dim_row;
  if (type == "random") {
    uint8_t l_bound = 0, u_bound = -1;
    for (int r = 0; r < dim_row; ++r) {
      v8 temp;
      for (int c = 0; c < dim_col; ++c) {
        temp.push_back(Random::RandomGen(l_bound, u_bound) & 1);
      }
      this->mat_.push_back(temp);
    }
  } else if (type == "identity") {
    for (int r = 0; r < dim_row; ++r) {
      v8 temp;
      for (int c = 0; c < dim_col; ++c) {
        if (r == c) {
          temp.push_back(1);
        } else {
          temp.push_back(0);
        }
      }
      this->mat_.push_back(temp);
    }
  } else {
    for (int r = 0; r < dim_row; ++r) {
      v8 temp;
      for (int c = 0; c < dim_col; ++c) {
        temp.push_back(0);
      }
      this->mat_.push_back(temp);
    }
  }
}
void BitMatrix::Print() {
  for (int r = 0; r < this->dim_row_; ++r) {
    for (int c = 0; c < this->dim_col_; ++c) {
      printf("%d", this->mat_[r][c]);
    }
    printf("\n");
  }
}
const std::vector<v8> BitMatrix::GetMat() const {
  return this->mat_;
}
const uint16_t BitMatrix::GetDimRow() const {
  return this->dim_row_;
}
const uint16_t BitMatrix::GetDimCol() const {
  return this->dim_col_;
}
void BitMatrix::MultMat(const BitMatrix &smat, BitMatrix *dmat) {
  uint16_t num_row = this->dim_row_, num_col = smat.GetDimCol(), num_mid = this->dim_col_;
  BitMatrix *temp_mat = new BitMatrix(num_row, num_col, "zero");
  uint8_t temp;
  for (int r = 0; r < num_row; ++r) {
    for (int c = 0; c < num_col; ++c) {
      temp = 0;
      for (int m = 0; m < num_mid; ++m) {
        temp ^= this->mat_[r][m] & smat.GetBit(m, c);
      }
      temp_mat->SetBit(r, c, temp);
    }
  }
  dmat->CopyMat(*temp_mat);
}
const uint8_t BitMatrix::GetBit(const uint16_t &row, const uint16_t &col) const {
  return this->mat_[row][col];
}
void BitMatrix::SetBit(const uint16_t &row, const uint16_t &col, uint8_t bit) {
  this->mat_[row][col] = bit;
}
void BitMatrix::CopyMat(const BitMatrix &mat) {
  this->mat_.clear();
  this->dim_col_ = mat.GetDimCol();
  this->dim_row_ = mat.GetDimRow();
  this->mat_ = mat.GetMat();
}
void BitMatrix::SwapRow(const uint16_t &r1, const uint16_t &r2) {
  v8 temp;
  temp = this->mat_[r1];
  this->mat_[r1] = this->mat_[r2];
  this->mat_[r2] = temp;
}
void BitMatrix::RowXor(const uint16_t &r1, const uint16_t &r2) {
  for (int c = 0; c < this->dim_col_; ++c) {
    this->mat_[r1][c] ^= this->mat_[r2][c];
  }
}
void BitMatrix::GetInv(BitMatrix *mat_inv) {
  uint16_t dim_rc = this->GetDimCol();
  BitMatrix iden_mat(dim_rc, dim_rc, "identity"); 
  mat_inv->CopyMat(iden_mat);
  BitMatrix temp;
  temp.CopyMat(*this);
  int r1, r2;
  bool is_invertable = true;
  for (r1 = 0; r1 < dim_rc; ++r1) {
    for (r2 = r1; r2 < dim_rc; ++r2) {
      if (temp.GetBit(r2, r1) == 1) {
        temp.SwapRow(r1, r2);
        mat_inv->SwapRow(r1, r2);
        break;
      }
    }
    if (r2 == dim_rc) {
      is_invertable = false;
      continue;
    }
    for (r2 = r1 + 1; r2 < dim_rc; ++r2) {
      if (temp.GetBit(r2, r1) == 1) {
        temp.RowXor(r2, r1);
        mat_inv->RowXor(r2, r1);
      }
    }
  }
  for (r1 = dim_rc - 1; r1 >= 0; --r1) {
    for (r2 = r1 - 1; r2 >= 0; --r2) {
      if (temp.GetBit(r2, r1) == 1) {
        temp.RowXor(r2, r1);
        mat_inv->RowXor(r2, r1);
      }
    }
  }
  if (!is_invertable) {
    mat_inv->GetInv(this);
  }
}
void BitMatrix::GetMatPair(BitMatrix *a, BitMatrix *b, size_t dim) {
  BitMatrix m1(dim, dim, "random"), m2;
  m1.GetInv(&m2);
  a->CopyMat(m1);
  b->CopyMat(m2);
}
void BitMatrix::SetMat(size_t dim_row, size_t dim_col, std::string type) {
  BitMatrix tmp(dim_row, dim_col, type);
  this->CopyMat(tmp);
}
BitVector::BitVector(const uint16_t &len, const uint64_t &val) {
  this->vec_.resize(len);
  this->len_ = len;
  uint64_t temp_val = val;
  for (int i = len - 1; i >= 0; --i) {
    this->vec_[i] = temp_val & 1;
    temp_val >>= 1;
  }
}
uint64_t BitVector::VecToVal(const uint16_t &l, const uint16_t &r) {
  uint64_t ret = 0;
  for (int i = l; i < r; ++i) {
    ret <<= 1;
    ret |= this->vec_[i];
  }
  return ret;
}
void BitMatrix::Concatenate(const std::vector<BitMatrix> &mats, BitMatrix *dmat) {
  int len = mats.size();
  int mdim = mats[0].GetDimCol();
  int dim = len * mdim;
  dmat->SetMat(dim, dim, "zero");
  for (int i = 0; i < mats.size(); ++i) {
    for (int r = i*mdim; r < (i + 1)*mdim; ++r) {
      for (int c = i*mdim; c < (i + 1)*mdim; ++c) {
        dmat->SetBit(r, c, mats[i].GetBit(r - i*mdim, c - i*mdim));
      }
    }
  }
}
uint8_t BitVector::operator[] (const uint16_t &index) const{
  return this->vec_[index];
}
void BitVector::SetBit(const uint16_t &index, const uint8_t &bit) {
  this->vec_[index] = bit;
}
const uint16_t BitVector::Length() const {
  return this->len_;
}
const std::vector<uint8_t> BitVector::GetVec() const {
  return this->vec_;
}
void BitVector::CopyVec(const BitVector &vec) {
  this->len_ = vec.Length();
  this->vec_ = vec.GetVec();
}
void BitVector::Concatenate(const BitVector &vec) {
  this->len_ += vec.Length();
  v8 temp = vec.GetVec();
  this->vec_.insert(this->vec_.end(), temp.begin(), temp.end());
}
void BitVector::MultMat(const BitMatrix &smat, BitVector *dvec) {
  uint16_t dim_row = smat.GetDimRow(), dim_col = smat.GetDimCol();
  std::vector<v8> mat = smat.GetMat();
  uint8_t temp_bit;
  BitVector *temp_vec = new BitVector(this->len_, 0);
  for (int r = 0; r < dim_row; ++r) {
    temp_bit = 0;
    for (int c = 0; c < dim_col; ++c) {
      temp_bit ^= mat[r][c] & this->vec_[c];
    }
    temp_vec->SetBit(r, temp_bit);
  }
  dvec->CopyVec(*temp_vec);
}
void BitVector::MultPartialMat(const uint16_t &ulr, const uint16_t &ulc,
                               const uint16_t &drr, const uint16_t &drc,
                               const BitMatrix &smat, BitVector *dvec) {
  std::vector<v8> mat = smat.GetMat();
  uint8_t temp_bit;
  BitVector *temp_vec = new BitVector(drr - ulr, 0);
  for (int r = ulr; r < drr; ++r) {
    temp_bit = 0;
    for (int c = ulc; c < drc; ++c) {
      temp_bit ^= mat[r][c] & this->vec_[c - ulc];
    }
    temp_vec->SetBit(r - ulr, temp_bit);
  }
  dvec->CopyVec(*temp_vec);
}
void BitVector::PartialMultMat(const uint16_t &l, const uint16_t &r,
                               const BitMatrix &smat, BitVector *dvec) {
  uint16_t dim_row = smat.GetDimRow();
  std::vector<v8> mat = smat.GetMat();
  uint8_t temp_bit;
  BitVector temp_vec(r - l, 0);
  for (int row = 0; row < dim_row; ++row) {
    temp_bit = 0;
    for (int c = l; c < r; ++c) {
      temp_bit ^= mat[row][c - l] & this->vec_[c];
    }
    temp_vec.SetBit(row, temp_bit);
  }
  dvec->CopyVec(temp_vec);
}
void BitVector::Print() {
  for (int i = 0; i < this->len_; ++i) {
    printf("%d", this->vec_[i]);
  }
  printf("\n");
}
BitVector::BitVector() { }
void BitMatrix::Expand() {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 4; ++j) {
      this->mat_[i].push_back(0);
    }
  }
  uint8_t lb = 0, ub = -1;
  for (int i = 0; i < 4; ++i) {
    v8 tmp(12);
    for (int j = 0; j < 12; ++j) {
      tmp[j] = Random::RandomGen(lb, ub) & 1;
    }
    this->mat_.push_back(tmp);
  }
  this->dim_col_ = 12;
  this->dim_row_ = 12;
}
} // namespace wbmatrix
