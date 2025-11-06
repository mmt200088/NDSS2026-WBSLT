// This library is a white-box AES implementation of Chow et al's version.

#ifndef WBRAG_SRC_CEJO_CEJO_H
#define WBRAG_SRC_CEJO_CEJO_H

#include "../aes/aes.h"
#include "../../libs/wbmatrix/wbmatrix.h"

namespace cejo {
using State = aes::State;
using BitMatrix = wbmatrix::BitMatrix;
class Round : public aes::Round {
 public:
  Round() = default;
  // Generates IDM round
  // dm: idm matrix, 128*128
  // encoding: output encoding, 4*4 matrix, 32 in total
  // l: mixing bijections protect the input prior to TypeII tables. 8*8, 16 in total.
  // is_idm: not in using, can be any value
  // is_enc: not in using, can be any value
  Round(const BitMatrix &dm,
        const std::vector<BitMatrix> &encoding,
        const std::vector<BitMatrix> &l,
        const bool &is_idm,
        const bool &is_enc);
  // Generates Middle Round
  // id: input decoding, 4*4 matrix, 32 in total
  // oe: output encoding, 4*4 matrix, 32 in total
  // l and l_inv: mixing bijections protect the input prior to TypeII tables, 8*8, 16 in total each
  // mb and mb_inv: mixing bijections protect the input prior to TypeIII tables, 32*32, 4 in total each
  // ty; ty tables
  // k: key in current round
  // is_enc: not in using, can be any value
  Round(const std::vector<BitMatrix> &id,
        const std::vector<BitMatrix> &oe,
        const std::vector<BitMatrix> &l,
        const std::vector<BitMatrix> &l_inv,
        const std::vector<BitMatrix> &mb,
        const std::vector<BitMatrix> &mb_inv,
        const std::vector<std::vector<uint32_t> > &ty,
        const std::vector<uint8_t> &k,
        const bool &is_enc);
  // Generates last round with ODM round
  // parameters is similar
  Round(const BitMatrix &dm,
        const std::vector<BitMatrix> &id,
        const std::vector<BitMatrix> &l,
        const std::vector<uint8_t> &k,
        const bool &is_idm,
        const bool &is_enc);
  void Encrypt(State *state);
 private:
  std::vector<uint8_t> type_i_, type_iv_;
  std::vector<uint32_t> type_ii_, type_iii_;
  // store the current round's type
  std::string type_;
};
class Aes : public aes::Aes {
 public:
  Aes() = default;
  Aes(const std::vector<uint8_t> &k);
  void Encrypt(State *state);
 private:
  std::vector<Round> round_;
};
} // namespace cejo
#endif