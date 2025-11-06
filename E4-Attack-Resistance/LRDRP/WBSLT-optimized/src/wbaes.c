#include "wbaes.h"

u32 TypeII[10][16][4096];//Type II
u32 TypeIII[9][16][4096];//Type III
u16 TypeIV_II[9][4][3][8][16][16];
u16 TypeIV_III[9][4][3][8][16][16];
u32 TypeIa[16][4][256];
u32 TypeIb[16][4][4096];

M128 IDM;
M128 ODM;
M128 IDM_inv;
M128 ODM_inv;
u16 TypeIV_Ia[4][8][15][16][16];
u8 TypeIV_Ib[4][8][15][16][16];

void shiftRows16 (u16 state[16]) 
{
  int i;
  u16 out[16];
  int shiftTab[16] = {0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11};
  for (i = 0; i < 16; i++) 
  {
    out[i] = state[shiftTab[i]];
  }
  memcpy(state, out, sizeof(out));
}

void printstate(unsigned char * in)
{
    int i;
    for(i = 0; i < 16; i++)
    {
        printf("%.2X", in[i]);
    }
    printf("\n");
}

void generatePermutation(u8 *permutation, u8 *inverse)
{
	int i, j;
	u8 temp;
	for (i = 0; i < 16; i++)
	{
		permutation[i] = i;
	}
	for (i = 0; i < 15; i++)
	{
		j = cus_random()%(16 - i);
		temp = permutation[i];
		permutation[i] = permutation[i+j];
		permutation[i + j] = temp;
	}
	for (i = 0; i < 16; i++)
	{
		inverse[permutation[i]] = i;
	}
}

void generatePermutation64(u8 *permutation, u8 *inverse)
{
	int i, j;
	u8 temp;
	for (i = 0; i < 64; i++)
	{
		permutation[i] = i;
	}
	for (i = 0; i < 63; i++)
	{
		j = cus_random()%(64 - i);
		temp = permutation[i];
		permutation[i] = permutation[i+j];
		permutation[i + j] = temp;
	}
	for (i = 0; i < 64; i++)
	{
		inverse[permutation[i]] = i;
	}
}

void optimized_wbaes_gen(u8 key[16])
{
    int i, j, x, y, k;
    u8 expandedKey[176];
    expandKey (key, expandedKey);

    genMatpairM128(&IDM, &IDM_inv);
    genMatpairM128(&ODM, &ODM_inv);
    // identityM128(&ODM);
    // identityM128(&ODM_inv);
    
    M8 L[9][16];
    M8 L_inv[9][16];
    M32 MB[9][4];
    M32 MB_inv[9][4];

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 16; j++)
        {
            genMatpairM8(&L[i][j], &L_inv[i][j]);
        }
    }
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 4; j++)
        {
            genMatpairM32(&MB[i][j], &MB_inv[i][j]);
        }
    }

    u32 Tyi[4][256];
    for (x = 0; x < 256; x++)
    {
        Tyi[0][x] = (gMul(2, x) << 24) | (x << 16) | (x << 8) | gMul(3, x);
        Tyi[1][x] = (gMul(3, x) << 24) | (gMul(2, x) << 16) | (x << 8) | x;
        Tyi[2][x] = (x << 24) | (gMul(3, x) << 16) | (gMul(2, x) << 8) | x;
        Tyi[3][x] = (x << 24) | (x << 16) | (gMul(3, x) << 8) | gMul(2, x);
    }

    M32 Out_L[9][4];
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 4; j++)
        {
            MatrixcomM8to32(L[i][4 * j], L[i][4 * j + 1], L[i][4 * j + 2], L[i][4 * j + 3], &Out_L[i][j]);
        }
    }

    u8 TypeII_out[9][16][8][16];
    u8 TypeII_out_inv[9][16][8][16];
    u8 TypeIV_II_out1[9][8][8][16];
    u8 TypeIV_II_out1_inv[9][8][8][16];
    u8 TypeIV_II_out2[9][4][8][64];
    u8 TypeIV_II_out2_inv[9][4][8][64];

    u8 TypeIII_out[9][16][8][16];
    u8 TypeIII_out_inv[9][16][8][16];
    u8 TypeIV_III_out1[9][8][8][16];
    u8 TypeIV_III_out1_inv[9][8][8][16];
    u8 TypeIV_III_out2[9][4][8][64];
    u8 TypeIV_III_out2_inv[9][4][8][64];

    u8 TypeII_ex_in[16][2][16];
    u8 TypeII_ex_in_inv[16][2][16];
    u8 TypeIII_ex_out[16][2][16];
    u8 TypeIII_ex_out_inv[16][2][16];

    u8 TypeIa_out[16][4][8][16];
    u8 TypeIa_out_inv[16][4][8][16];
    u8 TypeIV_Ia_out1[4][8][14][16];
    u8 TypeIV_Ia_out1_inv[4][8][14][16];
    u8 TypeIV_Ia_out2[4][8][64];
    u8 TypeIV_Ia_out2_inv[4][8][64];

    u8 TypeIb_out[16][4][8][16];
    u8 TypeIb_out_inv[16][4][8][16];
    u8 TypeIV_Ib_out1[4][8][14][16];
    u8 TypeIV_Ib_out1_inv[4][8][14][16];

    InitRandom((unsigned int)time(NULL));
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 16; j++)
        {
            for(k = 0; k < 8; k++)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for(x = 0; x < 16; x++)
                {
                    TypeII_out[i][j][k][x] = permutation[x];
                    TypeII_out_inv[i][j][k][x] = inverse[x];
                }
                generatePermutation(permutation, inverse);
                for(x = 0; x < 16; x++)
                {
                    TypeIII_out[i][j][k][x] = permutation[x];
                    TypeIII_out_inv[i][j][k][x] = inverse[x];
                }
            }
        }
    }
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 8; j++)
        {
            for(k = 0; k < 8; k++)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for(x = 0; x < 16; x++)
                {
                    TypeIV_II_out1[i][j][k][x] = permutation[x];
                    TypeIV_II_out1_inv[i][j][k][x] = inverse[x];
                }
                generatePermutation(permutation, inverse);
                for(x = 0; x < 16; x++)
                {
                    TypeIV_III_out1[i][j][k][x] = permutation[x];
                    TypeIV_III_out1_inv[i][j][k][x] = inverse[x];
                }
            }
        }
    }
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 4; j++)
        {
            for(k = 0; k < 8; k++)
            {
                u8 permutation[64];
                u8 inverse[64];
                generatePermutation64(permutation, inverse);
                for(x = 0; x < 64; x++)
                {
                    TypeIV_II_out2[i][j][k][x] = permutation[x];
                    TypeIV_II_out2_inv[i][j][k][x] = inverse[x];
                }
                generatePermutation64(permutation, inverse);
                for(x = 0; x < 64; x++)
                {
                    TypeIV_III_out2[i][j][k][x] = permutation[x];
                    TypeIV_III_out2_inv[i][j][k][x] = inverse[x];
                }
            }
        }
    }

    for(i = 0; i < 16; i++)
    {
        for(j = 0; j < 2; j++)
        {
            u8 permutation[16];
            u8 inverse[16];
            generatePermutation(permutation, inverse);
            for(x = 0; x < 16; x++)
            {
                TypeII_ex_in[i][j][x] = permutation[x];
                TypeII_ex_in_inv[i][j][x] = inverse[x];
            }
            generatePermutation(permutation, inverse);
            for(x = 0; x < 16; x++)
            {
                TypeIII_ex_out[i][j][x] = permutation[x];
                TypeIII_ex_out_inv[i][j][x] = inverse[x];
            }
        }
    }
    for (i = 0; i < 16; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            for (k = 0; k < 8; ++k)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for (x = 0; x < 16; ++x)
                {
                    TypeIa_out[i][j][k][x] = permutation[x];
                    TypeIa_out_inv[i][j][k][x] = inverse[x];
                }
            }
        }
    }
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 14; ++k)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for (x = 0; x < 16; ++x)
                {
                    TypeIV_Ia_out1[i][j][k][x] = permutation[x];
                    TypeIV_Ia_out1_inv[i][j][k][x] = inverse[x];
                }
            }
        }
    }
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            u8 permutation[64];
            u8 inverse[64];
            generatePermutation64(permutation, inverse);
            for (x = 0; x < 64; ++x)
            {
                TypeIV_Ia_out2[i][j][x] = permutation[x];
                TypeIV_Ia_out2_inv[i][j][x] = inverse[x];
            }
        }
    }

    for (i = 0; i < 16; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            for (k = 0; k < 8; ++k)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for (x = 0; x < 16; ++x)
                {
                    TypeIb_out[i][j][k][x] = permutation[x];
                    TypeIb_out_inv[i][j][k][x] = inverse[x];
                    // TypeIb_out[i][j][k][x] = x;
                    // TypeIb_out_inv[i][j][k][x] = x;
                }
            }
        }
    }
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 14; ++k)
            {
                u8 permutation[16];
                u8 inverse[16];
                generatePermutation(permutation, inverse);
                for (x = 0; x < 16; ++x)
                {
                    TypeIV_Ib_out1[i][j][k][x] = permutation[x];
                    TypeIV_Ib_out1_inv[i][j][k][x] = inverse[x];
                    // TypeIV_Ib_out1[i][j][k][x] = x;
                    // TypeIV_Ib_out1_inv[i][j][k][x] = x;
                }
            }
        }
    }
    
    int columnindex[]={0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
    int shiftindex[]={0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11};
    //IDM
    for (j = 0; j < 16; ++j)
    {
        V128 v128;
        for (x = 0; x < 256; ++x)
        {
            initV128(&v128);
            v128.V[j / 8] = (u64)x << ((j % 8) * 8);
            MatMulVecM128(IDM_inv, v128, &v128);
            TypeIa[j][0][x] = TypeIa_out[j][0][0][v128.V[0] & 0xf] | (TypeIa_out[j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIa_out[j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIa_out[j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIa_out[j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeIa[j][1][x] = TypeIa_out[j][1][0][v128.V[0] & 0xf] | (TypeIa_out[j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIa_out[j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIa_out[j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIa_out[j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeIa[j][2][x] = TypeIa_out[j][2][0][v128.V[1] & 0xf] | (TypeIa_out[j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIa_out[j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIa_out[j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIa_out[j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeIa[j][3][x] = TypeIa_out[j][3][0][v128.V[1] & 0xf] | (TypeIa_out[j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIa_out[j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIa_out[j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIa_out[j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
        }
    }
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 8; ++k)
            {
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ia[i][j][k][x][y] = TypeIV_Ia_out1[i][j][k][TypeIa_out_inv[k * 2][i][j][x] ^ TypeIa_out_inv[k * 2 + 1][i][j][y]];
                        // TypeIV_Ia[i][j][1][x][y] = TypeIV_Ia_out1[i][j][0][TypeIa_out_inv[0][i][j][x] ^ TypeIa_out_inv[1][i][j][y]];
                    }
                }
            }
            for (k = 8; k < 12; ++k)
            {
                int tmp = k % 8;
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ia[i][j][k][x][y] = TypeIV_Ia_out1[i][j][k][TypeIV_Ia_out1_inv[i][j][tmp * 2][x] ^ TypeIV_Ia_out1_inv[i][j][tmp * 2 + 1][y]];
                    }
                }
            }
            for (x = 0; x < 16; ++x)
            {
                for (y = 0; y < 16; ++y)
                {
                    TypeIV_Ia[i][j][12][x][y] = TypeIV_Ia_out1[i][j][12][TypeIV_Ia_out1_inv[i][j][8][x] ^ TypeIV_Ia_out1_inv[i][j][9][y]];
                    TypeIV_Ia[i][j][13][x][y] = TypeIV_Ia_out1[i][j][13][TypeIV_Ia_out1_inv[i][j][10][x] ^ TypeIV_Ia_out1_inv[i][j][11][y]];
                    u8 px, py;
                    px = TypeIV_Ia_out1_inv[i][j][12][x];
                    py = TypeIV_Ia_out1_inv[i][j][13][y];
                    TypeIV_Ia[i][j][14][x][y] = TypeIV_Ia_out2[i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                }
            }
        }
    }
    //Round 1
    shiftRows (expandedKey + 16 * 0);
    for(j = 0; j < 16; j++)//type_II
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            // temp_u8 = (TypeII_ex_in_inv[shiftindex[j]][0][(x & 0xf0) >> 4] << 4) | (TypeII_ex_in_inv[shiftindex[j]][1][(x & 0x0f)]);
            // temp_u8 = MatMulNumM8(ex_in_inv[shiftindex[j]], temp_u8);
            // temp_u8 = x;
            int tmp = j;
            tmp = shiftindex[tmp];
            lx = TypeIV_Ia_out2_inv[tmp / 4][(tmp % 4) * 2][(x & 0xfc0) >> 6];
            rx = TypeIV_Ia_out2_inv[tmp / 4][(tmp % 4) * 2 + 1][(x & 0x03f)];
            // rx = TypeIV_Ia_out2_inv[tmp / 4][(tmp % 4) * 2][(x & 0xfc0) >> 6];
            // lx = TypeIV_Ia_out2_inv[tmp / 4][(tmp % 4) * 2 + 1][(x & 0x03f)];
            // lx = (x >> 6) & 0x3f, rx = x & 0x3f;
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = SBox[temp_u8 ^ expandedKey[16 * 0 + j]];
            temp_u32 = Tyi[j % 4][temp_u8];
            temp_u32 = MatMulNumM32(MB[0][columnindex[j]], temp_u32);
            TypeII[0][j][x] = (TypeII_out[0][j][0][(temp_u32 & 0xf0000000) >> 28] << 28) | (TypeII_out[0][j][1][(temp_u32 & 0x0f000000) >> 24] << 24) | (TypeII_out[0][j][2][(temp_u32 & 0x00f00000) >> 20] << 20) | (TypeII_out[0][j][3][(temp_u32 & 0x000f0000) >> 16] << 16) | (TypeII_out[0][j][4][(temp_u32 & 0x0000f000) >> 12] << 12) | (TypeII_out[0][j][5][(temp_u32 & 0x00000f00) >> 8] << 8) | (TypeII_out[0][j][6][(temp_u32 & 0x000000f0) >> 4] << 4) | (TypeII_out[0][j][7][(temp_u32 & 0x0000000f)]);
        }
    }
    for(j = 0; j < 16; j++)//type_III
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        int shiftbit[]={24, 16, 8, 0};
        for(x = 0; x < 4096; x++)
        {
            // lx = (x >> 6) & 0x3f, rx = x & 0x3f;
            lx = TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2][(x & 0xfc0) >> 6];
            rx = TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2 + 1][(x & 0x03f)];
            // rx = TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2][(x & 0xfc0) >> 6];
            // lx = TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2 + 1][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            // temp_u8 = (TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2][(temp_u8 & 0xf0) >> 4] << 4) | (TypeIV_II_out2_inv[0][columnindex[j]][(j % 4) * 2 + 1][(temp_u8 & 0x0f)]); 
            temp_u32 = temp_u8 << shiftbit[j % 4];
            temp_u32 = MatMulNumM32(MB_inv[0][columnindex[j]], temp_u32);
            temp_u32 = MatMulNumM32(Out_L[0][columnindex[j]], temp_u32);
            TypeIII[0][j][x] = (TypeIII_out[0][j][0][(temp_u32 & 0xf0000000) >> 28] << 28) | (TypeIII_out[0][j][1][(temp_u32 & 0x0f000000) >> 24] << 24) | (TypeIII_out[0][j][2][(temp_u32 & 0x00f00000) >> 20] << 20) | (TypeIII_out[0][j][3][(temp_u32 & 0x000f0000) >> 16] << 16) | (TypeIII_out[0][j][4][(temp_u32 & 0x0000f000) >> 12] << 12) | (TypeIII_out[0][j][5][(temp_u32 & 0x00000f00) >> 8] << 8) | (TypeIII_out[0][j][6][(temp_u32 & 0x000000f0) >> 4] << 4) | (TypeIII_out[0][j][7][(temp_u32 & 0x0000000f)]);
        }
    }

    //Round 2-9
    for (i = 1; i < 9; i++)//Type_II
    {
        shiftRows (expandedKey + 16 * i);
        for(j = 0; j < 16; j++)
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            for(x = 0; x < 4096; x++)
            {
                // lx = (x >> 6) & 0x3f, rx = x & 0x3f;
                lx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
                rx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
                // rx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
                // lx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
                // temp_u8 = (TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(temp_u8 & 0xf0) >> 4] << 4) | (TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(temp_u8 & 0x0f)]);
                temp_u8 = MatMulNumM8(L_inv[i - 1][shiftindex[j]], temp_u8);
                temp_u8 = SBox[temp_u8 ^ expandedKey[16 * i + j]];
                temp_u32 = Tyi[j % 4][temp_u8];
                temp_u32 = MatMulNumM32(MB[i][columnindex[j]], temp_u32);
                TypeII[i][j][x] = (TypeII_out[i][j][0][(temp_u32 & 0xf0000000) >> 28] << 28) | (TypeII_out[i][j][1][(temp_u32 & 0x0f000000) >> 24] << 24) | (TypeII_out[i][j][2][(temp_u32 & 0x00f00000) >> 20] << 20) | (TypeII_out[i][j][3][(temp_u32 & 0x000f0000) >> 16] << 16) | (TypeII_out[i][j][4][(temp_u32 & 0x0000f000) >> 12] << 12) | (TypeII_out[i][j][5][(temp_u32 & 0x00000f00) >> 8] << 8) | (TypeII_out[i][j][6][(temp_u32 & 0x000000f0) >> 4] << 4) | (TypeII_out[i][j][7][(temp_u32 & 0x0000000f)]);
            }
        }
    
        for(j = 0; j < 16; j++)//type_III
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            int shiftbit[]={24, 16, 8, 0};
            for(x = 0; x < 4096; x++)
            {
                // lx = (x >> 6) & 0x3f, rx = x & 0x3f;
                lx = TypeIV_II_out2_inv[i][columnindex[j]][(j % 4) * 2][(x & 0xfc0) >> 6];
                rx = TypeIV_II_out2_inv[i][columnindex[j]][(j % 4) * 2 + 1][(x & 0x03f)];
                // rx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
                // lx = TypeIV_III_out2_inv[i - 1][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
                // temp_u8 = (TypeIV_II_out2_inv[i][columnindex[j]][(j % 4) * 2][(temp_u8 & 0xf0) >> 4] << 4) | (TypeIV_II_out2_inv[i][columnindex[j]][(j % 4) * 2 + 1][(temp_u8 & 0x0f)]);
                temp_u32 = temp_u8 << shiftbit[j % 4];
                temp_u32 = MatMulNumM32(MB_inv[i][columnindex[j]], temp_u32);
                temp_u32 = MatMulNumM32(Out_L[i][columnindex[j]], temp_u32);
                TypeIII[i][j][x] = (TypeIII_out[i][j][0][(temp_u32 & 0xf0000000) >> 28] << 28) | (TypeIII_out[i][j][1][(temp_u32 & 0x0f000000) >> 24] << 24) | (TypeIII_out[i][j][2][(temp_u32 & 0x00f00000) >> 20] << 20) | (TypeIII_out[i][j][3][(temp_u32 & 0x000f0000) >> 16] << 16) | (TypeIII_out[i][j][4][(temp_u32 & 0x0000f000) >> 12] << 12) | (TypeIII_out[i][j][5][(temp_u32 & 0x00000f00) >> 8] << 8) | (TypeIII_out[i][j][6][(temp_u32 & 0x000000f0) >> 4] << 4) | (TypeIII_out[i][j][7][(temp_u32 & 0x0000000f)]);
            }
        }
    }

    //Round 10
    // shiftRows (expandedKey + 16 * 9);
    // for(j = 0; j < 16; j++)//type_II
    // {
    //     u8 temp_u8;
    //     u8 lx, rx;
    //     for(x = 0; x < 4096; x++)
    //     {
    //         // lx = (x >> 6) & 0x3f, rx = x & 0x3f;
    //         lx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
    //         rx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
    //         temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
    //         // temp_u8 = (TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(temp_u8 & 0xf0) >> 4] << 4) | (TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(temp_u8 & 0x0f)]);
    //         temp_u8 = MatMulNumM8(L_inv[8][shiftindex[j]], temp_u8);
    //         temp_u8 = SBox[temp_u8 ^ expandedKey[16 * 9 + j]] ^ expandedKey[16 * 10 + j];
    //         // temp_u8 = MatMulNumM8(ex_out[j], temp_u8);
    //         // TypeII[9][j][x] = (TypeIII_ex_out[j][0][(temp_u8 & 0xf0) >> 4] << 4) | (TypeIII_ex_out[j][1][(temp_u8 & 0x0f)]);
    //         TypeII[9][j][x] = temp_u8;
    //     }
    // }

    // ODM
    shiftRows (expandedKey + 16 * 9);
    for(j = 0; j < 16; j++)//type_II
    {
        u8 temp_u8;
        u8 lx, rx;
        V128 v128;
        for(x = 0; x < 4096; x++)
        {
            initV128(&v128);
            lx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
            rx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
            // rx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2][(x & 0xfc0) >> 6];
            // lx = TypeIV_III_out2_inv[8][columnindex[shiftindex[j]]][(shiftindex[j] % 4) * 2 + 1][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = MatMulNumM8(L_inv[8][shiftindex[j]], temp_u8);
            temp_u8 = SBox[temp_u8 ^ expandedKey[16 * 9 + j]] ^ expandedKey[16 * 10 + j];
            // TypeII[9][j][x] = temp_u8;
            v128.V[j / 8] = (u64)temp_u8 << ((j % 8) * 8);
            MatMulVecM128(ODM_inv, v128, &v128);
            TypeIb[j][0][x] = TypeIb_out[j][0][0][v128.V[0] & 0xf] | (TypeIb_out[j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIb_out[j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIb_out[j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIb_out[j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeIb[j][1][x] = TypeIb_out[j][1][0][v128.V[0] & 0xf] | (TypeIb_out[j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIb_out[j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIb_out[j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIb_out[j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeIb[j][2][x] = TypeIb_out[j][2][0][v128.V[1] & 0xf] | (TypeIb_out[j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIb_out[j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIb_out[j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIb_out[j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeIb[j][3][x] = TypeIb_out[j][3][0][v128.V[1] & 0xf] | (TypeIb_out[j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIb_out[j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIb_out[j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIb_out[j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
        }
    }
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 8; ++k)
            {
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ib[i][j][k][x][y] = TypeIV_Ib_out1[i][j][k][TypeIb_out_inv[k * 2][i][j][x] ^ TypeIb_out_inv[k * 2 + 1][i][j][y]];
                    }
                }
            }
            for (k = 8; k < 12; ++k)
            {
                int tmp = k % 8;
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ib[i][j][k][x][y] = TypeIV_Ib_out1[i][j][k][TypeIV_Ib_out1_inv[i][j][tmp * 2][x] ^ TypeIV_Ib_out1_inv[i][j][tmp * 2 + 1][y]];
                    }
                }
            }
            for (x = 0; x < 16; ++x)
            {
                for (y = 0; y < 16; ++y)
                {
                    TypeIV_Ib[i][j][12][x][y] = TypeIV_Ib_out1[i][j][12][TypeIV_Ib_out1_inv[i][j][8][x] ^ TypeIV_Ib_out1_inv[i][j][9][y]];
                    TypeIV_Ib[i][j][13][x][y] = TypeIV_Ib_out1[i][j][13][TypeIV_Ib_out1_inv[i][j][10][x] ^ TypeIV_Ib_out1_inv[i][j][11][y]];
                    TypeIV_Ib[i][j][14][x][y] = TypeIV_Ib_out1_inv[i][j][12][x] ^ TypeIV_Ib_out1_inv[i][j][13][y];
                }
            }
        }
    }

    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 4; j++)
        {
            for(k = 0; k < 8; k++)
            {
                for(x = 0; x < 16; x++)
                {
                    u8 px, py;
                    for(y = 0; y < 16; y++)
                    {
                        TypeIV_II[i][j][0][k][x][y] = TypeIV_II_out1[i][2 * j][k][TypeII_out_inv[i][4 * j][k][x] ^ TypeII_out_inv[i][4 * j + 1][k][y]];
                        TypeIV_II[i][j][1][k][x][y] = TypeIV_II_out1[i][2 * j + 1][k][TypeII_out_inv[i][4 * j + 2][k][x] ^ TypeII_out_inv[i][4 * j + 3][k][y]];
                        px = TypeIV_II_out1_inv[i][2 * j][k][x];
                        py = TypeIV_II_out1_inv[i][2 * j + 1][k][y];
                        TypeIV_II[i][j][2][k][x][y] = TypeIV_II_out2[i][j][k][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                        // TypeIV_II[i][j][2][k][x][y] = TypeIV_II_out2[i][j][k][TypeIV_II_out1_inv[i][2 * j][k][x] ^ TypeIV_II_out1_inv[i][2 * j + 1][k][y]];

                        TypeIV_III[i][j][0][k][x][y] = TypeIV_III_out1[i][2 * j][k][TypeIII_out_inv[i][4 * j][k][x] ^ TypeIII_out_inv[i][4 * j + 1][k][y]];
                        TypeIV_III[i][j][1][k][x][y] = TypeIV_III_out1[i][2 * j + 1][k][TypeIII_out_inv[i][4 * j + 2][k][x] ^ TypeIII_out_inv[i][4 * j + 3][k][y]];
                        px = TypeIV_III_out1_inv[i][2 * j][k][x];
                        py = TypeIV_III_out1_inv[i][2 * j + 1][k][y];
                        TypeIV_III[i][j][2][k][x][y] = TypeIV_III_out2[i][j][k][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                        // TypeIV_III[i][j][2][k][x][y] = TypeIV_III_out2[i][j][k][TypeIV_III_out1_inv[i][2 * j][k][x] ^ TypeIV_III_out1_inv[i][2 * j + 1][k][y]];
                    }
                }
            }
        }
    }
}

void wbaes_encrypt(u8 input[16], u8 output[16]) 
{
    int i, j;
    u32 a, b, c, d, ab, cd;
    u16 state[16];
    for(i = 0; i < 16; i++)
    {
        state[i] = input[i];
    }
    
    //IDM
    u16 tmp_state[16];
    for (j = 0; j < 16; j++)
    {
        a = j / 4; // column index
        b = j % 4; // row index
        c = b * 2; // 
        d = b * 2 + 1;
        tmp_state[j] = 
        (TypeIV_Ia[a][c][14]
        [
            TypeIV_Ia[a][c][12]
            [
                TypeIV_Ia[a][c][8]
                [
                    TypeIV_Ia[a][c][0][(TypeIa[0][a][state[0]] >> (4 * c)) & 0xf][(TypeIa[1][a][state[1]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][c][1][(TypeIa[2][a][state[2]] >> (4 * c)) & 0xf][(TypeIa[3][a][state[3]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_Ia[a][c][9]
                [
                    TypeIV_Ia[a][c][2][(TypeIa[4][a][state[4]] >> (4 * c)) & 0xf][(TypeIa[5][a][state[5]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][c][3][(TypeIa[6][a][state[6]] >> (4 * c)) & 0xf][(TypeIa[7][a][state[7]] >> (4 * c)) & 0xf]
                ]
            ]
        ]
        [
            TypeIV_Ia[a][c][13]
            [
                TypeIV_Ia[a][c][10]
                [
                    TypeIV_Ia[a][c][4][(TypeIa[8][a][state[8]] >> (4 * c)) & 0xf][(TypeIa[9][a][state[9]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][c][5][(TypeIa[10][a][state[10]] >> (4 * c)) & 0xf][(TypeIa[11][a][state[11]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_Ia[a][c][11]
                [
                    TypeIV_Ia[a][c][6][(TypeIa[12][a][state[12]] >> (4 * c)) & 0xf][(TypeIa[13][a][state[13]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][c][7][(TypeIa[14][a][state[14]] >> (4 * c)) & 0xf][(TypeIa[15][a][state[15]] >> (4 * c)) & 0xf]
                ]
            ]
        ] << 6) | 
        TypeIV_Ia[a][d][14]
        [
            TypeIV_Ia[a][d][12]
            [
                TypeIV_Ia[a][d][8]
                [
                    TypeIV_Ia[a][d][0][(TypeIa[0][a][state[0]] >> (4 * d)) & 0xf][(TypeIa[1][a][state[1]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][d][1][(TypeIa[2][a][state[2]] >> (4 * d)) & 0xf][(TypeIa[3][a][state[3]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_Ia[a][d][9]
                [
                    TypeIV_Ia[a][d][2][(TypeIa[4][a][state[4]] >> (4 * d)) & 0xf][(TypeIa[5][a][state[5]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][d][3][(TypeIa[6][a][state[6]] >> (4 * d)) & 0xf][(TypeIa[7][a][state[7]] >> (4 * d)) & 0xf]
                ]
            ]
        ]
        [
            TypeIV_Ia[a][d][13]
            [
                TypeIV_Ia[a][d][10]
                [
                    TypeIV_Ia[a][d][4][(TypeIa[8][a][state[8]] >> (4 * d)) & 0xf][(TypeIa[9][a][state[9]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][d][5][(TypeIa[10][a][state[10]] >> (4 * d)) & 0xf][(TypeIa[11][a][state[11]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_Ia[a][d][11]
                [
                    TypeIV_Ia[a][d][6][(TypeIa[12][a][state[12]] >> (4 * d)) & 0xf][(TypeIa[13][a][state[13]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ia[a][d][7][(TypeIa[14][a][state[14]] >> (4 * d)) & 0xf][(TypeIa[15][a][state[15]] >> (4 * d)) & 0xf]
                ]
            ]
        ];
    }
    for (i = 0; i < 16; ++i)
    {
        state[i] = tmp_state[i];
    }

    // Round 1-9
    for (i = 0; i < 9; i++) 
    {
        shiftRows16 (state);
        for (j = 0; j < 4; j++)
        {
            a = TypeII[i][4*j + 0][state[4*j + 0]];
            b = TypeII[i][4*j + 1][state[4*j + 1]];
            c = TypeII[i][4*j + 2][state[4*j + 2]];
            d = TypeII[i][4*j + 3][state[4*j + 3]];

            ab = (TypeIV_II[i][j][0][0][(a >> 28) & 0xf][(b >> 28) & 0xf] << 28) | (TypeIV_II[i][j][0][1][(a >> 24) & 0xf][(b >> 24) & 0xf] << 24) | (TypeIV_II[i][j][0][2][(a >> 20) & 0xf][(b >> 20) & 0xf] << 20) |(TypeIV_II[i][j][0][3][(a >> 16) & 0xf][(b >> 16) & 0xf] << 16) |\
            (TypeIV_II[i][j][0][4][(a >> 12) & 0xf][(b >> 12) & 0xf] << 12) | (TypeIV_II[i][j][0][5][(a >> 8) & 0xf][(b >> 8) & 0xf] << 8) | (TypeIV_II[i][j][0][6][(a >> 4) & 0xf][(b >> 4) & 0xf] << 4) | TypeIV_II[i][j][0][7][a & 0xf][b & 0xf];
            
            cd = (TypeIV_II[i][j][1][0][(c >> 28) & 0xf][(d >> 28) & 0xf] << 28) | (TypeIV_II[i][j][1][1][(c >> 24) & 0xf][(d >> 24) & 0xf] << 24) | (TypeIV_II[i][j][1][2][(c >> 20) & 0xf][(d >> 20) & 0xf] << 20) |(TypeIV_II[i][j][1][3][(c >> 16) & 0xf][(d >> 16) & 0xf] << 16) |\
            (TypeIV_II[i][j][1][4][(c >> 12) & 0xf][(d >> 12) & 0xf] << 12) | (TypeIV_II[i][j][1][5][(c >> 8) & 0xf][(d >> 8) & 0xf] << 8) | (TypeIV_II[i][j][1][6][(c >> 4) & 0xf][(d >> 4) & 0xf] << 4) | TypeIV_II[i][j][1][7][c & 0xf][d & 0xf];

            state[4*j + 0] = (TypeIV_II[i][j][2][0][(ab >> 28) & 0xf][(cd >> 28) & 0xf] << 6) | TypeIV_II[i][j][2][1][(ab >> 24) & 0xf][(cd >> 24) & 0xf];
            state[4*j + 1] = (TypeIV_II[i][j][2][2][(ab >> 20) & 0xf][(cd >> 20) & 0xf] << 6) | TypeIV_II[i][j][2][3][(ab >> 16) & 0xf][(cd >> 16) & 0xf];
            state[4*j + 2] = (TypeIV_II[i][j][2][4][(ab >> 12) & 0xf][(cd >> 12) & 0xf] << 6) | TypeIV_II[i][j][2][5][(ab >> 8) & 0xf][(cd >> 8) & 0xf];
            state[4*j + 3] = (TypeIV_II[i][j][2][6][(ab >> 4) & 0xf][(cd >> 4) & 0xf] << 6) | TypeIV_II[i][j][2][7][ab & 0xf][cd & 0xf];

            a = TypeIII[i][4*j + 0][state[4*j + 0]];
            b = TypeIII[i][4*j + 1][state[4*j + 1]];
            c = TypeIII[i][4*j + 2][state[4*j + 2]];
            d = TypeIII[i][4*j + 3][state[4*j + 3]];

            ab = (TypeIV_III[i][j][0][0][(a >> 28) & 0xf][(b >> 28) & 0xf] << 28) | (TypeIV_III[i][j][0][1][(a >> 24) & 0xf][(b >> 24) & 0xf] << 24) | (TypeIV_III[i][j][0][2][(a >> 20) & 0xf][(b >> 20) & 0xf] << 20) |(TypeIV_III[i][j][0][3][(a >> 16) & 0xf][(b >> 16) & 0xf] << 16) |\
            (TypeIV_III[i][j][0][4][(a >> 12) & 0xf][(b >> 12) & 0xf] << 12) | (TypeIV_III[i][j][0][5][(a >> 8) & 0xf][(b >> 8) & 0xf] << 8) | (TypeIV_III[i][j][0][6][(a >> 4) & 0xf][(b >> 4) & 0xf] << 4) | TypeIV_III[i][j][0][7][a & 0xf][b & 0xf];
            
            cd = (TypeIV_III[i][j][1][0][(c >> 28) & 0xf][(d >> 28) & 0xf] << 28) | (TypeIV_III[i][j][1][1][(c >> 24) & 0xf][(d >> 24) & 0xf] << 24) | (TypeIV_III[i][j][1][2][(c >> 20) & 0xf][(d >> 20) & 0xf] << 20) |(TypeIV_III[i][j][1][3][(c >> 16) & 0xf][(d >> 16) & 0xf] << 16) |\
            (TypeIV_III[i][j][1][4][(c >> 12) & 0xf][(d >> 12) & 0xf] << 12) | (TypeIV_III[i][j][1][5][(c >> 8) & 0xf][(d >> 8) & 0xf] << 8) | (TypeIV_III[i][j][1][6][(c >> 4) & 0xf][(d >> 4) & 0xf] << 4) | TypeIV_III[i][j][1][7][c & 0xf][d & 0xf];
            
            state[4*j + 0] = (TypeIV_III[i][j][2][0][(ab >> 28) & 0xf][(cd >> 28) & 0xf] << 6) | TypeIV_III[i][j][2][1][(ab >> 24) & 0xf][(cd >> 24) & 0xf];
            state[4*j + 1] = (TypeIV_III[i][j][2][2][(ab >> 20) & 0xf][(cd >> 20) & 0xf] << 6) | TypeIV_III[i][j][2][3][(ab >> 16) & 0xf][(cd >> 16) & 0xf];
            state[4*j + 2] = (TypeIV_III[i][j][2][4][(ab >> 12) & 0xf][(cd >> 12) & 0xf] << 6) | TypeIV_III[i][j][2][5][(ab >> 8) & 0xf][(cd >> 8) & 0xf];
            state[4*j + 3] = (TypeIV_III[i][j][2][6][(ab >> 4) & 0xf][(cd >> 4) & 0xf] << 6) | TypeIV_III[i][j][2][7][ab & 0xf][cd & 0xf];
        }
    }
    
    //Round 10
    shiftRows16(state);
    for (j = 0; j < 16; j++)
    {
        a = j / 4; // column index
        b = j % 4; // row index
        c = b * 2; // 
        d = b * 2 + 1;
        output[j] = 
        (TypeIV_Ib[a][c][14]
        [
            TypeIV_Ib[a][c][12]
            [
                TypeIV_Ib[a][c][8]
                [
                    TypeIV_Ib[a][c][0][(TypeIb[0][a][state[0]] >> (4 * c)) & 0xf][(TypeIb[1][a][state[1]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][c][1][(TypeIb[2][a][state[2]] >> (4 * c)) & 0xf][(TypeIb[3][a][state[3]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_Ib[a][c][9]
                [
                    TypeIV_Ib[a][c][2][(TypeIb[4][a][state[4]] >> (4 * c)) & 0xf][(TypeIb[5][a][state[5]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][c][3][(TypeIb[6][a][state[6]] >> (4 * c)) & 0xf][(TypeIb[7][a][state[7]] >> (4 * c)) & 0xf]
                ]
            ]
        ]
        [
            TypeIV_Ib[a][c][13]
            [
                TypeIV_Ib[a][c][10]
                [
                    TypeIV_Ib[a][c][4][(TypeIb[8][a][state[8]] >> (4 * c)) & 0xf][(TypeIb[9][a][state[9]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][c][5][(TypeIb[10][a][state[10]] >> (4 * c)) & 0xf][(TypeIb[11][a][state[11]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_Ib[a][c][11]
                [
                    TypeIV_Ib[a][c][6][(TypeIb[12][a][state[12]] >> (4 * c)) & 0xf][(TypeIb[13][a][state[13]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][c][7][(TypeIb[14][a][state[14]] >> (4 * c)) & 0xf][(TypeIb[15][a][state[15]] >> (4 * c)) & 0xf]
                ]
            ]
        ]) | 
        (TypeIV_Ib[a][d][14]
        [
            TypeIV_Ib[a][d][12]
            [
                TypeIV_Ib[a][d][8]
                [
                    TypeIV_Ib[a][d][0][(TypeIb[0][a][state[0]] >> (4 * d)) & 0xf][(TypeIb[1][a][state[1]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][d][1][(TypeIb[2][a][state[2]] >> (4 * d)) & 0xf][(TypeIb[3][a][state[3]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_Ib[a][d][9]
                [
                    TypeIV_Ib[a][d][2][(TypeIb[4][a][state[4]] >> (4 * d)) & 0xf][(TypeIb[5][a][state[5]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][d][3][(TypeIb[6][a][state[6]] >> (4 * d)) & 0xf][(TypeIb[7][a][state[7]] >> (4 * d)) & 0xf]
                ]
            ]
        ]
        [
            TypeIV_Ib[a][d][13]
            [
                TypeIV_Ib[a][d][10]
                [
                    TypeIV_Ib[a][d][4][(TypeIb[8][a][state[8]] >> (4 * d)) & 0xf][(TypeIb[9][a][state[9]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][d][5][(TypeIb[10][a][state[10]] >> (4 * d)) & 0xf][(TypeIb[11][a][state[11]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_Ib[a][d][11]
                [
                    TypeIV_Ib[a][d][6][(TypeIb[12][a][state[12]] >> (4 * d)) & 0xf][(TypeIb[13][a][state[13]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_Ib[a][d][7][(TypeIb[14][a][state[14]] >> (4 * d)) & 0xf][(TypeIb[15][a][state[15]] >> (4 * d)) & 0xf]
                ]
            ]
        ] << 4);
    }
}