#include "slt.h"

M128 slt_m128;
M128 slt_m128_inv;
M64 slt_m64;
M64 slt_m64_inv;

void printState (u8 in[16])
{
  int i;
  for(i=0; i < 4; i++) 
  {
    printf("%.2X %.2X %.2X %.2X\n", in[i], in[i+4], in[i+8], in[i+12]);
  }
  printf("\n");
}

void slt_init()
{
  genMatpairM128(&slt_m128, &slt_m128_inv);
  genMatpairM64(&slt_m64, &slt_m64_inv);
}

void slt_encrypt (u8* input, u8* key, u8* output, int s, int r)
{
  int i, j;

  for (i = 0; i < r; i++) 
  {
    for (j = 0; j < s; ++j)
    {
      input[j] = SBox[input[j] ^ key[j]];
    }
    if (s == 8)
    {
      V64 v64;
      initV64(&v64);
      v64.V = (u64)input[7] | ((u64)input[6] << 8) | ((u64)input[5] << 16) | ((u64)input[4] << 24) | ((u64)input[3] << 32) | ((u64)input[2] << 40) | ((u64)input[1] << 48) | ((u64)input[0] << 56);
      MatMulVecM64(slt_m64, v64, &v64);
      for (int i = 0; i < 8; ++i)
      {
          input[7 - i] = (v64.V >> (8*i)) & 0xff;
      }
    } else
    {
      V128 v128;
      initV128(&v128);
      v128.V[0] = (u64)input[7] | ((u64)input[6] << 8) | ((u64)input[5] << 16) | ((u64)input[4] << 24) | ((u64)input[3] << 32) | ((u64)input[2] << 40) | ((u64)input[1] << 48) | ((u64)input[0] << 56);
      v128.V[1] = (u64)input[15] | ((u64)input[14] << 8) | ((u64)input[13] << 16) | ((u64)input[12] << 24) | ((u64)input[11] << 32) | ((u64)input[10] << 40) | ((u64)input[9] << 48) | ((u64)input[8] << 56);
      MatMulVecM128(slt_m128, v128, &v128);
      for (int i = 0; i < 8; ++i)
      {
          input[7 - i] = (v128.V[0] >> (8*i)) & 0xff;
      }
      for (int i = 0; i < 8; ++i)
      {
          input[7 - i + 8] = (v128.V[1] >> (8*i)) & 0xff;
      }
    }
  }

  for (i = 0; i < s; i++)
    output[i] = input[i];
}
