#include "cejo.h"
#include "../../libs/wbmatrix/wbmatrix.h"
#include <stdlib.h>

namespace cejo {
using AesBase = aes::AesBase;
using BitVector = wbmatrix::BitVector;
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define v8 std::vector<uint8_t>
#define v16 std::vector<uint16_t>
#define v32 std::vector<uint32_t>
#define v64 std::vector<uint64_t>
#define vr std::vector<Round>
#define vb std::vector<BitMatrix>
#define vv std::vector<BitVector>
#define v(a) std::vector<a>
#define U32_TO_U8(a,r) (((a) >> ((3-(r)) * 8)) & 0xff)
#define U8_TO_U32(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))
// Generates Adder Network after TypeI, II and III tables.
// For TypeI table, the adder network is 15 rows, 32 columns
// For TypeII and III tables, adder network is 3 rows, 32 columns
// input1 -> id*input1 \
//                      input1^input2 -> out -> oe*out
// input2 -> id*input2 /
void GenerateAdderNetwork(const int &dim_row, const vb &id, const vb &oe, v8 *adder) {
  adder->resize(dim_row*32*256);
  v(vb) aid(dim_row), aoe(dim_row);
  for (int i = 0; i < dim_row - 1; ++i) {
    aid[i].resize(32), aoe[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      BitMatrix::GetMatPair(&aid[i][j], &aoe[i][j], 4);
    }
  }
  for (int r = 0; r < dim_row; ++r) {
    for (int i = 0; i < 32; ++i) {
      for (int in1 = 0; in1 < 16; ++in1) {
        BitVector v1(4, in1);
        if (r == 0) {
          v1.MultMat(id[i], &v1);
        } else {
          v1.MultMat(aid[r - 1][i], &v1);
        }
        for (int in2 = 0; in2 < 16; ++in2) {
          BitVector v2(4, in2);
          v2.MultMat(id[(r+1)*32 + i], &v2);
          int val = v1.VecToVal(0, 4) ^ v2.VecToVal(0, 4);
          BitVector vout(4, val);
          if (r == dim_row - 1) {
            vout.MultMat(oe[i], &vout);
          } else {
            vout.MultMat(aoe[r][i], &vout);
          }
          (*adder)[r*32*256 + i*256 + in1*16 + in2] = vout.VecToVal(0, 4);
        }
      }
    }
  }
}
// input(8 bit) -> * dm (128 bit) -> * concatenated l (128 bit)-> * oe (32 * 4 bit) -> out (32 * 4 bit)
Round::Round(const BitMatrix &dm, const vb &encoding, const vb &l, const bool &is_idm, const bool &is_enc) {
  this->type_ = "idm";
  BitMatrix mat;
  BitMatrix::Concatenate(l, &mat);
  this->type_i_.resize(16 * 256 * 16);
  int state_index, state_val, out_index;
  BitVector out_vec;
  uint16_t val;
  vb id(16*32), oe(16*32);
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      BitMatrix::GetMatPair(&id[i*32 + j], &oe[i*32 + j], 4);
    }
  }
  for (state_index = 0; state_index < 16; ++state_index) {
    for (state_val = 0; state_val < 256; ++state_val) {
      BitVector vec1(8, state_val);
      vec1.MultPartialMat(0, 8*state_index,
                          128, 8*(state_index+1),
                          dm, &vec1);
      vec1.MultMat(mat, &vec1);
      for (out_index = 0; out_index < 16; ++out_index) {
        val = 0;
        vec1.PartialMultMat(8*out_index, 8*out_index + 4,
                            oe[state_index*32 + out_index*2],
                            &out_vec);
        val = out_vec.VecToVal(0, out_vec.Length());
        val <<= 4;
        vec1.PartialMultMat(8*out_index + 4, 8*(out_index+1),
                            oe[state_index*32 + out_index*2 + 1],
                            &out_vec);
        val |= out_vec.VecToVal(0, out_vec.Length());
        this->type_i_[state_index*256*16 + state_val*16 + out_index] = val;
      }
    }
  }
  GenerateAdderNetwork(15, id, encoding, &this->type_iv_);
}
// input1 and input2 -> * id(2 * 4 bit) and concatenate (8 bit) -> * l_inv (8 bit) -> TboxTyi tables (32 bit) -> * mb (32 bit) -> * oe (8 * 4 bit) -> out
// outs -> adder network -> input
// input1 and input2 -> * id(2 * 4 bit) and concatenate (8 bit) -> * mb_inv (8 bit) -> concatenated l (32 bit) -> * oe (8 * 4 bit) -> out
Round::Round(const vb &id, const vb &oe, const vb &l, const vb &l_inv, const vb &mb, const vb &mb_inv, const v(v32) &ty, const v8 &k, const bool &is_enc) {
  this->type_ = "round";
  this->type_ii_.resize(16*256);
  this->type_iii_.resize(16*256);
  vb id1(4*32), oe1(4*32), id2(4*32), oe2(4*32);
  vb aid1(32), aoe1(32);
  for (int i = 0; i < 32; ++i) {
    BitMatrix::GetMatPair(&aid1[i], &aoe1[i], 4);
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      BitMatrix::GetMatPair(&id1[i*32 + j], &oe1[i*32 + j], 4);
      BitMatrix::GetMatPair(&id2[i*32 + j], &oe2[i*32 + j], 4);
    }
  }
  vb tmp_vmat(8);
  BitMatrix tmp_mat;
  for (int i = 0; i < 16; ++i) {
    int r = i % 4, c = i / 4;
    for (int in1 = 0; in1 < 16; ++in1) {
      BitVector v1(4, in1);
      v1.MultMat(id[i*2], &v1);
      for (int in2 = 0; in2 < 16; ++in2) {
        BitVector v2(4, in2);
        v2.MultMat(id[i*2 + 1], &v2);
        int val = v1.VecToVal(0, 4)*16 + v2.VecToVal(0, 4);
        BitVector v3(8, val);
        v3.MultMat(l_inv[i], &v3);
        val = v3.VecToVal(0, 8);
        val = ty[r][AesBase::kSbox[val ^ k[i]]];
        BitVector v4(32, val);
        v4.MultMat(mb[c], &v4);
        for (int h = 0; h < 8; ++h) {
          tmp_vmat[h] = oe1[r*32 + c*8 + h];
        }
        BitMatrix::Concatenate(tmp_vmat, &tmp_mat);
        v4.MultMat(tmp_mat, &v4);
        this->type_ii_[i*256 + in1*16 + in2] = v4.VecToVal(0, 32);
      }
    }
  }
  v8 adder1;
  GenerateAdderNetwork(3, id1, aoe1, &adder1);
  for (int i = 0; i < adder1.size(); ++i) {
    this->type_iv_.push_back(adder1[i]);
  }
  vb tmp_l(4);
  BitMatrix tmp_lm;
  for (int i = 0; i < 16; ++i) {
    int r = i % 4, c = i / 4;
    for (int in1 = 0; in1 < 16; ++in1) {
      BitVector v1(4, in1);
      v1.MultMat(aid1[i*2], &v1);
      for (int in2 = 0; in2 < 16; ++in2) {
        BitVector v2(4, in2);
        v2.MultMat(aid1[i*2 + 1], &v2);
        int val = v1.VecToVal(0, 4)*16 + v2.VecToVal(0, 4);
        BitVector v3(8, val);
        v3.MultPartialMat(0, 8*r, 32, 8*(r+1), mb_inv[c], &v3);
        for (int h = 0; h < 4; ++h) {
          tmp_l[h] = l[c*4 + h];
        }
        BitMatrix::Concatenate(tmp_l, &tmp_lm);
        v3.MultMat(tmp_lm, &v3);
        for (int h = 0; h < 8; ++h) {
          tmp_vmat[h] = oe2[r*32 + c*8 + h];
        }
        BitMatrix::Concatenate(tmp_vmat, &tmp_mat);
        v3.MultMat(tmp_mat, &v3);
        this->type_iii_[i*256 + in1*16 + in2] = v3.VecToVal(0, 32);
      }
    }
  }
  v8 adder2;
  GenerateAdderNetwork(3, id2, oe, &adder2);
  for (int i = 0; i < adder2.size(); ++i) {
    this->type_iv_.push_back(adder2[i]);
  }
}
// input1 and input2 -> id (2 * 4 bit) and concatenate (8 bit) -> * l (8 bit) -> Tbox (8 bit) -> * dm (128 bit) -> out
Round::Round(const BitMatrix &dm, const vb &id, const vb &l, const v8 &k, const bool &is_idm, const bool &is_enc) {
  this->type_ = "odm";
  this->type_i_.resize(16 * 256 * 16);
  int state_index, state_val, out_index;
  BitVector out_vec;
  vb aid(16*32), aoe(16*32);
  vb oe(32);
  for (int i = 0; i < 32; ++i) {
    oe[i].SetMat(4, 4, "identity");
  }
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      BitMatrix::GetMatPair(&aid[i*32 + j], &aoe[i*32 + j], 4);
    }
  }
  for (state_index = 0; state_index < 16; ++state_index) {
    for (state_val = 0; state_val < 256; ++state_val) {
      BitVector v1(4, state_val >> 4), v2(4, state_val & 0xf);
      v1.MultMat(id[state_index*2], &v1);
      v2.MultMat(id[state_index*2 + 1], &v2);
      v1.Concatenate(v2);
      v1.MultMat(l[state_index], &v1);
      int val = v1.VecToVal(0, 8);
      val = AesBase::kSbox[val^k[state_index]] ^ k[state_index + 16];
      BitVector vec2(8, val);
      BitVector vec1;
      vec2.MultPartialMat(0, 8*state_index,
                          128, 8*(state_index+1),
                          dm, &vec1);
      for (out_index = 0; out_index < 16; ++out_index) {
        val = 0;
        vec1.PartialMultMat(8*out_index, 8*out_index + 4,
                            aoe[state_index*32 + out_index*2],
                            &out_vec);
        val = out_vec.VecToVal(0, out_vec.Length());
        val <<= 4;
        vec1.PartialMultMat(8*out_index + 4, 8*(out_index+1),
                            aoe[state_index*32 + out_index*2 + 1],
                            &out_vec);
        val |= out_vec.VecToVal(0, out_vec.Length());
        this->type_i_[state_index*256*16 + state_val*16 + out_index] = val;
      }
    }
  }
  GenerateAdderNetwork(15, aid, oe, &this->type_iv_);
}
Aes::Aes(const v8 &k) {
  // KeySchedule
  v32 w;
  u32 key_len = k.size() * 8;
  this->KeySchedule(k, key_len, &w);
  v(v8) round_key(this->Nr_ + 1);
  for (size_t i = 1; i <= this->Nr_ - 1; ++i) {
    round_key[i].resize(16);
    for (size_t j = 0; j < 4; ++j) {
      round_key[i][j*4 + 0] = (w[(i - 1)*4 + j] >> 24) & 0xff;
      round_key[i][j*4 + 1] = (w[(i - 1)*4 + j] >> 16) & 0xff;
      round_key[i][j*4 + 2] = (w[(i - 1)*4 + j] >> 8) & 0xff;
      round_key[i][j*4 + 3] = (w[(i - 1)*4 + j]) & 0xff;
    }
  }
  round_key[this->Nr_].resize(32);
  for (size_t j = 0; j < 4; ++j) {
    round_key[this->Nr_][j*4 + 0] = (w[(this->Nr_ - 1)*4 + j] >> 24) & 0xff;
    round_key[this->Nr_][j*4 + 1] = (w[(this->Nr_ - 1)*4 + j] >> 16) & 0xff;
    round_key[this->Nr_][j*4 + 2] = (w[(this->Nr_ - 1)*4 + j] >> 8) & 0xff;
    round_key[this->Nr_][j*4 + 3] = (w[(this->Nr_ - 1)*4 + j]) & 0xff;
    round_key[this->Nr_][j*4 + 0 + 16] = (w[this->Nr_*4 + j] >> 24) & 0xff;
    round_key[this->Nr_][j*4 + 1 + 16] = (w[this->Nr_*4 + j] >> 16) & 0xff;
    round_key[this->Nr_][j*4 + 2 + 16] = (w[this->Nr_*4 + j] >> 8) & 0xff;
    round_key[this->Nr_][j*4 + 3 + 16] = (w[this->Nr_*4 + j]) & 0xff;
  }
  // init round num
  this->round_.resize(this->Nr_ + 1);
  // Ty table generate
  v(v32) ty(4), ty_inv(4);
  v32 tyi(256), tyi_inv(256);
  for (size_t col = 0; col < 4; ++col) {
    for (size_t val = 0; val < 256; ++val) {
      tyi[val] = (Round::Gmult(val, AesBase::kMixMat[0][col]) << 24) |
                 (Round::Gmult(val, AesBase::kMixMat[1][col]) << 16) |
                 (Round::Gmult(val, AesBase::kMixMat[2][col]) << 8) |
                 (Round::Gmult(val, AesBase::kMixMat[3][col]));
      tyi_inv[val] = (Round::Gmult(val, AesBase::kInvMixMat[0][col]) << 24) |
                     (Round::Gmult(val, AesBase::kInvMixMat[1][col]) << 16) |
                     (Round::Gmult(val, AesBase::kInvMixMat[2][col]) << 8) |
                     (Round::Gmult(val, AesBase::kInvMixMat[3][col]));
    }
    ty[col] = tyi;
    ty_inv[col] = tyi_inv;
  }
  // IDM ODM matrix generate
  BitMatrix idm, idm_inv, odm, odm_inv;
  // BitMatrix::GetMatPair(&idm, &idm_inv, 128);
  // BitMatrix::GetMatPair(&odm, &odm_inv, 128);
  idm.SetMat(128, 128, "identity");
  odm.SetMat(128, 128, "identity");
  // L MB matrix generate
  v(vb) l(this->Nr_), l_inv(this->Nr_);
  v(vb) mb(this->Nr_ - 1), mb_inv(this->Nr_ - 1);
  for (size_t r = 0; r < this->Nr_ - 1; ++r) {
    mb[r].resize(4);
    mb_inv[r].resize(4);
    for (size_t i = 0; i < 4; ++i) {
      BitMatrix::GetMatPair(&mb[r][i], &mb_inv[r][i], 32);
    }
  }
  for (size_t r = 0; r < this->Nr_; ++r) {
    l[r].resize(16);
    l_inv[r].resize(16);
    for (size_t i = 0; i < 16; ++i) {
      BitMatrix::GetMatPair(&l[r][i], &l_inv[r][i], 8);
    }
  }
  // encoding and decoding
  v(vb) id(this->Nr_ + 1), oe(this->Nr_ + 1);
  for (size_t r = 0; r < this->Nr_ + 1; ++r) {
    id[r].resize(32), oe[r].resize(32);
    for (size_t i = 0; i < 32; ++i) {
      BitMatrix::GetMatPair(&id[r][i], &oe[r][i], 4);
    }
  }
  // IDM
  this->round_[0] = Round(idm, oe[0], l[0], true, true);
  // round 1 to Nr - 1
  for (size_t r = 1; r <= this->Nr_ - 1; ++r) {
    AesBase::ShiftVec<BitMatrix>(&id[r-1], 2);
    AesBase::ShiftVec<BitMatrix>(&l_inv[r-1], 1);
    AesBase::ShiftVec<uint8_t>(&round_key[r], 1);
    this->round_[r] = Round(id[r - 1], oe[r], l[r], l_inv[r - 1], mb[r - 1], mb_inv[r - 1], ty, round_key[r], true);
  }
  // round Nr with odm
  AesBase::ShiftVec<BitMatrix>(&id[this->Nr_ - 1], 2);
  AesBase::ShiftVec<BitMatrix>(&l_inv[this->Nr_ - 1], 1);
  AesBase::ShiftVec<uint8_t>(&round_key[this->Nr_], 1);
  this->round_[this->Nr_] = Round(odm, id[this->Nr_ - 1], l_inv[this->Nr_ - 1], round_key[this->Nr_], false, true);
}
void Aes::Encrypt(State *state) {
  auto it = this->round_.begin();
  while (it != this->round_.end()) {
    it->Encrypt(state);
    it++;
  }
}
void Round::Encrypt(State *state) {
  v(v(int)) vec(16);
  v(int) st(16);
  for (int i = 0; i < 16; ++i) {
    st[i] = state->GetState(i);
  }
  if (this->type_ == "round") {
    AesBase::ShiftVec<int>(&st, 1);
    for (int i = 0; i < 4; ++i) {
      vec[i].resize(32);
    }
    for (int i = 0; i < 4; ++i) {
      int a;
      for (int j = 0; j < 4; ++j) {
        a = this->type_ii_[(i+j*4)*256 + st[i + j * 4]];
        vec[i][j*8 + 7] = (a) & 0xf;
        vec[i][j*8 + 6] = (a >> 4) & 0xf;
        vec[i][j*8 + 5] = (a >> 8) & 0xf;
        vec[i][j*8 + 4] = (a >> 12) & 0xf;
        vec[i][j*8 + 3] = (a >> 16) & 0xf;
        vec[i][j*8 + 2] = (a >> 20) & 0xf;
        vec[i][j*8 + 1] = (a >> 24) & 0xf;
        vec[i][j*8 + 0] = (a >> 28) & 0xf;
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*256 + vec[0][i]*16 + vec[1][i]];
    }
    for (int i = 2; i < 4; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*32*256 + j*256 + vec[i-1][j]*16 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[3][i*2] << 4) | vec[3][i*2 + 1];
    }
    for (int i = 0; i < 16; ++i) {
      state->SetState(i, st[i]);
    }
    for (int i = 0; i < 4; ++i) {
      int a;
      for (int j = 0; j < 4; ++j) {
        a = this->type_iii_[(i+j*4)*256 + st[i + j * 4]];
        vec[i][j*8 + 7] = (a) & 0xf;
        vec[i][j*8 + 6] = (a >> 4) & 0xf;
        vec[i][j*8 + 5] = (a >> 8) & 0xf;
        vec[i][j*8 + 4] = (a >> 12) & 0xf;
        vec[i][j*8 + 3] = (a >> 16) & 0xf;
        vec[i][j*8 + 2] = (a >> 20) & 0xf;
        vec[i][j*8 + 1] = (a >> 24) & 0xf;
        vec[i][j*8 + 0] = (a >> 28) & 0xf;
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[24576 + i*256 + vec[0][i]*16 + vec[1][i]];
    }
    for (int i = 2; i < 4; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[24576 + (i-1)*32*256 + j*256 + vec[i-1][j]*16 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[3][i*2] << 4) | vec[3][i*2 + 1];
    }
  } else if (this->type_ == "idm") {
    for (int i = 0; i < 16; ++i) {
      vec[i].resize(32);
    }
    for (int i = 0; i < 16; ++i) {
      int a;
      for (int j = 0; j < 16; ++j) {
        a = this->type_i_[i*256*16 + st[i]*16 + j];
        vec[i][j*2] = a >> 4;
        vec[i][j*2 + 1] = a & 0xf;
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*256 + vec[0][i]*16 + vec[1][i]];
    }
    for (int i = 2; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*32*256 + j*256 + vec[i-1][j]*16 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[15][i*2] << 4) | vec[15][i*2 + 1];
    }
  } else {
    AesBase::ShiftVec<int>(&st, 1);
    for (int i = 0; i < 16; ++i) {
      vec[i].resize(32);
    }
    for (int i = 0; i < 16; ++i) {
      int a;
      for (int j = 0; j < 16; ++j) {
        a = this->type_i_[i*256*16 + st[i]*16 + j];
        vec[i][j*2] = a >> 4;
        vec[i][j*2 + 1] = a & 0xf;
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*256 + vec[0][i]*16 + vec[1][i]];
    }
    for (int i = 2; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*32*256 + j*256 + vec[i-1][j]*16 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[15][i*2] << 4) | vec[15][i*2 + 1];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, st[i]);
  }
}
} // namespace cejo