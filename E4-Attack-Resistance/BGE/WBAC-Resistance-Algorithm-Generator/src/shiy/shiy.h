#ifndef WBRAG_SRC_SHIY_SHIY_H
#define WBRAG_SRC_SHIY_SHIY_H

#include "../cejo/cejo.h"

namespace shiy {
using State = aes::State;
using BitMatrix = wbmatrix::BitMatrix;
class Round : public cejo::Round {
 public:
  Round() = default;
  Round(const BitMatrix &dm,
        // const std::vector<BitMatrix> &encoding,
        const std::vector<std::vector<uint16_t> > &encoding,
        const std::vector<BitMatrix> &l_inv,
        const bool &nxt_nsnl,
        const bool &cur_pa,
        const int &cur_q);
  Round(// const std::vector<BitMatrix> &id,
        // const std::vector<BitMatrix> &oe,
        const std::vector<std::vector<uint16_t> > &id,
        const std::vector<std::vector<uint16_t> > &oe,
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
        const bool &nxt_nsnl);
  Round(const BitMatrix &dm,
        // const std::vector<BitMatrix> &id,
        const std::vector<std::vector<uint16_t> > &id,
        const std::vector<BitMatrix> &l,
        const std::vector<uint8_t> &k,
        const bool &pre_pa,
        const int &pre_q,
        const bool &cur_nsnl,
        const bool &cur_pa,
        const int &cur_q);
  void Encrypt(State *state);
  uint64_t CalcTypeISize();
  uint64_t CalcTypeIISize();
  uint64_t CalcTypeIIISize();
  uint64_t CalcTypeIVSize();
  void PrintTable(const int &index, FILE* fp);
 private:
  std::vector<uint32_t> type_i_, type_ii_, type_iii_, type_iv_;
  int type_iv_in1_, type_iv_in2_;
  int type_iv_outbit1_, type_iv_outbit2_;
  std::string type_;
  uint64_t type_i_outbit_, type_ii_outbit_, type_iii_outbit_, type_iv_outbit_;
};
class Aes : public cejo::Aes {
 public:
  Aes() = default;
  void InitKey(const std::vector<uint8_t> &k);
  void InitRound();
  void Encrypt(State *state);
  void EnablePartialAdder(const int &round_index, const int &q);
  void EnableNSNL(const int &round_index);
  void PrintTableSize();
  void PrintTable(FILE *fp);
 private:
  std::vector<uint32_t> k_;
  std::vector<bool> pa_, nsnl_;
  std::vector<int> q_;
  std::vector<Round> round_;
};
} // namespace shiy
#endif