#include "aes.h"
#include <stdlib.h>

namespace aes {
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define v8 std::vector<uint8_t>
#define v16 std::vector<uint16_t>
#define v32 std::vector<uint32_t>
#define vr std::vector<Round>
#define U32_TO_U8(a,r) (((a) >> ((3-(r)) * 8)) & 0xff)
#define U8_TO_U32(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

State::State() {
  for (int i = 0; i < 16; ++i) {
    this->state_[i] = 0;
  }
  this->SetBitsPerState(8);
}
State::State(State *state) {
  for (int i = 0; i < 16; ++i) {
    this->state_[i] = state->GetState(i);
  }
  this->SetBitsPerState(state->GetBitsPerState());
}
State::State(v16 v) {
  for (int i = 0; i < 16; ++i) {
    this->state_[i] = v[i];
  }
  u8 max_bit = 8;
  for (auto x : v) {
    while(x > (1 << max_bit) - 1) max_bit++;
  }
  this->SetBitsPerState(max_bit);
}
u16 State::operator[] (const u8 &index) {
  return this->state_[index];
}
u16 State::GetState(const u8 &index) {
  return this->state_[index];
}
u16 State::GetState(const u8 &index_x, const u8 &index_y) {
  return this->state_[index_x + index_y * 4];
}
void State::SetState(uint8_t index, uint16_t value) {

  this->state_[index] = value;
}
void State::SetState(uint8_t index_x, uint8_t index_y, uint16_t value) {
  this->state_[index_x + index_y * 4] = value;
}
void State::SetBitsPerState(const u8 &bits) {
  this->bits_per_state_ = bits;
}
u8 State::GetBitsPerState() {
  return this->bits_per_state_;
}
void State::Print() {
  for (int x = 0; x < 4; ++x) {
    for (int y = 0; y < 4; ++y) {
      printf("%.4x ", this->GetState(x, y));
    }
    printf("\n");
  }
}

u32 Aes::RotWord(u32 wd) {
  u32 temp = wd;
  wd <<= 8;
  wd |= temp >> 24;
  return wd;
}
u32 Aes::SubWord(u32 wd) {
  u8 temp[4];
  for (int i = 0; i < 4; ++i) {
    temp[i] = U32_TO_U8(wd, i);
    temp[i] = AesBase::kSbox[temp[i]];
  }
  return U8_TO_U32(temp[0],temp[1],temp[2],temp[3]);
}
void Aes::KeySchedule(const v8 &k, const u32 &key_length, v32 *w) {
  this->Nb_ =  4;
  this->Nk_ = key_length / 32;
  this->Nr_ = 6 + this->Nk_;
  w->resize((this->Nr_ + 1) * this->Nb_);
  u32 temp;
  int i = 0;
  u32 tmp_k[4];
  while (i < this->Nk_) {
    for (int j = 0; j < 4; ++j) {
      tmp_k[j] = static_cast<u32>(k[4*i + j]);
    }
    (*w)[i] = U8_TO_U32(tmp_k[0],tmp_k[1],tmp_k[2],tmp_k[3]);
    i++;
  }
  i = this->Nk_;
  while (i < this->Nb_ * (this->Nr_ + 1)) {
    temp = (*w)[i - 1];
    if (i % this->Nk_ == 0) {
      temp = this->SubWord(this->RotWord(temp)) ^ AesBase::kRcon[i / this->Nk_];
    } else if ((this->Nk_ > 6) && (i % this->Nk_ == 4)) {
      temp = this->SubWord(temp);
    }
    (*w)[i] = (*w)[i - this->Nk_] ^ temp;
    i++;
  }
}
Aes::Aes(const v8 &k) {
  v32 *w = new v32();
  u32 key_length = k.size() * 8;
  this->KeySchedule(k, key_length, w);
  // for (int i = 0; i < w->size(); ++i) {
  //   printf("%.8x\n", (*w)[i]);
  // }
  for (int i = 0; i < w->size(); i += this->Nk_) {
    if (i + 8 == w->size()) {
      this->round_.push_back(Round(v32(w->begin() + i, w->begin() + i + 8), true));
      break;
    } else {
      this->round_.push_back(Round(v32(w->begin() + i, w->begin() + i + 4), false));
    }
  }
}
void Aes::Encrypt(State *state) {
  auto it = this->round_.begin();
  while (it != this->round_.end()) {
    it->Encrypt(state);
    it++;
  }
}
void Aes::Decrypt(State *state) {
  auto it = this->round_.rbegin();
  while (it != this->round_.rend()) {
    it->Decrypt(state);
    it++;
  }
}

Round::Round(const v32 &w, const bool &is_final) {
  if (!is_final) {
    std::vector<void*> v_addroundkey, v_subbytes, v_shiftrows, v_mixcolumns;
    v_addroundkey.push_back(NULL);
    v_subbytes.push_back(NULL);
    v_shiftrows.push_back(NULL);
    v_mixcolumns.push_back(NULL);
    v32 *tmp_w = new v32();
    for (int i = 0; i < w.size(); ++i) {
      tmp_w->push_back(w[i]);
    }
    v_addroundkey.push_back(tmp_w);
    // encryption
    this->enc_param_vector_.push_back(v_addroundkey);
    this->enc_param_vector_.push_back(v_subbytes);
    this->enc_param_vector_.push_back(v_shiftrows);
    this->enc_param_vector_.push_back(v_mixcolumns);
    this->enc_func_vector_.push_back(Round::AddRoundKey);
    this->enc_func_vector_.push_back(Round::SubBytes);
    this->enc_func_vector_.push_back(Round::ShiftRows);
    this->enc_func_vector_.push_back(Round::MixColumns);
    // decryption
    this->dec_param_vector_.push_back(v_mixcolumns);
    this->dec_param_vector_.push_back(v_shiftrows);
    this->dec_param_vector_.push_back(v_subbytes);
    this->dec_param_vector_.push_back(v_addroundkey);
    this->dec_func_vector_.push_back(Round::InvMixColumns);
    this->dec_func_vector_.push_back(Round::InvShiftRows);
    this->dec_func_vector_.push_back(Round::InvSubBytes);
    this->dec_func_vector_.push_back(Round::InvAddRoundKey);
    
  } else {
    std::vector<void*> v_addroundkey1, v_subbytes, v_shiftrows, v_addroundkey2;
    v_addroundkey1.push_back(NULL);
    v_subbytes.push_back(NULL);
    v_shiftrows.push_back(NULL);
    v_addroundkey2.push_back(NULL);
    v32 *tmp_w1 = new v32(), *tmp_w2 = new v32();
    for (int i = 0; i < 4; ++i) {
      tmp_w1->push_back(w[i]);
    }
    for (int i = 4; i < 8; ++i) {
      tmp_w2->push_back(w[i]);
    }
    v_addroundkey1.push_back(tmp_w1);
    v_addroundkey2.push_back(tmp_w2);
    // encryption
    this->enc_param_vector_.push_back(v_addroundkey1);
    this->enc_param_vector_.push_back(v_subbytes);
    this->enc_param_vector_.push_back(v_shiftrows);
    this->enc_param_vector_.push_back(v_addroundkey2);
    this->enc_func_vector_.push_back(Round::AddRoundKey);
    this->enc_func_vector_.push_back(Round::SubBytes);
    this->enc_func_vector_.push_back(Round::ShiftRows);
    this->enc_func_vector_.push_back(Round::AddRoundKey);
    // decryption
    this->dec_param_vector_.push_back(v_addroundkey2);
    this->dec_param_vector_.push_back(v_shiftrows);
    this->dec_param_vector_.push_back(v_subbytes);
    this->dec_param_vector_.push_back(v_addroundkey1);
    this->dec_func_vector_.push_back(Round::InvAddRoundKey);
    this->dec_func_vector_.push_back(Round::InvShiftRows);
    this->dec_func_vector_.push_back(Round::InvSubBytes);
    this->dec_func_vector_.push_back(Round::InvAddRoundKey);
  }
}
void Round::AddRoundKey(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  v32 *w = static_cast<v32*>(param[1]);
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, state->GetState(i) ^ U32_TO_U8((*w)[i / 4], i % 4));
  }
}
void Round::SubBytes(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, AesBase::kSbox[state->GetState(i)]);
  }
}
void Round::ShiftRows(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  v16 temp(16);
  for (int i = 0; i < 16; ++i) {
    temp[i] = state->GetState(i);
  }
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, temp[AesBase::kShiftTab[i]]);
  }
}
void Round::MixColumns(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  State temp = *state;
  u8 tmp = 0;
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      tmp = 0;
      for (int t = 0; t < 4; ++t) {
        tmp ^= Gmult(temp.GetState(t, c), AesBase::kMixMat[r][t]);
      }
      state->SetState(r, c, tmp);
    }
  }
}
void Round::InvAddRoundKey(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  v32 *w = static_cast<v32*>(param[1]);
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, state->GetState(i) ^ U32_TO_U8((*w)[i / 4], i % 4));
  }
}
void Round::InvSubBytes(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, AesBase::kInvSbox[state->GetState(i)]);
  }
}
void Round::InvShiftRows(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  v16 temp(16);
  for (int i = 0; i < 16; ++i) {
    temp[i] = state->GetState(i);
  }
  for (int i = 0; i < 16; ++i) {
    state->SetState(i, temp[AesBase::kInvShiftTab[i]]);
  }
}
void Round::InvMixColumns(std::vector<void*> param) {
  State *state = static_cast<State*>(param[0]);
  State temp = *state;
  u8 tmp = 0;
  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 4; ++c) {
      tmp = 0;
      for (int t = 0; t < 4; ++t) {
        tmp ^= Gmult(temp.GetState(t, c), AesBase::kInvMixMat[r][t]);
      }
      state->SetState(r, c, tmp);
    }
  }
}
u8 Round::Gmult(u8 a, u8 b) {
  if (b == 0x01) {
    return a;
  }
  if (b == 0x02) {
    return (a & 0x80) ? ((a << 1) ^ 0x1b) : (a << 1);
  }
  return (b & 0x01) ? (a ^ Round::Gmult(Gmult(a, 2), b >> 1)) : Round::Gmult(Gmult(a, 2), b >> 1);
}
void Round::Encrypt(State *state) {
  for (auto it = this->enc_param_vector_.begin(); it < this->enc_param_vector_.end(); ++it) {
    (*it)[0] = state;
  }
  auto param_vec = this->enc_param_vector_.begin();
  auto func = this->enc_func_vector_.begin();
  while (func != this->enc_func_vector_.end()) {
    (*func)(*param_vec);
    func++;
    param_vec++;
  }
}
void Round::Decrypt(State *state) {
  for (auto it = this->dec_param_vector_.begin(); it < this->dec_param_vector_.end(); ++it) {
    (*it)[0] = state;
  }
  auto param_vec = this->dec_param_vector_.begin();
  auto func = this->dec_func_vector_.begin();
  while (func != this->dec_func_vector_.end()) {
    (*func)(*param_vec);
    func++;
    param_vec++;
  }
}
} // namespace aes