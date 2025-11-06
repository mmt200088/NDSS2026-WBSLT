#include "shiy.h"
#include "../../libs/random/random.h"
#include <algorithm>
#include <stdlib.h>

namespace shiy {
using AesBase = aes::AesBase;
using BitVector = wbmatrix::BitVector;
using BitMatrix = wbmatrix::BitMatrix;
using Random = myrandom::Random;
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

// #define FAST_GEN
// #define CANCLE_MB_ENC
// #define CANCLE_L_ENC
// #define CANCLE_ROUND_ENC
// #define CANCLE_TYPEI_ENC
// #define CANCLE_TYPEII_ENC
// #define CANCLE_TYPEIII_ENC
// #define CANCLE_TYPEIV_ENC

void GenerateAdderNetwork(const int &dim_row,
                          // const vb &id,
                          // const vb &oe,
                          const v(v(v16)) &id,
                          const v(v16) &oe,
                          const bool &is_nsnl,
                          const bool &is_pa,
                          const int &q,
                          v32 *adder) {
  uint64_t val, val1, val2;
  int t;
  if (is_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  int in_val = 1 << t;
  adder->resize(dim_row*32*in_val*in_val);
  // v(vb) aid(dim_row), aoe(dim_row);
  v(v(v16)) aid(dim_row), aoe(dim_row);
  for (int i = 0; i < dim_row - 1; ++i) {
    aid[i].resize(32), aoe[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      // BitMatrix::GetMatPair(&aid[i][j], &aoe[i][j], t);
      Random::GenPerm(in_val, &aid[i][j], &aoe[i][j]);
      #ifdef CANCLE_TYPEIV_ENC
        aid[i][j].SetMat(t, t, "identity");
        aoe[i][j].SetMat(t, t, "identity");
      #endif
    }
  }

  #ifdef FAST_GEN
    return;
  #endif

  for (int r = 0; r < dim_row; ++r) {
    for (int i = 0; i < 32; ++i) {
      for (int in1 = 0; in1 < in_val; ++in1) {
        // BitVector v1(t, in1);
        if (r == 0) {
          // v1.MultMat(id[i], &v1);
          val1 = id[r][i][in1];
        } else {
          // v1.MultMat(aid[r - 1][i], &v1);
          val1 = aid[r - 1][i][in1];
        }
        for (int in2 = 0; in2 < in_val; ++in2) {
          // BitVector v2(t, in2);
          // v2.MultMat(id[(r+1)*32 + i], &v2);
          val2 = id[r+1][i][in2];
          // val1 = v1.VecToVal(0, t);
          // val2 = v2.VecToVal(0, t);
          val = val1 ^ val2;
          if (r == dim_row - 1) {
            if (is_pa) {
              val >>= q;
              val <<= q*2;
              val |= (val1 & ((1 << q) - 1)) << q;
              val |= val2 & ((1 << q) - 1);
            }
            // BitVector vout(t + q, val);
            // vout.MultMat(oe[i], &vout);
            val = oe[i][val];
            // (*adder)[r*32*in_val*in_val + i*in_val*in_val + in1*in_val + in2] = vout.VecToVal(0, t + q);
            (*adder)[r*32*in_val*in_val + i*in_val*in_val + in1*in_val + in2] = val;
          } else {
            // BitVector vout(t, val);
            // vout.MultMat(aoe[r][i], &vout);
            val = aoe[r][i][val];
            // (*adder)[r*32*in_val*in_val + i*in_val*in_val + in1*in_val + in2] = vout.VecToVal(0, t);
            (*adder)[r*32*in_val*in_val + i*in_val*in_val + in1*in_val + in2] = val;
          }
        }
      }
    }
  }
}
void GenerateTypeII(//const vb &id,
                    // const vb &oe,
                    const v(v16) &id,
                    const v(v(v16)) &oe,
                    const vb &l,
                    const vb &mb,
                    const v(v32) &ty,
                    const v8 &k,
                    const bool &pre_pa,
                    const int &pre_q,
                    const bool &cur_nsnl,
                    v32 *table) {
  int r, c, t;
  if (cur_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  int in_val = 1 << (t+pre_q);
  uint64_t val1, val2, val;
  BitVector vout;
  table->resize(16*in_val*in_val*8);

  #ifdef FAST_GEN
    return;
  #endif

  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int in1 = 0; in1 < in_val; ++in1) {
      // BitVector v1(t+pre_q, in1);
      // uint16_t v1 = in1;
      // v1.MultMat(id[i*2], &v1);
      // val1 = v1.VecToVal(0, t+pre_q);
      val1 = id[i*2][in1];
      if (pre_pa) {
        val1 = ((val1>>(pre_q*2))<<pre_q) | (((val1>>pre_q)&((1<<pre_q)-1)) ^ (val1&((1<<pre_q)-1)));
      }
      for (int in2 = 0; in2 < in_val; ++in2) {
        // BitVector v2(t+pre_q, in2);
        // v2.MultMat(id[i*2 + 1], &v2);
        // val2 = v2.VecToVal(0, t+pre_q);
        // uint16_t v2 = in2;
        val2 = id[i*2+1][in2];
        if (pre_pa) {
          val2 = ((val2>>(pre_q*2))<<pre_q) | (((val2>>pre_q)&((1<<pre_q)-1)) ^ (val2&((1<<pre_q)-1)));
        }
        val = (val1 << t) | val2;
        BitVector v3(t*2, val);
        v3.MultMat(l[i], &v3);
        val = v3.VecToVal(0, t*2);
        if (cur_nsnl) {
          val >>= 4;
        }
        val = ty[r][AesBase::kSbox[val^k[i]]];
        if (cur_nsnl) {
          val = (((((val>>24)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<36) |
                (((((val>>16)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<24) |
                (((((val>>8)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<12) |
                (((((val)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16)));
        }
        BitVector v4(t*8, val);
        v4.MultMat(mb[c], &v4);
        for (int oi = 0; oi < 8; ++oi) {
          val = v4.VecToVal(t*oi, t*oi+t);
          (*table)[i*in_val*in_val*8 + in1*in_val*8 + in2*8 + oi] = oe[r][c*8+oi][val];
          // v4.PartialMultMat(t*oi, t*oi+t, oe[r*32+c*8+oi], &vout);
          // (*table)[i*in_val*in_val*8 + in1*in_val*8 + in2*8 + oi] = vout.VecToVal(0, t);
        }
      }
    }
  }
}
void GenerateTypeIII(// const vb &id,
                    //  const vb &oe,
                     const v(v16) &id,
                     const v(v(v16)) &oe,
                     const vb &l_inv,
                     const vb &mb_inv,
                     const bool &cur_nsnl,
                     const bool &cur_pa,
                     const int &cur_q,
                     const bool &nxt_nsnl,
                     v32 *table) {
  int t;
  if (cur_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  int in_val = 1 << (t+cur_q);
  int r, c;
  uint64_t val1, val2, val;
  BitVector vout;
  vb lv(4);
  BitMatrix lm;
  table->resize(16*in_val*in_val*8);

  #ifdef FAST_GEN
    return;
  #endif

  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int in1 = 0; in1 < in_val; ++in1) {
      // BitVector v1(t+cur_q, in1);
      // v1.MultMat(id[i*2], &v1);
      // val1 = v1.VecToVal(0, t+cur_q);
      val1 = id[i*2][in1];
      if (cur_pa) {
        val1 = ((val1>>(cur_q*2))<<cur_q) | (((val1>>cur_q)&((1<<cur_q)-1)) ^ (val1&((1<<cur_q)-1)));
      }
      for (int in2 = 0; in2 < in_val; ++in2) {
        // BitVector v2(t+cur_q, in2);
        // v2.MultMat(id[i*2 + 1], &v2);
        // val2 = v2.VecToVal(0, t+cur_q);
        val2 = id[i*2+1][in2];
        if (cur_pa) {
          val2 = ((val2>>(cur_q*2))<<cur_q) | (((val2>>cur_q)&((1<<cur_q)-1)) ^ (val2&((1<<cur_q)-1)));
        }
        val = (val1 << t) | val2;
        BitVector v3(t*2, val);
        v3.MultPartialMat(0, r*t*2, t*8, (r+1)*t*2, mb_inv[c], &v3);
        val = v3.VecToVal(0, t*8);
        if (cur_nsnl && !nxt_nsnl) {
          t = 4;
          val = ((val>>40)<<24) | (((val>>28)&0xff)<<16) | (((val>>16)&0xff)<<8) | ((val>>4)&0xff);
        }
        if (!cur_nsnl && nxt_nsnl) {
          t = 6;
          val = ((((val>>24)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<36) | 
                (((((val>>16)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<24) | 
                (((((val>>8)&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16))<<12) | 
                (((val&0xff)<<4)|(Random::RandomGen(0, 1e9+7)%16));
        }
        BitVector v4(t*8, val);
        for (int h = 0; h < 4; ++h) {
          lv[h] = l_inv[c*4 + h];
        }
        BitMatrix::Concatenate(lv, &lm);
        v4.MultMat(lm, &v4);
        for (int oi = 0; oi < 8; ++oi) {
          // v4.PartialMultMat(t*oi, t*oi+t, oe[r*32+c*8+oi], &vout);
          // (*table)[i*in_val*in_val*8 + in1*in_val*8 + in2*8 + oi] = vout.VecToVal(0, t);
          val = v4.VecToVal(t*oi, t*oi+t);
          (*table)[i*in_val*in_val*8 + in1*in_val*8 + in2*8 + oi] = oe[r][c*8+oi][val];
        }
        if (cur_nsnl) {
          t = 6;
        } else {
          t = 4;
        }
      }
    }
  }
}
void Round::Encrypt(State *state) {
  v(int) st(16);
  v(v(int)) vec(16);
  for (int i = 0; i < 16; ++i) {
    st[i] = state->GetState(i);
    vec[i].resize(32);
  }
  if (this->type_ == "round") {
    AesBase::ShiftVec<int>(&st, 1);
    int in_val2 = this->type_ii_.size() / 16 / 8;
    int in_val3 = this->type_iii_.size() / 16 / 8;
    int in_val4 = this->type_iv_in1_;
    for (int i = 0; i < 16; ++i) {
      int r = i % 4, c = i / 4;
      for (int j = 0; j < 8; ++j) {
        vec[r][c*8 + j] = this->type_ii_[i*in_val2*8 + st[i]*8 + j];
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*in_val4*in_val4 + vec[0][i]*in_val4 + vec[1][i]];
    }
    for (int i = 2; i < 4; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*in_val4*in_val4*32 + j*in_val4*in_val4 + vec[i-1][j]*in_val4 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[3][i*2]<<this->type_iv_outbit1_) | vec[3][i*2 + 1];
    }
    int iv_offset = 3*in_val4*in_val4*32;
    in_val4 = this->type_iv_in2_;
    for (int i = 0; i < 16; ++i) {
      int r = i % 4, c = i / 4;
      for (int j = 0; j < 8; ++j) {
        vec[r][c*8 + j] = this->type_iii_[i*in_val3*8 + st[i]*8 + j];
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[iv_offset + i*in_val4*in_val4 + vec[0][i]*in_val4 + vec[1][i]];
    }
    for (int i = 2; i < 4; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[iv_offset + (i-1)*in_val4*in_val4*32 + j*in_val4*in_val4 + vec[i-1][j]*in_val4 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[3][i*2]<<this->type_iv_outbit2_) | vec[3][i*2 + 1];
    }
  } else if (this->type_ == "idm") {
    int in_val1 = this->type_i_.size() / 16 / 32;
    int in_val4 = sqrt(this->type_iv_.size() / 15 / 32);
    for (int i = 0; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_i_[i*in_val1*32 + st[i]*32 + j];
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*in_val4*in_val4 + vec[0][i]*in_val4 + vec[1][i]];
    }
    for (int i = 2; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*in_val4*in_val4*32 + j*in_val4*in_val4 + vec[i-1][j]*in_val4 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[15][i*2]<<this->type_iv_outbit1_) | vec[15][i*2 + 1];
    }
  } else {
    AesBase::ShiftVec<int>(&st, 1);
    int in_val1 = this->type_i_.size() / 16 / 32;
    int in_val4 = sqrt(this->type_iv_.size() / 15 / 32);
    printf("%d\n", in_val4);
    for (int i = 0; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_i_[i*in_val1*32 + st[i]*32 + j];
      }
    }
    for (int i = 0; i < 32; ++i) {
      vec[1][i] = this->type_iv_[i*in_val4*in_val4 + vec[0][i]*in_val4 + vec[1][i]];
    }
    for (int i = 2; i < 16; ++i) {
      for (int j = 0; j < 32; ++j) {
        vec[i][j] = this->type_iv_[(i-1)*in_val4*in_val4*32 + j*in_val4*in_val4 + vec[i-1][j]*in_val4 + vec[i][j]];
      }
    }
    for (int i = 0; i < 16; ++i) {
      st[i] = (vec[15][i*2]<<this->type_iv_outbit1_) | vec[15][i*2 + 1];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, st[i]);
  }
}
Round::Round(const BitMatrix &dm,
            //  const vb &encoding,
             const v(v16) &encoding,
             const vb &l_inv,
             const bool &nxt_nsnl,
             const bool &cur_pa,
             const int &cur_q) {
  this->type_ = "idm";
  int t;
  if (nxt_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  this->type_iv_outbit1_ = t + cur_q;
  int r, c;
  // vb id(16*32), oe(16*32);
  v(v(v16)) id(16), oe(16);
  uint64_t val;
  BitVector vout;
  for (int i = 0; i < 16; ++i) {
    id[i].resize(32);
    oe[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      // BitMatrix::GetMatPair(&id[i*32+j], &oe[i*32+j], t);
      Random::GenPerm(1<<t, &id[i][j], &oe[i][j]);
      #ifdef CANCLE_TYPEI_ENC
        id[i*32+j].SetMat(t,t,"identity");
        oe[i*32+j].SetMat(t,t,"identity");
      #endif
    }
  }
  GenerateAdderNetwork(15, id, encoding, nxt_nsnl, cur_pa, cur_q, &this->type_iv_);
  BitMatrix lm;
  BitMatrix::Concatenate(l_inv, &lm);
  this->type_i_.resize(16*256*32);
  this->type_i_outbit_ = (t + 7) / 8 * 8;
  this->type_ii_outbit_ = 0;
  this->type_iii_outbit_ = 0;
  this->type_iv_outbit_ = 8;

  #ifdef FAST_GEN
    return;
  #endif

  uint16_t out;
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 256; ++j) {
      val = j;
      if (nxt_nsnl) {
        val = (val << 4) | (Random::RandomGen(0, 1e9+7)%16);
      }
      BitVector v1(t*2, val);
      v1.MultPartialMat(0, t*2*i, t*32, t*2*(i+1), dm, &v1);
      v1.MultMat(lm, &v1);
      for (int oi = 0; oi < 32; ++oi) {
        out = v1.VecToVal(t*oi, t*oi+t);
        // v1.PartialMultMat(t*oi, t*oi+t, oe[i*32+oi], &vout);
        // this->type_i_[i*256*32 + j*32 + oi] = vout.VecToVal(0, t);
        this->type_i_[i*256*32 + j*32 + oi] = oe[i][oi][out];
      }
    }
  }
}
Round::Round(// const std::vector<BitMatrix> &id,
            //  const std::vector<BitMatrix> &oe,
             const v(v16) &id,
             const v(v16) &oe,
             const std::vector<BitMatrix> &l,
             const std::vector<BitMatrix> &l_inv,
             const std::vector<BitMatrix> &mb,
             const std::vector<BitMatrix> &mb_inv,
             const std::vector<std::vector<uint32_t> > &ty,
             const std::vector<uint8_t> &k,
             const bool &pre_pa,
             const int &pre_q,
             const bool &cur_nsnl,
             const bool &cur_pa,
             const int &cur_q,
             const bool &nxt_nsnl) {
  this->type_ = "round";
  int t;
  if (cur_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  this->type_iv_in1_ = 1<<t;
  this->type_iv_outbit1_ = t + cur_q;
  // vb id1(4*32), oe1(4*32), id2(4*32), oe2(4*32);
  // vb aid1(32), aoe1(32);
  v(v(v16)) id1(4), oe1(4), id2(4), oe2(4);
  v(v16) aid1(32), aoe1(32);
  for (int i = 0; i < 32; ++i) {
    // BitMatrix::GetMatPair(&aid1[i], &aoe1[i], t+cur_q);
    Random::GenPerm(1<<(t+cur_q), &aid1[i], &aoe1[i]);
    #ifdef CANCLE_TYPEIV_ENC
      aid1[i].SetMat(t+cur_q, t+cur_q, "identity");
      aoe1[i].SetMat(t+cur_q, t+cur_q, "identity");
    #endif
  }
  for (int i = 0; i < 4; ++i) {
    id1[i].resize(32);
    oe1[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      // BitMatrix::GetMatPair(&id1[i*32 + j], &oe1[i*32 + j], t);
      Random::GenPerm(1<<t, &id1[i][j], &oe1[i][j]);
      #ifdef CANCLE_TYPEII_ENC
        id1[i*32+j].SetMat(t,t,"identity");
        oe1[i*32+j].SetMat(t,t,"identity");
      #endif
    }
  }
  if (nxt_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  this->type_iv_in2_ = 1<<t;
  this->type_iv_outbit2_ = t + cur_q;
  for (int i = 0; i < 4; ++i) {
    id2[i].resize(32);
    oe2[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      // BitMatrix::GetMatPair(&id2[i*32 + j], &oe2[i*32 + j], t);
      Random::GenPerm(1<<t, &id2[i][j], &oe2[i][j]);
      #ifdef CANCLE_TYPEIII_ENC
        id2[i*32+j].SetMat(t,t,"identity");
        oe2[i*32+j].SetMat(t,t,"identity");
      #endif
    }
  }
  GenerateTypeII(id, oe1, l, mb, ty, k, pre_pa, pre_q, cur_nsnl, &this->type_ii_);
  v32 adder1;
  GenerateAdderNetwork(3, id1, aoe1, cur_nsnl, cur_pa, cur_q, &adder1);
  for (int i = 0; i < adder1.size(); ++i) {
    this->type_iv_.push_back(adder1[i]);
  }
  GenerateTypeIII(aid1, oe2, l_inv, mb_inv, cur_nsnl, cur_pa, cur_q, nxt_nsnl, &this->type_iii_);
  v32 adder2;
  GenerateAdderNetwork(3, id2, oe, nxt_nsnl, cur_pa, cur_q, &adder2);
  for (int i = 0; i < adder2.size(); ++i) {
    this->type_iv_.push_back(adder2[i]);
  }
  this->type_i_outbit_ = 0;
  this->type_ii_outbit_ = 8;
  this->type_iii_outbit_ = 8;
  if (cur_q > 2) {
    this->type_iv_outbit_ = 16;
  } else {
    this->type_iv_outbit_ = 8;
  }
}
Round::Round(const BitMatrix &dm,
            //  const std::vector<BitMatrix> &encoding,
             const v(v16) &encoding,
             const std::vector<BitMatrix> &l_inv,
             const std::vector<uint8_t> &k,
             const bool &pre_pa,
             const int &pre_q,
             const bool &cur_nsnl,
             const bool &cur_pa,
             const int &cur_q) {
  this->type_ = "odm";
  int t;
  if (cur_nsnl) {
    t = 6;
  } else {
    t = 4;
  }
  this->type_iv_outbit1_ = t + cur_q;
  int in_val = 1 << (t+pre_q);
  int r, c;
  uint64_t val, val1, val2;
  // vb id(16*32), oe(16*32), aoe(32);
  v(v(v16)) id(16), oe(16);
  v(v16) aoe(32);
  BitVector vout;
  for (int i = 0; i < 16; ++i) {
    id[i].resize(32);
    oe[i].resize(32);
    for (int j = 0; j < 32; ++j) {
      // BitMatrix::GetMatPair(&id[i*32+j], &oe[i*32+j], t);
      Random::GenPerm(1<<t, &id[i][j], &oe[i][j]);
      #ifdef CANCLE_TYPEI_ENC
        id[i*32+j].SetMat(t, t, "identity");
        oe[i*32+j].SetMat(t, t, "identity");
      #endif
    }
  }
  for (int i = 0; i < 32; ++i) {
    // aoe[i].SetMat(t+cur_q, t+cur_q, "identity");
    aoe[i].resize(1<<(t+cur_q));
    for (int j = 0; j < (1<<(t+cur_q)); ++j) {
      aoe[i][j]=j;
    }
  }
  GenerateAdderNetwork(15, id, aoe, cur_nsnl, cur_pa, cur_q, &this->type_iv_);
  this->type_i_.resize(16*in_val*in_val*32);
  this->type_i_outbit_ = 8;
  this->type_ii_outbit_ = 0;
  this->type_iii_outbit_ = 0;
  if (cur_q > 2) {
    this->type_iv_outbit_ = 16;
  } else {
    this->type_iv_outbit_ = 8;
  }

  #ifdef FAST_GEN
    return;
  #endif

  uint16_t out;

  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int in1 = 0; in1 < in_val; ++in1) {
      // BitVector v1(t+pre_q, in1);
      // v1.MultMat(encoding[i*2], &v1);
      // val1 = v1.VecToVal(0, t+pre_q);
      val1 = encoding[i*2][in1];
      if (pre_pa) {
        val1 = ((val1>>(pre_q*2))<<pre_q) | (((val1>>pre_q)&((1<<pre_q)-1)) ^ (val1&((1<<pre_q)-1)));
      }
      for (int in2 = 0; in2 < in_val; ++in2) {
        // BitVector v2(t+pre_q, in2);
        // v2.MultMat(encoding[i*2 + 1], &v2);
        // val2 = v2.VecToVal(0, t+pre_q);
        val2 = encoding[i*2+1][in2];
        if (pre_pa) {
          val2 = ((val2>>(pre_q*2))<<pre_q) | (((val2>>pre_q)&((1<<pre_q)-1)) ^ (val2&((1<<pre_q)-1)));
        }
        val = (val1 << t) | val2;
        BitVector v3(t*2, val);
        v3.MultMat(l_inv[i], &v3);
        val = v3.VecToVal(0, t*2);
        if (cur_nsnl) {
          val >>= 4;
        }
        val = AesBase::kSbox[val ^ k[i]] ^ k[i + 16];
        if (cur_nsnl) {
          val = (val << 4) | (Random::RandomGen(0, 1e9+7)%16);
        }
        BitVector v4(t*2, val);
        v4.MultPartialMat(0, t*2*i, t*32, t*2*(i+1), dm, &v4);
        for (int oi = 0; oi < 32; ++oi) {
          // v4.PartialMultMat(t*oi, t*(oi+1), oe[i*32+oi], &vout);
          // this->type_i_[i*in_val*in_val*32 + in1*in_val*32 + in2*32 + oi] = vout.VecToVal(0, t);
          out = v4.VecToVal(t*oi, t*oi+t);
          this->type_i_[i*in_val*in_val*32 + in1*in_val*32 + in2*32 + oi] = oe[i][oi][out];
        }
      }
    }
  }
}
uint64_t Round::CalcTypeISize() {
  return this->type_i_outbit_*this->type_i_.size();
}
uint64_t Round::CalcTypeIISize() {
  return this->type_ii_outbit_*this->type_ii_.size();
}
uint64_t Round::CalcTypeIIISize() {
  return this->type_iii_outbit_*this->type_iii_.size();
}
uint64_t Round::CalcTypeIVSize() {
  return this->type_iv_outbit_*this->type_iv_.size();
}
void Round::PrintTable(const int &index, FILE* fp) {
  if (this->type_ == "round") {
    int in_val2 = this->type_ii_.size() / 16 / 8;
    int in_val3 = this->type_iii_.size() / 16 / 8;
    int in_val4 = this->type_iv_in1_;
    int iv_offset = 3*in_val4*in_val4*32;
    fprintf(fp, "offset%da=%d\n", index, in_val4);
    fprintf(fp, "type_ii%d=[", index);
    for (int i = 0; i < 16; ++i) {
      fprintf(fp, "[");
      for (int j = 0; j < in_val2; ++j) {
        fprintf(fp, "[");
        for (int k = 0; k < 8; ++k) {
          if (k == 7) fprintf(fp, "%d", this->type_ii_[i*in_val2*8+j*8+k]);
          else fprintf(fp, "%d,", this->type_ii_[i*in_val2*8+j*8+k]);
        }
        if (j == in_val2 - 1) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 15) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");

    fprintf(fp, "type_iii%d=[", index);
    for (int i = 0; i < 16; ++i) {
      fprintf(fp, "[");
      for (int j = 0; j < in_val3; ++j) {
        fprintf(fp, "[");
        for (int k = 0; k < 8; ++k) {
          if (k == 7) fprintf(fp, "%d", this->type_iii_[i*in_val2*8+j*8+k]);
          else fprintf(fp, "%d,", this->type_iii_[i*in_val3*8+j*8+k]);
        }
        if (j == in_val3 - 1) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 15) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");

    fprintf(fp, "type_iv%da=[", index);
    for (int i = 0; i < 3; ++i) {
      fprintf(fp, "[");
      for (int h = 0; h < 32; ++h) {
        fprintf(fp, "[");
        for (int j = 0; j < in_val4; ++j) {
          fprintf(fp, "[");
          for (int k = 0; k < in_val4; ++k) {
            if (k == in_val4 - 1) fprintf(fp, "%d", this->type_iv_[i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
            else fprintf(fp, "%d,", this->type_iv_[i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
          }
          if (j == in_val4 - 1) fprintf(fp, "]");
          else fprintf(fp, "],");
        }
        if (h == 31) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 2) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");
    in_val4 = this->type_iv_in2_;
    fprintf(fp, "offset%db=%d\n", index, in_val4);
    fprintf(fp, "type_iv%db=[", index);
    for (int i = 0; i < 3; ++i) {
      fprintf(fp, "[");
      for (int h = 0; h < 32; ++h) {
        fprintf(fp, "[");
        for (int j = 0; j < in_val4; ++j) {
          fprintf(fp, "[");
          for (int k = 0; k < in_val4; ++k) {
            if (k == in_val4 - 1) fprintf(fp, "%d", this->type_iv_[iv_offset+i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
            else fprintf(fp, "%d,", this->type_iv_[iv_offset+i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
          }
          if (j == in_val4 - 1) fprintf(fp, "]");
          else fprintf(fp, "],");
        }
        if (h == 31) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 2) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");
  } else {
    int in_val1 = this->type_i_.size() / 16 / 32;
    int in_val4 = sqrt(this->type_iv_.size() / 15 / 32);
    fprintf(fp, "offset%d=%d\n", index, in_val4);
    fprintf(fp, "type_i%d=[", index);
    for (int i = 0; i < 16; ++i) {
      fprintf(fp, "[");
      for (int j = 0; j < in_val1; ++j) {
        fprintf(fp, "[");
        for (int k = 0; k < 32; ++k) {
          if (k == 31) fprintf(fp, "%d", this->type_i_[i*in_val1*32+j*32+k]);
          else fprintf(fp, "%d,", this->type_i_[i*in_val1*32+j*32+k]);
        }
        if (j == in_val1 - 1) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 15) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");

    fprintf(fp, "type_iv%d=[", index);
    for (int i = 0; i < 15; ++i) {
      fprintf(fp, "[");
      for (int h = 0; h < 32; ++h) {
        fprintf(fp, "[");
        for (int j = 0; j < in_val4; ++j) {
          fprintf(fp, "[");
          for (int k = 0; k < in_val4; ++k) {
            if (k == in_val4 - 1) fprintf(fp, "%d", this->type_iv_[i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
            else fprintf(fp, "%d,", this->type_iv_[i*32*in_val4*in_val4+h*in_val4*in_val4+j*in_val4+k]);
          }
          if (j == in_val4 - 1) fprintf(fp, "]");
          else fprintf(fp, "],");
        }
        if (h == 31) fprintf(fp, "]");
        else fprintf(fp, "],");
      }
      if (i == 14) fprintf(fp, "]");
      else fprintf(fp, "],");
    }
    fprintf(fp, "]\n");
  }
}

void Aes::InitKey(const v8 &k) {
  int keylen = k.size() * 8;
  aes::Aes::KeySchedule(k, keylen, &this->k_);
  this->pa_.resize(this->Nr_ + 1);
  this->nsnl_.resize(this->Nr_ + 1);
  this->q_.resize(this->Nr_ + 1);
  for (int i = 0; i < this->Nr_ + 1; ++i) {
    this->pa_[i] = false;
    this->nsnl_[i] = false;
    this->q_[i] = 0;
  }
}
void Aes::InitRound() {
  if (this->k_.empty()) return;
  this->round_.resize(this->Nr_ + 1);
  auto w = this->k_;
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
  int t, q;
  // IDM ODM matrix generate
  BitMatrix idm, idm_inv, odm, odm_inv;
  if (this->nsnl_[1]) {
    t = 6;
  } else {
    t = 4;
  }
  // BitMatrix::GetMatPair(&idm, &idm_inv, t*32);
  idm.SetMat(t*32, t*32, "identity");
  if (this->nsnl_[this->Nr_]) {
    t = 6;
  } else {
    t = 4;
  }
  // BitMatrix::GetMatPair(&odm, &odm_inv, t*32);
  odm.SetMat(t*32, t*32, "identity");

  // L MB matrix generate
  v(vb) l(this->Nr_ + 1), l_inv(this->Nr_ + 1);
  v(vb) mb(this->Nr_), mb_inv(this->Nr_);
  for (size_t r = 1; r < this->Nr_; ++r) {
    if (this->nsnl_[r]) {
      t = 6;
    } else {
      t = 4;
    }
    mb[r].resize(4);
    mb_inv[r].resize(4);
    for (size_t i = 0; i < 4; ++i) {
      BitMatrix::GetMatPair(&mb[r][i], &mb_inv[r][i], t*8);
      #ifdef CANCLE_MB_ENC
        mb[r][i].SetMat(t*8,t*8,"identity");
        mb_inv[r][i].SetMat(t*8,t*8,"identity");
      #endif
    }
  }
  for (size_t r = 1; r <= this->Nr_; ++r) {
    l[r].resize(16);
    l_inv[r].resize(16);
    for (size_t i = 0; i < 16; ++i) {
      BitMatrix::GetMatPair(&l[r][i], &l_inv[r][i], 8);
      #ifdef CANCLE_L_ENC
        l[r][i].SetMat(8,8,"identity");
        l_inv[r][i].SetMat(8,8,"identity");
      #endif
    }
    if (this->nsnl_[r]) {
      for (int i = 0; i < 16; ++i) {
        l[r][i].Expand();
        l_inv[r][i].Expand();
      }
    }
  }
  // encoding and decoding
  // v(vb) id(this->Nr_), oe(this->Nr_);
  v(v(v16)) id(this->Nr_), oe(this->Nr_);
  for (size_t r = 0; r < this->Nr_; ++r) {
    if (this->nsnl_[r + 1]) {
      t = 6;
    } else {
      t = 4;
    }
    q = this->q_[r];
    id[r].resize(32), oe[r].resize(32);
    for (size_t i = 0; i < 32; ++i) {
      // BitMatrix::GetMatPair(&id[r][i], &oe[r][i], t + q);
      Random::GenPerm(1<<(t+q), &id[r][i], &oe[r][i]);
      #ifdef CANCLE_ROUND_ENC
        id[r][i].SetMat(t+q,t+q,"identity");
        oe[r][i].SetMat(t+q,t+q,"identity");
      #endif
    }
  }
  // IDM
  printf("idm\n");
  this->round_[0] = Round(idm, oe[0], l_inv[1], this->nsnl_[1], this->pa_[0], this->q_[0]);
  // round 1 to Nr - 1
  for (size_t r = 1; r <= this->Nr_ - 1; ++r) {
    printf("round %d\n", r);
    // AesBase::ShiftVec<BitMatrix>(&id[r-1], 2);
    AesBase::ShiftVec<v16>(&id[r-1], 2);
    AesBase::ShiftVec<BitMatrix>(&l[r], 1);
    AesBase::ShiftVec<uint8_t>(&round_key[r], 1);
    this->round_[r] = Round(id[r - 1], oe[r], l[r], l_inv[r + 1], mb[r], mb_inv[r], ty, round_key[r], this->pa_[r-1], this->q_[r-1], this->nsnl_[r], this->pa_[r], this->q_[r], this->nsnl_[r+1]);
  }
  // round Nr with odm
  printf("odm\n");
  // AesBase::ShiftVec<BitMatrix>(&id[this->Nr_ - 1], 2);
  AesBase::ShiftVec<v16>(&id[this->Nr_ - 1], 2);
  AesBase::ShiftVec<BitMatrix>(&l[this->Nr_], 1);
  AesBase::ShiftVec<uint8_t>(&round_key[this->Nr_], 1);
  this->round_[this->Nr_] = Round(odm, id[this->Nr_ - 1], l[this->Nr_], round_key[this->Nr_], this->pa_[this->Nr_-1],this->q_[this->Nr_-1], this->nsnl_[this->Nr_], this->pa_[this->Nr_], this->q_[this->Nr_]);
}
void Aes::Encrypt(State *state) {
  for (auto it = this->round_.begin(); it < this->round_.end(); ++it) {
    it->Encrypt(state);
  }
}
void Aes::EnablePartialAdder(const int &round_index, const int &q) {
  if (round_index > this->Nr_) return;
  this->pa_[round_index] = true;
  this->q_[round_index] = q;
}
void Aes::EnableNSNL(const int &round_index) {
  if (round_index > this->Nr_) return;
  this->nsnl_[round_index] = true;
}
void Aes::PrintTable(FILE *fp) {
  int round_num = 0;
  for (auto r : this->round_) {
    r.PrintTable(round_num, fp);
    round_num++;
  }
}
void Aes::PrintTableSize() {
  double type_i_tot = 0, type_ii_tot = 0, type_iii_tot = 0, type_iv_tot = 0;
  double type_i_sz, type_ii_sz, type_iii_sz, type_iv_sz;
  int i = 0;
  for (auto r : this->round_) {
    type_i_sz = r.CalcTypeISize();
    type_ii_sz = r.CalcTypeIISize();
    type_iii_sz = r.CalcTypeIIISize();
    type_iv_sz = r.CalcTypeIVSize();
    type_i_tot += type_i_sz;
    type_ii_tot += type_ii_sz;
    type_iii_tot += type_iii_sz;
    type_iv_tot += type_iv_sz;
    // printf("round %d\n", i);
    i++;
    // printf("round table size in bit:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_sz, type_ii_sz, type_iii_sz, type_iv_sz);
    type_i_sz /= 8;
    type_ii_sz /= 8;
    type_iii_sz /= 8;
    type_iv_sz /= 8;
    // printf("round table size in byte:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_sz, type_ii_sz, type_iii_sz, type_iv_sz);
    type_i_sz /= 1024;
    type_ii_sz /= 1024;
    type_iii_sz /= 1024;
    type_iv_sz /= 1024;
    // printf("round table size in kb:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_sz, type_ii_sz, type_iii_sz, type_iv_sz);
    // printf("\n");
  }
  double tot = type_i_tot + type_ii_tot + type_iii_tot + type_iv_tot;
  // printf("round total table size in bit:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_tot, type_ii_tot, type_iii_tot, type_iv_tot);
  type_i_tot /= 8;
  type_ii_tot /= 8;
  type_iii_tot /= 8;
  type_iv_tot /= 8;
  // printf("round total table size in byte:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_tot, type_ii_tot, type_iii_tot, type_iv_tot);
  type_i_tot /= 1024;
  type_ii_tot /= 1024;
  type_iii_tot /= 1024;
  type_iv_tot /= 1024;
  // printf("round total table size in kb:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_tot, type_ii_tot, type_iii_tot, type_iv_tot);
  type_i_tot /= 1024;
  type_ii_tot /= 1024;
  type_iii_tot /= 1024;
  type_iv_tot /= 1024;
  // printf("round total table size in mb:\n TypeI %.2lf, TypeII %.2lf, TypeIII %.2lf, TypeIV %.2lf\n", type_i_tot, type_ii_tot, type_iii_tot, type_iv_tot);
  printf("\ntotal table size:\nin bit %.2lf\nin byte %.2lf\nin kb %.2lf\nin mb %.2lf\n", tot, tot/8, tot/8/1024, tot/8/1024/1024);
  // printf("%.2lf/%.2lf\n", tot/8/1024, tot/8/1024/1024);
}
} // namespace shiy
