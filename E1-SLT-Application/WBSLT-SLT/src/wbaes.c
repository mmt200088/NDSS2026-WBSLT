#include "wbaes.h"

// u32 TypeII[9][16][4][4096];
// u32 TypeIII[9][16][4][4096];
// u16 TypeIV_II[9][4][8][15][16][16];
// u16 TypeIV_III[9][4][8][15][16][16];
// u32 TypeIa[16][4][256];
// u32 TypeIb[16][4][4096];
u32**** TypeII;
u32**** TypeIII;
u16****** TypeIV_II;
u16****** TypeIV_III;
u32*** TypeIa;
u32*** TypeIb;

M128 IDM128;
M128 ODM128;
M128 IDM128_inv;
M128 ODM128_inv;

M64 IDM64;
M64 ODM64;
M64 IDM64_inv;
M64 ODM64_inv;

// u16* TypeIV_Ia[4][8][15][16][16];
// u8* TypeIV_Ib[4][8][15][16][16];
u16***** TypeIV_Ia;
u8***** TypeIV_Ib;

void init_table(int R, int block_size)
{
    const int ROUND = R - 1;
    if (block_size == 128)
    {
        // ---------- Allocate TypeII ----------
        TypeII = malloc(ROUND * sizeof(*TypeII));
        for (int i = 0; i < ROUND; i++) {
            TypeII[i] = malloc(16 * sizeof(*TypeII[i]));
            for (int j = 0; j < 16; j++) {
                TypeII[i][j] = malloc(4 * sizeof(*TypeII[i][j]));
                for (int k = 0; k < 4; k++) {
                    TypeII[i][j][k] = malloc(4096 * sizeof(*TypeII[i][j][k]));
                }
            }
        }

        // ---------- Allocate TypeIII ----------
        TypeIII = malloc(ROUND * sizeof(*TypeIII));
        for (int i = 0; i < ROUND; i++) {
            TypeIII[i] = malloc(16 * sizeof(*TypeIII[i]));
            for (int j = 0; j < 16; j++) {
                TypeIII[i][j] = malloc(4 * sizeof(*TypeIII[i][j]));
                for (int k = 0; k < 4; k++) {
                    TypeIII[i][j][k] = malloc(4096 * sizeof(*TypeIII[i][j][k]));
                }
            }
        }

        // ---------- Allocate TypeIV_II ----------
        TypeIV_II = malloc(ROUND * sizeof(*TypeIV_II));
        for (int i = 0; i < ROUND; i++) {
            TypeIV_II[i] = malloc(4 * sizeof(*TypeIV_II[i]));
            for (int j = 0; j < 4; j++) {
                TypeIV_II[i][j] = malloc(8 * sizeof(*TypeIV_II[i][j]));
                for (int k = 0; k < 8; k++) {
                    TypeIV_II[i][j][k] = malloc(15 * sizeof(*TypeIV_II[i][j][k]));
                    for (int l = 0; l < 15; l++) {
                        TypeIV_II[i][j][k][l] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l]));
                        for (int m = 0; m < 16; m++) {
                            TypeIV_II[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l][m]));
                        }
                    }
                }
            }
        }

        // ---------- Allocate TypeIV_III ----------
        TypeIV_III = malloc(ROUND * sizeof(*TypeIV_III));
        for (int i = 0; i < ROUND; i++) {
            TypeIV_III[i] = malloc(4 * sizeof(*TypeIV_III[i]));
            for (int j = 0; j < 4; j++) {
                TypeIV_III[i][j] = malloc(8 * sizeof(*TypeIV_III[i][j]));
                for (int k = 0; k < 8; k++) {
                    TypeIV_III[i][j][k] = malloc(15 * sizeof(*TypeIV_III[i][j][k]));
                    for (int l = 0; l < 15; l++) {
                        TypeIV_III[i][j][k][l] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l]));
                        for (int m = 0; m < 16; m++) {
                            TypeIV_III[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l][m]));
                        }
                    }
                }
            }
        }

        // ---------- Allocate TypeIa ----------
        TypeIa = malloc(16 * sizeof(*TypeIa));
        for (int i = 0; i < 16; i++) {
            TypeIa[i] = malloc(4 * sizeof(*TypeIa[i]));
            for (int j = 0; j < 4; j++) {
                TypeIa[i][j] = malloc(256 * sizeof(*TypeIa[i][j]));
            }
        }

        // ---------- Allocate TypeIb ----------
        TypeIb = malloc(16 * sizeof(*TypeIb));
        for (int i = 0; i < 16; i++) {
            TypeIb[i] = malloc(4 * sizeof(*TypeIb[i]));
            for (int j = 0; j < 4; j++) {
                TypeIb[i][j] = malloc(4096 * sizeof(*TypeIb[i][j]));
            }
        }

        // ---------- Allocate TypeIV_Ia ----------
        TypeIV_Ia = malloc(4 * sizeof(*TypeIV_Ia));
        for (int i = 0; i < 4; i++) {
            TypeIV_Ia[i] = malloc(8 * sizeof(*TypeIV_Ia[i]));
            for (int j = 0; j < 8; j++) {
                TypeIV_Ia[i][j] = malloc(15 * sizeof(*TypeIV_Ia[i][j]));
                for (int k = 0; k < 15; k++) {
                    TypeIV_Ia[i][j][k] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k]));
                    for (int l = 0; l < 16; l++) {
                        TypeIV_Ia[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k][l]));
                    }
                }
            }
        }

        // ---------- Allocate TypeIV_Ib ----------
        TypeIV_Ib = malloc(4 * sizeof(*TypeIV_Ib));
        for (int i = 0; i < 4; i++) {
            TypeIV_Ib[i] = malloc(8 * sizeof(*TypeIV_Ib[i]));
            for (int j = 0; j < 8; j++) {
                TypeIV_Ib[i][j] = malloc(15 * sizeof(*TypeIV_Ib[i][j]));
                for (int k = 0; k < 15; k++) {
                    TypeIV_Ib[i][j][k] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k]));
                    for (int l = 0; l < 16; l++) {
                        TypeIV_Ib[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k][l]));
                    }
                }
            }
        }
    } else if (block_size == 64)
    {
        // ---------- Allocate TypeII ----------
        TypeII = malloc(ROUND * sizeof(*TypeII));
        for (int i = 0; i < ROUND; i++) {
            TypeII[i] = malloc(8 * sizeof(*TypeII[i]));
            for (int j = 0; j < 8; j++) {
                TypeII[i][j] = malloc(2 * sizeof(*TypeII[i][j]));
                for (int k = 0; k < 2; k++) {
                    TypeII[i][j][k] = malloc(4096 * sizeof(*TypeII[i][j][k]));
                }
            }
        }

        // ---------- Allocate TypeIII ----------
        TypeIII = malloc(ROUND * sizeof(*TypeIII));
        for (int i = 0; i < ROUND; i++) {
            TypeIII[i] = malloc(8 * sizeof(*TypeIII[i]));
            for (int j = 0; j < 8; j++) {
                TypeIII[i][j] = malloc(2 * sizeof(*TypeIII[i][j]));
                for (int k = 0; k < 2; k++) {
                    TypeIII[i][j][k] = malloc(4096 * sizeof(*TypeIII[i][j][k]));
                }
            }
        }

        // ---------- Allocate TypeIV_II ----------
        TypeIV_II = malloc(ROUND * sizeof(*TypeIV_II));
        for (int i = 0; i < ROUND; i++) {
            TypeIV_II[i] = malloc(2 * sizeof(*TypeIV_II[i]));
            for (int j = 0; j < 2; j++) {
                TypeIV_II[i][j] = malloc(8 * sizeof(*TypeIV_II[i][j]));
                for (int k = 0; k < 8; k++) {
                    TypeIV_II[i][j][k] = malloc(7 * sizeof(*TypeIV_II[i][j][k]));
                    for (int l = 0; l < 7; l++) {
                        TypeIV_II[i][j][k][l] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l]));
                        for (int m = 0; m < 16; m++) {
                            TypeIV_II[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l][m]));
                        }
                    }
                }
            }
        }

        // ---------- Allocate TypeIV_III ----------
        TypeIV_III = malloc(ROUND * sizeof(*TypeIV_III));
        for (int i = 0; i < ROUND; i++) {
            TypeIV_III[i] = malloc(2 * sizeof(*TypeIV_III[i]));
            for (int j = 0; j < 2; j++) {
                TypeIV_III[i][j] = malloc(8 * sizeof(*TypeIV_III[i][j]));
                for (int k = 0; k < 8; k++) {
                    TypeIV_III[i][j][k] = malloc(7 * sizeof(*TypeIV_III[i][j][k]));
                    for (int l = 0; l < 7; l++) {
                        TypeIV_III[i][j][k][l] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l]));
                        for (int m = 0; m < 16; m++) {
                            TypeIV_III[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l][m]));
                        }
                    }
                }
            }
        }

        // ---------- Allocate TypeIa ----------
        TypeIa = malloc(8 * sizeof(*TypeIa));
        for (int i = 0; i < 8; i++) {
            TypeIa[i] = malloc(2 * sizeof(*TypeIa[i]));
            for (int j = 0; j < 2; j++) {
                TypeIa[i][j] = malloc(256 * sizeof(*TypeIa[i][j]));
            }
        }

        // ---------- Allocate TypeIb ----------
        TypeIb = malloc(8 * sizeof(*TypeIb));
        for (int i = 0; i < 8; i++) {
            TypeIb[i] = malloc(2 * sizeof(*TypeIb[i]));
            for (int j = 0; j < 2; j++) {
                TypeIb[i][j] = malloc(4096 * sizeof(*TypeIb[i][j]));
            }
        }

        // ---------- Allocate TypeIV_Ia ----------
        TypeIV_Ia = malloc(2 * sizeof(*TypeIV_Ia));
        for (int i = 0; i < 2; i++) {
            TypeIV_Ia[i] = malloc(8 * sizeof(*TypeIV_Ia[i]));
            for (int j = 0; j < 8; j++) {
                TypeIV_Ia[i][j] = malloc(7 * sizeof(*TypeIV_Ia[i][j]));
                for (int k = 0; k < 7; k++) {
                    TypeIV_Ia[i][j][k] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k]));
                    for (int l = 0; l < 16; l++) {
                        TypeIV_Ia[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k][l]));
                    }
                }
            }
        }

        // ---------- Allocate TypeIV_Ib ----------
        TypeIV_Ib = malloc(2 * sizeof(*TypeIV_Ib));
        for (int i = 0; i < 2; i++) {
            TypeIV_Ib[i] = malloc(8 * sizeof(*TypeIV_Ib[i]));
            for (int j = 0; j < 8; j++) {
                TypeIV_Ib[i][j] = malloc(7 * sizeof(*TypeIV_Ib[i][j]));
                for (int k = 0; k < 7; k++) {
                    TypeIV_Ib[i][j][k] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k]));
                    for (int l = 0; l < 16; l++) {
                        TypeIV_Ib[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k][l]));
                    }
                }
            }
        }
    }
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

void printstate16(u16 * in)
{
    int i;
    for(i = 0; i < 16; i++)
    {
        printf("%.4X", in[i]);
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

void general_wbslt128_gen(u8 key[16], int R)
{
    int i, j, x, y, k;
    V128 v128;

    const int ROUND = R - 1;
    // init_table(R, 128);
    // // ---------- Allocate TypeII ----------
    // TypeII = malloc(ROUND * sizeof(*TypeII));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeII[i] = malloc(16 * sizeof(*TypeII[i]));
    //     for (int j = 0; j < 16; j++) {
    //         TypeII[i][j] = malloc(4 * sizeof(*TypeII[i][j]));
    //         for (int k = 0; k < 4; k++) {
    //             TypeII[i][j][k] = malloc(4096 * sizeof(*TypeII[i][j][k]));
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIII ----------
    // TypeIII = malloc(ROUND * sizeof(*TypeIII));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIII[i] = malloc(16 * sizeof(*TypeIII[i]));
    //     for (int j = 0; j < 16; j++) {
    //         TypeIII[i][j] = malloc(4 * sizeof(*TypeIII[i][j]));
    //         for (int k = 0; k < 4; k++) {
    //             TypeIII[i][j][k] = malloc(4096 * sizeof(*TypeIII[i][j][k]));
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_II ----------
    // TypeIV_II = malloc(ROUND * sizeof(*TypeIV_II));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIV_II[i] = malloc(4 * sizeof(*TypeIV_II[i]));
    //     for (int j = 0; j < 4; j++) {
    //         TypeIV_II[i][j] = malloc(8 * sizeof(*TypeIV_II[i][j]));
    //         for (int k = 0; k < 8; k++) {
    //             TypeIV_II[i][j][k] = malloc(15 * sizeof(*TypeIV_II[i][j][k]));
    //             for (int l = 0; l < 15; l++) {
    //                 TypeIV_II[i][j][k][l] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l]));
    //                 for (int m = 0; m < 16; m++) {
    //                     TypeIV_II[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l][m]));
    //                 }
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_III ----------
    // TypeIV_III = malloc(ROUND * sizeof(*TypeIV_III));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIV_III[i] = malloc(4 * sizeof(*TypeIV_III[i]));
    //     for (int j = 0; j < 4; j++) {
    //         TypeIV_III[i][j] = malloc(8 * sizeof(*TypeIV_III[i][j]));
    //         for (int k = 0; k < 8; k++) {
    //             TypeIV_III[i][j][k] = malloc(15 * sizeof(*TypeIV_III[i][j][k]));
    //             for (int l = 0; l < 15; l++) {
    //                 TypeIV_III[i][j][k][l] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l]));
    //                 for (int m = 0; m < 16; m++) {
    //                     TypeIV_III[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l][m]));
    //                 }
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIa ----------
    // TypeIa = malloc(16 * sizeof(*TypeIa));
    // for (int i = 0; i < 16; i++) {
    //     TypeIa[i] = malloc(4 * sizeof(*TypeIa[i]));
    //     for (int j = 0; j < 4; j++) {
    //         TypeIa[i][j] = malloc(256 * sizeof(*TypeIa[i][j]));
    //     }
    // }

    // // ---------- Allocate TypeIb ----------
    // TypeIb = malloc(16 * sizeof(*TypeIb));
    // for (int i = 0; i < 16; i++) {
    //     TypeIb[i] = malloc(4 * sizeof(*TypeIb[i]));
    //     for (int j = 0; j < 4; j++) {
    //         TypeIb[i][j] = malloc(4096 * sizeof(*TypeIb[i][j]));
    //     }
    // }

    // // ---------- Allocate TypeIV_Ia ----------
    // TypeIV_Ia = malloc(4 * sizeof(*TypeIV_Ia));
    // for (int i = 0; i < 4; i++) {
    //     TypeIV_Ia[i] = malloc(8 * sizeof(*TypeIV_Ia[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeIV_Ia[i][j] = malloc(15 * sizeof(*TypeIV_Ia[i][j]));
    //         for (int k = 0; k < 15; k++) {
    //             TypeIV_Ia[i][j][k] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k]));
    //             for (int l = 0; l < 16; l++) {
    //                 TypeIV_Ia[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k][l]));
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_Ib ----------
    // TypeIV_Ib = malloc(4 * sizeof(*TypeIV_Ib));
    // for (int i = 0; i < 4; i++) {
    //     TypeIV_Ib[i] = malloc(8 * sizeof(*TypeIV_Ib[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeIV_Ib[i][j] = malloc(15 * sizeof(*TypeIV_Ib[i][j]));
    //         for (int k = 0; k < 15; k++) {
    //             TypeIV_Ib[i][j][k] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k]));
    //             for (int l = 0; l < 16; l++) {
    //                 TypeIV_Ib[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k][l]));
    //             }
    //         }
    //     }
    // }

    genMatpairM128(&IDM128, &IDM128_inv);
    genMatpairM128(&ODM128, &ODM128_inv);
    
    // M8 L[9][16];
    // M8 L_inv[9][16];
    // M128 MB[9];
    // M128 MB_inv[9];

    M8 L[ROUND][16];
    M8 L_inv[ROUND][16];
    M128 MB[ROUND];
    M128 MB_inv[ROUND];

    // // ---------- Allocate L & L_inv ----------
    // L = malloc((ROUND) * sizeof(*L));
    // L_inv = malloc((ROUND) * sizeof(*L_inv));
    // for (int i = 0; i < ROUND; i++) {
    //     L[i] = malloc(16 * sizeof(*L[i]));
    //     L_inv[i] = malloc(16 * sizeof(*L_inv[i]));
    // }

    // // ---------- Allocate MB & MB_inv ----------
    // MB = malloc((ROUND) * sizeof(*MB));
    // MB_inv = malloc((ROUND) * sizeof(*MB_inv));

    for(i = 0; i < (ROUND); i++)
    {
        for(j = 0; j < 16; j++)
        {
            genMatpairM8(&L[i][j], &L_inv[i][j]);
            // identityM8(&L[i][j]);
            // identityM8(&L_inv[i][j]);
        }
    }

    for(i = 0; i < (ROUND); i++)
    {
        genMatpairM128(&MB[i], &MB_inv[i]);
        // identityM128(&MB[i]);
        // identityM128(&MB_inv[i]);
    }

    

    M128 Out_L[ROUND];
    for(i = 0; i < ROUND; i++)
    {
        MatrixcomM8to128(L[i][0], L[i][1], L[i][2], L[i][3], L[i][4], L[i][5], L[i][6], L[i][7], L[i][8], L[i][9], L[i][10], L[i][11], L[i][12], L[i][13], L[i][14], L[i][15], &Out_L[i]);
    }

    

    u8 TypeII_out[ROUND][16][4][8][16];
    u8 TypeII_out_inv[ROUND][16][4][8][16];
    u8 TypeIV_II_out1[ROUND][4][8][14][16];
    u8 TypeIV_II_out1_inv[ROUND][4][8][14][16];
    u8 TypeIV_II_out2[ROUND][4][8][64];
    u8 TypeIV_II_out2_inv[ROUND][4][8][64];

    u8 TypeIII_out[ROUND][16][4][8][16];
    u8 TypeIII_out_inv[ROUND][16][4][8][16];
    u8 TypeIV_III_out1[ROUND][4][8][14][16];
    u8 TypeIV_III_out1_inv[ROUND][4][8][14][16];
    u8 TypeIV_III_out2[ROUND][4][8][64];
    u8 TypeIV_III_out2_inv[ROUND][4][8][64];

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
    for(i = 0; i < ROUND; i++)
    {
        for(j = 0; j < 16; j++)
        {
            for (y = 0; y < 4; ++y)
            {
                for(k = 0; k < 8; k++)
                {
                    u8 permutation[16];
                    u8 inverse[16];
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeII_out[i][j][y][k][x] = permutation[x];
                        TypeII_out_inv[i][j][y][k][x] = inverse[x];
                        // TypeII_out[i][j][y][k][x] = x;
                        // TypeII_out_inv[i][j][y][k][x] = x;
                    }
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIII_out[i][j][y][k][x] = permutation[x];
                        TypeIII_out_inv[i][j][y][k][x] = inverse[x];
                        // TypeIII_out[i][j][y][k][x] = x;
                        // TypeIII_out_inv[i][j][y][k][x] = x;
                    }
                }
            }
        }
    }
    for(i = 0; i < ROUND; i++)
    {
        for(j = 0; j < 4; j++)
        {
            for(k = 0; k < 8; k++)
            {
                for (y = 0; y < 14; ++y)
                {
                    u8 permutation[16];
                    u8 inverse[16];
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIV_II_out1[i][j][k][y][x] = permutation[x];
                        TypeIV_II_out1_inv[i][j][k][y][x] = inverse[x];
                        // TypeIV_II_out1[i][j][k][y][x] = x;
                        // TypeIV_II_out1_inv[i][j][k][y][x] = x;
                    }
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIV_III_out1[i][j][k][y][x] = permutation[x];
                        TypeIV_III_out1_inv[i][j][k][y][x] = inverse[x];
                        // TypeIV_III_out1[i][j][k][y][x] = x;
                        // TypeIV_III_out1_inv[i][j][k][y][x] = x;
                    }
                }
            }
        }
    }
    for(i = 0; i < ROUND; i++)
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
                    // TypeIV_II_out2[i][j][k][x] = x;
                    // TypeIV_II_out2_inv[i][j][k][x] = x;
                }
                generatePermutation64(permutation, inverse);
                for(x = 0; x < 64; x++)
                {
                    TypeIV_III_out2[i][j][k][x] = permutation[x];
                    TypeIV_III_out2_inv[i][j][k][x] = inverse[x];
                    // TypeIV_III_out2[i][j][k][x] = x;
                    // TypeIV_III_out2_inv[i][j][k][x] = x;
                }
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
                    // TypeIa_out[i][j][k][x] = x;
                    // TypeIa_out_inv[i][j][k][x] = x;
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
                    // TypeIV_Ia_out1[i][j][k][x] = x;
                    // TypeIV_Ia_out1_inv[i][j][k][x] = x;
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

                // TypeIV_Ia_out2[i][j][x] = x;
                // TypeIV_Ia_out2_inv[i][j][x] = x;
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
    //IDM128
    for (j = 0; j < 16; ++j)
    {
        for (x = 0; x < 256; ++x)
        {
            initV128(&v128);
            v128.V[j / 8] = (u64)x << ((7 - (j % 8)) * 8);
            MatMulVecM128(IDM128_inv, v128, &v128);
            TypeIa[j][1][x] = TypeIa_out[j][1][0][v128.V[0] & 0xf] | (TypeIa_out[j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIa_out[j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIa_out[j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIa_out[j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeIa[j][0][x] = TypeIa_out[j][0][0][v128.V[0] & 0xf] | (TypeIa_out[j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIa_out[j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIa_out[j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIa_out[j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeIa[j][3][x] = TypeIa_out[j][3][0][v128.V[1] & 0xf] | (TypeIa_out[j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIa_out[j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIa_out[j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIa_out[j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeIa[j][2][x] = TypeIa_out[j][2][0][v128.V[1] & 0xf] | (TypeIa_out[j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIa_out[j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIa_out[j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIa_out[j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIa_out[j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIa_out[j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIa_out[j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
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
    for(j = 0; j < 16; j++)//type_II
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            int tmp = j;
            int ttmp = 3 - (tmp % 4);
            lx = TypeIV_Ia_out2_inv[tmp / 4][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_Ia_out2_inv[tmp / 4][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = SBox[temp_u8 ^ key[j]];
            // temp_u32 = Tyi[j % 4][temp_u8];

            initV128(&v128);
            
            v128.V[j / 8] = (u64)temp_u8 << ((7 - (j % 8)) * 8);

            // if (j < 4)
            // {
            //     v128.V[0] = (u64)temp_u32 << 32;
            // } else if (j < 8)
            // {
            //     v128.V[0] = (u64)temp_u32;
            // } else if (j < 12)
            // {
            //     v128.V[1] = (u64)temp_u32 << 32;
            // } else
            // {
            //     v128.V[1] = (u64)temp_u32;
            // }
            MatMulVecM128(slt_m128, v128, &v128);
            MatMulVecM128(MB[0], v128, &v128);
            TypeII[0][j][1][x] = TypeII_out[0][j][1][0][v128.V[0] & 0xf] | (TypeII_out[0][j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeII_out[0][j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeII_out[0][j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeII_out[0][j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeII[0][j][0][x] = TypeII_out[0][j][0][0][v128.V[0] & 0xf] | (TypeII_out[0][j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeII_out[0][j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeII_out[0][j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeII_out[0][j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeII[0][j][3][x] = TypeII_out[0][j][3][0][v128.V[1] & 0xf] | (TypeII_out[0][j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeII_out[0][j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeII_out[0][j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeII_out[0][j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeII[0][j][2][x] = TypeII_out[0][j][2][0][v128.V[1] & 0xf] | (TypeII_out[0][j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeII_out[0][j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeII_out[0][j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeII_out[0][j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
        }
    }
    for(j = 0; j < 16; j++)//type_III
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            int ttmp = 3 - (j % 4);
            lx = TypeIV_II_out2_inv[0][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_II_out2_inv[0][columnindex[j]][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));


            initV128(&v128);
            v128.V[j / 8] = (u64)temp_u8 << ((7 - (j % 8)) * 8);
            // if (j < 4)
            // {
            //     v128.V[0] = (u64)temp_u32 << 32;
            // } else if (j < 8)
            // {
            //     v128.V[0] = (u64)temp_u32;
            // } else if (j < 12)
            // {
            //     v128.V[1] = (u64)temp_u32 << 32;
            // } else
            // {
            //     v128.V[1] = (u64)temp_u32;
            // }
            MatMulVecM128(MB_inv[0], v128, &v128);
            MatMulVecM128(Out_L[0], v128, &v128);
            TypeIII[0][j][1][x] = TypeIII_out[0][j][1][0][v128.V[0] & 0xf] | (TypeIII_out[0][j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeIII[0][j][0][x] = TypeIII_out[0][j][0][0][v128.V[0] & 0xf] | (TypeIII_out[0][j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeIII[0][j][3][x] = TypeIII_out[0][j][3][0][v128.V[1] & 0xf] | (TypeIII_out[0][j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeIII[0][j][2][x] = TypeIII_out[0][j][2][0][v128.V[1] & 0xf] | (TypeIII_out[0][j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
        }
    }

    //Round 2-9
    for (i = 1; i < ROUND; i++)//Type_II
    {
        for(j = 0; j < 16; j++)
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            for(x = 0; x < 4096; x++)
            {
                int ttmp = 3 - (j % 4);
                lx = TypeIV_III_out2_inv[i - 1][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
                rx = TypeIV_III_out2_inv[i - 1][columnindex[j]][ttmp * 2][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
                temp_u8 = MatMulNumM8(L_inv[i - 1][j], temp_u8);
                temp_u8 = SBox[temp_u8 ^ key[j]];
                // temp_u32 = Tyi[j % 4][temp_u8];


                initV128(&v128);
                v128.V[j / 8] = (u64)temp_u8 << ((7 - (j % 8)) * 8);
                // if (j < 4)
                // {
                //     v128.V[0] = (u64)temp_u32 << 32;
                // } else if (j < 8)
                // {
                //     v128.V[0] = (u64)temp_u32;
                // } else if (j < 12)
                // {
                //     v128.V[1] = (u64)temp_u32 << 32;
                // } else
                // {
                //     v128.V[1] = (u64)temp_u32;
                // }
                MatMulVecM128(slt_m128, v128, &v128);
                MatMulVecM128(MB[i], v128, &v128);
                TypeII[i][j][1][x] = TypeII_out[i][j][1][0][v128.V[0] & 0xf] | (TypeII_out[i][j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeII_out[i][j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeII_out[i][j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeII_out[i][j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
                v128.V[0] >>= 32;
                TypeII[i][j][0][x] = TypeII_out[i][j][0][0][v128.V[0] & 0xf] | (TypeII_out[i][j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeII_out[i][j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeII_out[i][j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeII_out[i][j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
                TypeII[i][j][3][x] = TypeII_out[i][j][3][0][v128.V[1] & 0xf] | (TypeII_out[i][j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeII_out[i][j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeII_out[i][j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeII_out[i][j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
                v128.V[1] >>= 32;
                TypeII[i][j][2][x] = TypeII_out[i][j][2][0][v128.V[1] & 0xf] | (TypeII_out[i][j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeII_out[i][j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeII_out[i][j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeII_out[i][j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            }
        }
    
        for(j = 0; j < 16; j++)//type_III
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            for(x = 0; x < 4096; x++)
            {
                int ttmp = 3 - (j % 4);
                lx = TypeIV_II_out2_inv[i][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
                rx = TypeIV_II_out2_inv[i][columnindex[j]][ttmp * 2][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));


                initV128(&v128);
                v128.V[j / 8] = (u64)temp_u8 << ((7 - (j % 8)) * 8);
                // if (j < 4)
                // {
                //     v128.V[0] = (u64)temp_u32 << 32;
                // } else if (j < 8)
                // {
                //     v128.V[0] = (u64)temp_u32;
                // } else if (j < 12)
                // {
                //     v128.V[1] = (u64)temp_u32 << 32;
                // } else
                // {
                //     v128.V[1] = (u64)temp_u32;
                // }
                
                MatMulVecM128(MB_inv[i], v128, &v128);
                MatMulVecM128(Out_L[i], v128, &v128);
                TypeIII[i][j][1][x] = TypeIII_out[i][j][1][0][v128.V[0] & 0xf] | (TypeIII_out[i][j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
                v128.V[0] >>= 32;
                TypeIII[i][j][0][x] = TypeIII_out[i][j][0][0][v128.V[0] & 0xf] | (TypeIII_out[i][j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
                TypeIII[i][j][3][x] = TypeIII_out[i][j][3][0][v128.V[1] & 0xf] | (TypeIII_out[i][j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
                v128.V[1] >>= 32;
                TypeIII[i][j][2][x] = TypeIII_out[i][j][2][0][v128.V[1] & 0xf] | (TypeIII_out[i][j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            }
        }
    }

    // ODM128
    for(j = 0; j < 16; j++)//type_II
    {
        u8 temp_u8;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            initV128(&v128);
            int ttmp = 3 - (j % 4);
            lx = TypeIV_III_out2_inv[ROUND - 1][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_III_out2_inv[ROUND - 1][columnindex[j]][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = MatMulNumM8(L_inv[ROUND - 1][j], temp_u8);
            temp_u8 = SBox[temp_u8 ^ key[j]];
            v128.V[j / 8] = (u64)temp_u8 << ((7 - (j % 8)) * 8);
            MatMulVecM128(slt_m128, v128, &v128);
            MatMulVecM128(ODM128_inv, v128, &v128);
            TypeIb[j][1][x] = TypeIb_out[j][1][0][v128.V[0] & 0xf] | (TypeIb_out[j][1][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIb_out[j][1][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIb_out[j][1][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIb_out[j][1][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][1][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][1][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][1][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            v128.V[0] >>= 32;
            TypeIb[j][0][x] = TypeIb_out[j][0][0][v128.V[0] & 0xf] | (TypeIb_out[j][0][1][(v128.V[0] & 0xf0) >> 4] << 4) | (TypeIb_out[j][0][2][(v128.V[0] & 0xf00) >> 8] << 8) | (TypeIb_out[j][0][3][(v128.V[0] & 0xf000) >> 12] << 12) | (TypeIb_out[j][0][4][(v128.V[0] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][0][5][(v128.V[0] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][0][6][(v128.V[0] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][0][7][(v128.V[0] & 0xf0000000) >> 28] << 28);
            TypeIb[j][3][x] = TypeIb_out[j][3][0][v128.V[1] & 0xf] | (TypeIb_out[j][3][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIb_out[j][3][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIb_out[j][3][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIb_out[j][3][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][3][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][3][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][3][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
            v128.V[1] >>= 32;
            TypeIb[j][2][x] = TypeIb_out[j][2][0][v128.V[1] & 0xf] | (TypeIb_out[j][2][1][(v128.V[1] & 0xf0) >> 4] << 4) | (TypeIb_out[j][2][2][(v128.V[1] & 0xf00) >> 8] << 8) | (TypeIb_out[j][2][3][(v128.V[1] & 0xf000) >> 12] << 12) | (TypeIb_out[j][2][4][(v128.V[1] & 0xf0000) >> 16] << 16) | (TypeIb_out[j][2][5][(v128.V[1] & 0xf00000) >> 20] << 20) | (TypeIb_out[j][2][6][(v128.V[1] & 0xf000000) >> 24] << 24) | (TypeIb_out[j][2][7][(v128.V[1] & 0xf0000000) >> 28] << 28);
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

    for (int rr = 0; rr < ROUND; ++rr)
    {
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
                            TypeIV_II[rr][i][j][k][x][y] = TypeIV_II_out1[rr][i][j][k][TypeII_out_inv[rr][k * 2][i][j][x] ^ TypeII_out_inv[rr][k * 2 + 1][i][j][y]];
                            TypeIV_III[rr][i][j][k][x][y] = TypeIV_III_out1[rr][i][j][k][TypeIII_out_inv[rr][k * 2][i][j][x] ^ TypeIII_out_inv[rr][k * 2 + 1][i][j][y]];
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
                            TypeIV_II[rr][i][j][k][x][y] = TypeIV_II_out1[rr][i][j][k][TypeIV_II_out1_inv[rr][i][j][tmp * 2][x] ^ TypeIV_II_out1_inv[rr][i][j][tmp * 2 + 1][y]];
                            TypeIV_III[rr][i][j][k][x][y] = TypeIV_III_out1[rr][i][j][k][TypeIV_III_out1_inv[rr][i][j][tmp * 2][x] ^ TypeIV_III_out1_inv[rr][i][j][tmp * 2 + 1][y]];
                        }
                    }
                }
                for (x = 0; x < 16; ++x)
                {
                    u8 px, py;
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_II[rr][i][j][12][x][y] = TypeIV_II_out1[rr][i][j][12][TypeIV_II_out1_inv[rr][i][j][8][x] ^ TypeIV_II_out1_inv[rr][i][j][9][y]];
                        TypeIV_II[rr][i][j][13][x][y] = TypeIV_II_out1[rr][i][j][13][TypeIV_II_out1_inv[rr][i][j][10][x] ^ TypeIV_II_out1_inv[rr][i][j][11][y]];
                        px = TypeIV_II_out1_inv[rr][i][j][12][x];
                        py = TypeIV_II_out1_inv[rr][i][j][13][y];
                        TypeIV_II[rr][i][j][14][x][y] = TypeIV_II_out2[rr][i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];

                        TypeIV_III[rr][i][j][12][x][y] = TypeIV_III_out1[rr][i][j][12][TypeIV_III_out1_inv[rr][i][j][8][x] ^ TypeIV_III_out1_inv[rr][i][j][9][y]];
                        TypeIV_III[rr][i][j][13][x][y] = TypeIV_III_out1[rr][i][j][13][TypeIV_III_out1_inv[rr][i][j][10][x] ^ TypeIV_III_out1_inv[rr][i][j][11][y]];
                        px = TypeIV_III_out1_inv[rr][i][j][12][x];
                        py = TypeIV_III_out1_inv[rr][i][j][13][y];
                        TypeIV_III[rr][i][j][14][x][y] = TypeIV_III_out2[rr][i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                    }
                }
            }
        }
    }
}

void wbslt128_encrypt(u8 input[16], u8 output[16], int R) 
{
    int i, j;
    u32 a, b, c, d, ab, cd;
    u16 state[16];
    for(i = 0; i < 16; i++)
    {
        state[i] = input[i];
    }
    
    //IDM128
    u16 tmp_state[16];
    for (j = 0; j < 16; j++)
    {
        a = j / 4; // column index
        b = 3 - (j % 4); // row index
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
        ]) | 
        (TypeIV_Ia[a][d][14]
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
        ] << 6);
    }
    for (i = 0; i < 16; ++i)
    {
        state[i] = tmp_state[i];
    }

    // Round 1 to R-1
    for (i = 0; i < R - 1; i++) 
    {
        for (j = 0; j < 16; j++)
        {
            a = j / 4; // column index
            b = 3 - (j % 4); // row index
            c = b * 2; // 
            d = b * 2 + 1;
            tmp_state[j] = 
            (TypeIV_II[i][a][c][14]
            [
                TypeIV_II[i][a][c][12]
                [
                    TypeIV_II[i][a][c][8]
                    [
                        TypeIV_II[i][a][c][0][(TypeII[i][0][a][state[0]] >> (4 * c)) & 0xf][(TypeII[i][1][a][state[1]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][c][1][(TypeII[i][2][a][state[2]] >> (4 * c)) & 0xf][(TypeII[i][3][a][state[3]] >> (4 * c)) & 0xf]
                    ]
                ]
                [
                    TypeIV_II[i][a][c][9]
                    [
                        TypeIV_II[i][a][c][2][(TypeII[i][4][a][state[4]] >> (4 * c)) & 0xf][(TypeII[i][5][a][state[5]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][c][3][(TypeII[i][6][a][state[6]] >> (4 * c)) & 0xf][(TypeII[i][7][a][state[7]] >> (4 * c)) & 0xf]
                    ]
                ]
            ]
            [
                TypeIV_II[i][a][c][13]
                [
                    TypeIV_II[i][a][c][10]
                    [
                        TypeIV_II[i][a][c][4][(TypeII[i][8][a][state[8]] >> (4 * c)) & 0xf][(TypeII[i][9][a][state[9]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][c][5][(TypeII[i][10][a][state[10]] >> (4 * c)) & 0xf][(TypeII[i][11][a][state[11]] >> (4 * c)) & 0xf]
                    ]
                ]
                [
                    TypeIV_II[i][a][c][11]
                    [
                        TypeIV_II[i][a][c][6][(TypeII[i][12][a][state[12]] >> (4 * c)) & 0xf][(TypeII[i][13][a][state[13]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][c][7][(TypeII[i][14][a][state[14]] >> (4 * c)) & 0xf][(TypeII[i][15][a][state[15]] >> (4 * c)) & 0xf]
                    ]
                ]
            ]) | 
            (TypeIV_II[i][a][d][14]
            [
                TypeIV_II[i][a][d][12]
                [
                    TypeIV_II[i][a][d][8]
                    [
                        TypeIV_II[i][a][d][0][(TypeII[i][0][a][state[0]] >> (4 * d)) & 0xf][(TypeII[i][1][a][state[1]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][d][1][(TypeII[i][2][a][state[2]] >> (4 * d)) & 0xf][(TypeII[i][3][a][state[3]] >> (4 * d)) & 0xf]
                    ]
                ]
                [
                    TypeIV_II[i][a][d][9]
                    [
                        TypeIV_II[i][a][d][2][(TypeII[i][4][a][state[4]] >> (4 * d)) & 0xf][(TypeII[i][5][a][state[5]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][d][3][(TypeII[i][6][a][state[6]] >> (4 * d)) & 0xf][(TypeII[i][7][a][state[7]] >> (4 * d)) & 0xf]
                    ]
                ]
            ]
            [
                TypeIV_II[i][a][d][13]
                [
                    TypeIV_II[i][a][d][10]
                    [
                        TypeIV_II[i][a][d][4][(TypeII[i][8][a][state[8]] >> (4 * d)) & 0xf][(TypeII[i][9][a][state[9]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][d][5][(TypeII[i][10][a][state[10]] >> (4 * d)) & 0xf][(TypeII[i][11][a][state[11]] >> (4 * d)) & 0xf]
                    ]
                ]
                [
                    TypeIV_II[i][a][d][11]
                    [
                        TypeIV_II[i][a][d][6][(TypeII[i][12][a][state[12]] >> (4 * d)) & 0xf][(TypeII[i][13][a][state[13]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_II[i][a][d][7][(TypeII[i][14][a][state[14]] >> (4 * d)) & 0xf][(TypeII[i][15][a][state[15]] >> (4 * d)) & 0xf]
                    ]
                ]
            ] << 6);
        }
        for (ab = 0; ab < 16; ++ab)
        {
            state[ab] = tmp_state[ab];
        }

        for (j = 0; j < 16; j++)
        {
            a = j / 4; // column index
            b = 3 - (j % 4); // row index
            c = b * 2; // 
            d = b * 2 + 1;
            tmp_state[j] = 
            (TypeIV_III[i][a][c][14]
            [
                TypeIV_III[i][a][c][12]
                [
                    TypeIV_III[i][a][c][8]
                    [
                        TypeIV_III[i][a][c][0][(TypeIII[i][0][a][state[0]] >> (4 * c)) & 0xf][(TypeIII[i][1][a][state[1]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][c][1][(TypeIII[i][2][a][state[2]] >> (4 * c)) & 0xf][(TypeIII[i][3][a][state[3]] >> (4 * c)) & 0xf]
                    ]
                ]
                [
                    TypeIV_III[i][a][c][9]
                    [
                        TypeIV_III[i][a][c][2][(TypeIII[i][4][a][state[4]] >> (4 * c)) & 0xf][(TypeIII[i][5][a][state[5]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][c][3][(TypeIII[i][6][a][state[6]] >> (4 * c)) & 0xf][(TypeIII[i][7][a][state[7]] >> (4 * c)) & 0xf]
                    ]
                ]
            ]
            [
                TypeIV_III[i][a][c][13]
                [
                    TypeIV_III[i][a][c][10]
                    [
                        TypeIV_III[i][a][c][4][(TypeIII[i][8][a][state[8]] >> (4 * c)) & 0xf][(TypeIII[i][9][a][state[9]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][c][5][(TypeIII[i][10][a][state[10]] >> (4 * c)) & 0xf][(TypeIII[i][11][a][state[11]] >> (4 * c)) & 0xf]
                    ]
                ]
                [
                    TypeIV_III[i][a][c][11]
                    [
                        TypeIV_III[i][a][c][6][(TypeIII[i][12][a][state[12]] >> (4 * c)) & 0xf][(TypeIII[i][13][a][state[13]] >> (4 * c)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][c][7][(TypeIII[i][14][a][state[14]] >> (4 * c)) & 0xf][(TypeIII[i][15][a][state[15]] >> (4 * c)) & 0xf]
                    ]
                ]
            ]) | 
            (TypeIV_III[i][a][d][14]
            [
                TypeIV_III[i][a][d][12]
                [
                    TypeIV_III[i][a][d][8]
                    [
                        TypeIV_III[i][a][d][0][(TypeIII[i][0][a][state[0]] >> (4 * d)) & 0xf][(TypeIII[i][1][a][state[1]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][d][1][(TypeIII[i][2][a][state[2]] >> (4 * d)) & 0xf][(TypeIII[i][3][a][state[3]] >> (4 * d)) & 0xf]
                    ]
                ]
                [
                    TypeIV_III[i][a][d][9]
                    [
                        TypeIV_III[i][a][d][2][(TypeIII[i][4][a][state[4]] >> (4 * d)) & 0xf][(TypeIII[i][5][a][state[5]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][d][3][(TypeIII[i][6][a][state[6]] >> (4 * d)) & 0xf][(TypeIII[i][7][a][state[7]] >> (4 * d)) & 0xf]
                    ]
                ]
            ]
            [
                TypeIV_III[i][a][d][13]
                [
                    TypeIV_III[i][a][d][10]
                    [
                        TypeIV_III[i][a][d][4][(TypeIII[i][8][a][state[8]] >> (4 * d)) & 0xf][(TypeIII[i][9][a][state[9]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][d][5][(TypeIII[i][10][a][state[10]] >> (4 * d)) & 0xf][(TypeIII[i][11][a][state[11]] >> (4 * d)) & 0xf]
                    ]
                ]
                [
                    TypeIV_III[i][a][d][11]
                    [
                        TypeIV_III[i][a][d][6][(TypeIII[i][12][a][state[12]] >> (4 * d)) & 0xf][(TypeIII[i][13][a][state[13]] >> (4 * d)) & 0xf]
                    ]
                    [
                        TypeIV_III[i][a][d][7][(TypeIII[i][14][a][state[14]] >> (4 * d)) & 0xf][(TypeIII[i][15][a][state[15]] >> (4 * d)) & 0xf]
                    ]
                ]
            ] << 6);
        }
        for (ab = 0; ab < 16; ++ab)
        {
            state[ab] = tmp_state[ab];
        }
    }
    
    //Round R
    for (j = 0; j < 16; j++)
    {
        a = j / 4; // column index
        b = 3 - (j % 4); // row index
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









void general_wbslt64_gen(u8 key[8], int R)
{
    int i, j, x, y, k;
    V64 v64;

    const int ROUND = R - 1;
    // init_table(R, 64);
    // // ---------- Allocate TypeII ----------
    // TypeII = malloc(ROUND * sizeof(*TypeII));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeII[i] = malloc(8 * sizeof(*TypeII[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeII[i][j] = malloc(2 * sizeof(*TypeII[i][j]));
    //         for (int k = 0; k < 2; k++) {
    //             TypeII[i][j][k] = malloc(4096 * sizeof(*TypeII[i][j][k]));
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIII ----------
    // TypeIII = malloc(ROUND * sizeof(*TypeIII));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIII[i] = malloc(8 * sizeof(*TypeIII[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeIII[i][j] = malloc(2 * sizeof(*TypeIII[i][j]));
    //         for (int k = 0; k < 2; k++) {
    //             TypeIII[i][j][k] = malloc(4096 * sizeof(*TypeIII[i][j][k]));
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_II ----------
    // TypeIV_II = malloc(ROUND * sizeof(*TypeIV_II));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIV_II[i] = malloc(2 * sizeof(*TypeIV_II[i]));
    //     for (int j = 0; j < 2; j++) {
    //         TypeIV_II[i][j] = malloc(8 * sizeof(*TypeIV_II[i][j]));
    //         for (int k = 0; k < 8; k++) {
    //             TypeIV_II[i][j][k] = malloc(7 * sizeof(*TypeIV_II[i][j][k]));
    //             for (int l = 0; l < 7; l++) {
    //                 TypeIV_II[i][j][k][l] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l]));
    //                 for (int m = 0; m < 16; m++) {
    //                     TypeIV_II[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_II[i][j][k][l][m]));
    //                 }
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_III ----------
    // TypeIV_III = malloc(ROUND * sizeof(*TypeIV_III));
    // for (int i = 0; i < ROUND; i++) {
    //     TypeIV_III[i] = malloc(2 * sizeof(*TypeIV_III[i]));
    //     for (int j = 0; j < 2; j++) {
    //         TypeIV_III[i][j] = malloc(8 * sizeof(*TypeIV_III[i][j]));
    //         for (int k = 0; k < 8; k++) {
    //             TypeIV_III[i][j][k] = malloc(7 * sizeof(*TypeIV_III[i][j][k]));
    //             for (int l = 0; l < 7; l++) {
    //                 TypeIV_III[i][j][k][l] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l]));
    //                 for (int m = 0; m < 16; m++) {
    //                     TypeIV_III[i][j][k][l][m] = malloc(16 * sizeof(*TypeIV_III[i][j][k][l][m]));
    //                 }
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIa ----------
    // TypeIa = malloc(8 * sizeof(*TypeIa));
    // for (int i = 0; i < 8; i++) {
    //     TypeIa[i] = malloc(2 * sizeof(*TypeIa[i]));
    //     for (int j = 0; j < 2; j++) {
    //         TypeIa[i][j] = malloc(256 * sizeof(*TypeIa[i][j]));
    //     }
    // }

    // // ---------- Allocate TypeIb ----------
    // TypeIb = malloc(8 * sizeof(*TypeIb));
    // for (int i = 0; i < 8; i++) {
    //     TypeIb[i] = malloc(2 * sizeof(*TypeIb[i]));
    //     for (int j = 0; j < 2; j++) {
    //         TypeIb[i][j] = malloc(4096 * sizeof(*TypeIb[i][j]));
    //     }
    // }

    // // ---------- Allocate TypeIV_Ia ----------
    // TypeIV_Ia = malloc(2 * sizeof(*TypeIV_Ia));
    // for (int i = 0; i < 2; i++) {
    //     TypeIV_Ia[i] = malloc(8 * sizeof(*TypeIV_Ia[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeIV_Ia[i][j] = malloc(7 * sizeof(*TypeIV_Ia[i][j]));
    //         for (int k = 0; k < 7; k++) {
    //             TypeIV_Ia[i][j][k] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k]));
    //             for (int l = 0; l < 16; l++) {
    //                 TypeIV_Ia[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ia[i][j][k][l]));
    //             }
    //         }
    //     }
    // }

    // // ---------- Allocate TypeIV_Ib ----------
    // TypeIV_Ib = malloc(2 * sizeof(*TypeIV_Ib));
    // for (int i = 0; i < 2; i++) {
    //     TypeIV_Ib[i] = malloc(8 * sizeof(*TypeIV_Ib[i]));
    //     for (int j = 0; j < 8; j++) {
    //         TypeIV_Ib[i][j] = malloc(7 * sizeof(*TypeIV_Ib[i][j]));
    //         for (int k = 0; k < 7; k++) {
    //             TypeIV_Ib[i][j][k] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k]));
    //             for (int l = 0; l < 16; l++) {
    //                 TypeIV_Ib[i][j][k][l] = malloc(16 * sizeof(*TypeIV_Ib[i][j][k][l]));
    //             }
    //         }
    //     }
    // }

    genMatpairM64(&IDM64, &IDM64_inv);
    genMatpairM64(&ODM64, &ODM64_inv);
    
    // M8 L[9][16];
    // M8 L_inv[9][16];
    // M128 MB[9];
    // M128 MB_inv[9];

    M8 L[ROUND][8];
    M8 L_inv[ROUND][8];
    M64 MB[ROUND];
    M64 MB_inv[ROUND];

    // // ---------- Allocate L & L_inv ----------
    // L = malloc((ROUND) * sizeof(*L));
    // L_inv = malloc((ROUND) * sizeof(*L_inv));
    // for (int i = 0; i < ROUND; i++) {
    //     L[i] = malloc(16 * sizeof(*L[i]));
    //     L_inv[i] = malloc(16 * sizeof(*L_inv[i]));
    // }

    // // ---------- Allocate MB & MB_inv ----------
    // MB = malloc((ROUND) * sizeof(*MB));
    // MB_inv = malloc((ROUND) * sizeof(*MB_inv));

    for(i = 0; i < (ROUND); i++)
    {
        for(j = 0; j < 8; j++)
        {
            genMatpairM8(&L[i][j], &L_inv[i][j]);
            // identityM8(&L[i][j]);
            // identityM8(&L_inv[i][j]);
        }
    }

    for(i = 0; i < (ROUND); i++)
    {
        genMatpairM64(&MB[i], &MB_inv[i]);
        // identityM128(&MB[i]);
        // identityM128(&MB_inv[i]);
    }

    

    M64 Out_L[ROUND];
    for(i = 0; i < ROUND; i++)
    {
        MatrixcomM8to64(L[i][0], L[i][1], L[i][2], L[i][3], L[i][4], L[i][5], L[i][6], L[i][7], &Out_L[i]);
    }

    

    u8 TypeII_out[ROUND][16][4][8][16];
    u8 TypeII_out_inv[ROUND][16][4][8][16];
    u8 TypeIV_II_out1[ROUND][4][8][14][16];
    u8 TypeIV_II_out1_inv[ROUND][4][8][14][16];
    u8 TypeIV_II_out2[ROUND][4][8][64];
    u8 TypeIV_II_out2_inv[ROUND][4][8][64];

    u8 TypeIII_out[ROUND][16][4][8][16];
    u8 TypeIII_out_inv[ROUND][16][4][8][16];
    u8 TypeIV_III_out1[ROUND][4][8][14][16];
    u8 TypeIV_III_out1_inv[ROUND][4][8][14][16];
    u8 TypeIV_III_out2[ROUND][4][8][64];
    u8 TypeIV_III_out2_inv[ROUND][4][8][64];

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
    for(i = 0; i < ROUND; i++)
    {
        for(j = 0; j < 16; j++)
        {
            for (y = 0; y < 4; ++y)
            {
                for(k = 0; k < 8; k++)
                {
                    u8 permutation[16];
                    u8 inverse[16];
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeII_out[i][j][y][k][x] = permutation[x];
                        TypeII_out_inv[i][j][y][k][x] = inverse[x];
                        // TypeII_out[i][j][y][k][x] = x;
                        // TypeII_out_inv[i][j][y][k][x] = x;
                    }
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIII_out[i][j][y][k][x] = permutation[x];
                        TypeIII_out_inv[i][j][y][k][x] = inverse[x];
                        // TypeIII_out[i][j][y][k][x] = x;
                        // TypeIII_out_inv[i][j][y][k][x] = x;
                    }
                }
            }
        }
    }
    for(i = 0; i < ROUND; i++)
    {
        for(j = 0; j < 4; j++)
        {
            for(k = 0; k < 8; k++)
            {
                for (y = 0; y < 14; ++y)
                {
                    u8 permutation[16];
                    u8 inverse[16];
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIV_II_out1[i][j][k][y][x] = permutation[x];
                        TypeIV_II_out1_inv[i][j][k][y][x] = inverse[x];
                        // TypeIV_II_out1[i][j][k][y][x] = x;
                        // TypeIV_II_out1_inv[i][j][k][y][x] = x;
                    }
                    generatePermutation(permutation, inverse);
                    for(x = 0; x < 16; x++)
                    {
                        TypeIV_III_out1[i][j][k][y][x] = permutation[x];
                        TypeIV_III_out1_inv[i][j][k][y][x] = inverse[x];
                        // TypeIV_III_out1[i][j][k][y][x] = x;
                        // TypeIV_III_out1_inv[i][j][k][y][x] = x;
                    }
                }
            }
        }
    }
    for(i = 0; i < ROUND; i++)
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
                    // TypeIV_II_out2[i][j][k][x] = x;
                    // TypeIV_II_out2_inv[i][j][k][x] = x;
                }
                generatePermutation64(permutation, inverse);
                for(x = 0; x < 64; x++)
                {
                    TypeIV_III_out2[i][j][k][x] = permutation[x];
                    TypeIV_III_out2_inv[i][j][k][x] = inverse[x];
                    // TypeIV_III_out2[i][j][k][x] = x;
                    // TypeIV_III_out2_inv[i][j][k][x] = x;
                }
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
                    // TypeIa_out[i][j][k][x] = x;
                    // TypeIa_out_inv[i][j][k][x] = x;
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
                    // TypeIV_Ia_out1[i][j][k][x] = x;
                    // TypeIV_Ia_out1_inv[i][j][k][x] = x;
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

                // TypeIV_Ia_out2[i][j][x] = x;
                // TypeIV_Ia_out2_inv[i][j][x] = x;
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
    //IDM
    for (j = 0; j < 8; ++j)
    {
        for (x = 0; x < 256; ++x)
        {
            initV64(&v64);
            v64.V = (u64)x << ((7 - (j % 8)) * 8);
            MatMulVecM64(IDM64_inv, v64, &v64);
            TypeIa[j][1][x] = TypeIa_out[j][1][0][v64.V & 0xf] | (TypeIa_out[j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIa_out[j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIa_out[j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIa_out[j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIa_out[j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIa_out[j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIa_out[j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
            v64.V >>= 32;
            TypeIa[j][0][x] = TypeIa_out[j][0][0][v64.V & 0xf] | (TypeIa_out[j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIa_out[j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIa_out[j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIa_out[j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIa_out[j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIa_out[j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIa_out[j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
        }
    }
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 4; ++k)
            {
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ia[i][j][k][x][y] = TypeIV_Ia_out1[i][j][k][TypeIa_out_inv[k * 2][i][j][x] ^ TypeIa_out_inv[k * 2 + 1][i][j][y]];
                    }
                }
            }
            // for (k = 4; k < 6; ++k)
            // {
            //     int tmp = k % 4;
            //     for (x = 0; x < 16; ++x)
            //     {
            //         for (y = 0; y < 16; ++y)
            //         {
            //             TypeIV_Ia[i][j][k][x][y] = TypeIV_Ia_out1[i][j][k][TypeIV_Ia_out1_inv[i][j][tmp * 2][x] ^ TypeIV_Ia_out1_inv[i][j][tmp * 2 + 1][y]];
            //         }
            //     }
            // }
            for (x = 0; x < 16; ++x)
            {
                for (y = 0; y < 16; ++y)
                {
                    TypeIV_Ia[i][j][4][x][y] = TypeIV_Ia_out1[i][j][4][TypeIV_Ia_out1_inv[i][j][0][x] ^ TypeIV_Ia_out1_inv[i][j][1][y]];
                    TypeIV_Ia[i][j][5][x][y] = TypeIV_Ia_out1[i][j][5][TypeIV_Ia_out1_inv[i][j][2][x] ^ TypeIV_Ia_out1_inv[i][j][3][y]];
                    u8 px, py;
                    px = TypeIV_Ia_out1_inv[i][j][4][x];
                    py = TypeIV_Ia_out1_inv[i][j][5][y];
                    TypeIV_Ia[i][j][6][x][y] = TypeIV_Ia_out2[i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                }
            }
        }
    }
    //Round 1
    for(j = 0; j < 8; j++)//type_II
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            int tmp = j;
            int ttmp = 3 - (tmp % 4);
            lx = TypeIV_Ia_out2_inv[tmp / 4][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_Ia_out2_inv[tmp / 4][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = SBox[temp_u8 ^ key[j]];
            // temp_u32 = Tyi[j % 4][temp_u8];

            initV64(&v64);
            
            v64.V = (u64)temp_u8 << ((7 - (j % 8)) * 8);

            // if (j < 4)
            // {
            //     v64.V = (u64)temp_u32 << 32;
            // } else if (j < 8)
            // {
            //     v64.V = (u64)temp_u32;
            // } else if (j < 12)
            // {
            //     v64.V[1] = (u64)temp_u32 << 32;
            // } else
            // {
            //     v64.V[1] = (u64)temp_u32;
            // }
            MatMulVecM64(slt_m64, v64, &v64);
            MatMulVecM64(MB[0], v64, &v64);
            TypeII[0][j][1][x] = TypeII_out[0][j][1][0][v64.V & 0xf] | (TypeII_out[0][j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeII_out[0][j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeII_out[0][j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeII_out[0][j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
            v64.V >>= 32;
            TypeII[0][j][0][x] = TypeII_out[0][j][0][0][v64.V & 0xf] | (TypeII_out[0][j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeII_out[0][j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeII_out[0][j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeII_out[0][j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeII_out[0][j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeII_out[0][j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeII_out[0][j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
        }
    }
    for(j = 0; j < 8; j++)//type_III
    {
        u8 temp_u8;
        u32 temp_u32;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            int ttmp = 3 - (j % 4);
            lx = TypeIV_II_out2_inv[0][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_II_out2_inv[0][columnindex[j]][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));


            initV64(&v64);
            v64.V = (u64)temp_u8 << ((7 - (j % 8)) * 8);
            // if (j < 4)
            // {
            //     v64.V = (u64)temp_u32 << 32;
            // } else if (j < 8)
            // {
            //     v64.V = (u64)temp_u32;
            // } else if (j < 12)
            // {
            //     v64.V[1] = (u64)temp_u32 << 32;
            // } else
            // {
            //     v64.V[1] = (u64)temp_u32;
            // }
            MatMulVecM64(MB_inv[0], v64, &v64);
            MatMulVecM64(Out_L[0], v64, &v64);
            TypeIII[0][j][1][x] = TypeIII_out[0][j][1][0][v64.V & 0xf] | (TypeIII_out[0][j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
            v64.V >>= 32;
            TypeIII[0][j][0][x] = TypeIII_out[0][j][0][0][v64.V & 0xf] | (TypeIII_out[0][j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIII_out[0][j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIII_out[0][j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIII_out[0][j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIII_out[0][j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIII_out[0][j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIII_out[0][j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
        }
    }

    //Round 2-9
    for (i = 1; i < ROUND; i++)//Type_II
    {
        for(j = 0; j < 8; j++)
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            for(x = 0; x < 4096; x++)
            {
                int ttmp = 3 - (j % 4);
                lx = TypeIV_III_out2_inv[i - 1][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
                rx = TypeIV_III_out2_inv[i - 1][columnindex[j]][ttmp * 2][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
                temp_u8 = MatMulNumM8(L_inv[i - 1][j], temp_u8);
                temp_u8 = SBox[temp_u8 ^ key[j]];
                // temp_u32 = Tyi[j % 4][temp_u8];


                initV64(&v64);
                v64.V = (u64)temp_u8 << ((7 - (j % 8)) * 8);
                // if (j < 4)
                // {
                //     v64.V = (u64)temp_u32 << 32;
                // } else if (j < 8)
                // {
                //     v64.V = (u64)temp_u32;
                // } else if (j < 12)
                // {
                //     v64.V[1] = (u64)temp_u32 << 32;
                // } else
                // {
                //     v64.V[1] = (u64)temp_u32;
                // }
                MatMulVecM64(slt_m64, v64, &v64);
                MatMulVecM64(MB[i], v64, &v64);
                TypeII[i][j][1][x] = TypeII_out[i][j][1][0][v64.V & 0xf] | (TypeII_out[i][j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeII_out[i][j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeII_out[i][j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeII_out[i][j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
                v64.V >>= 32;
                TypeII[i][j][0][x] = TypeII_out[i][j][0][0][v64.V & 0xf] | (TypeII_out[i][j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeII_out[i][j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeII_out[i][j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeII_out[i][j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeII_out[i][j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeII_out[i][j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeII_out[i][j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
            }
        }
    
        for(j = 0; j < 8; j++)//type_III
        {
            u8 temp_u8;
            u32 temp_u32;
            u8 lx, rx;
            for(x = 0; x < 4096; x++)
            {
                int ttmp = 3 - (j % 4);
                lx = TypeIV_II_out2_inv[i][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
                rx = TypeIV_II_out2_inv[i][columnindex[j]][ttmp * 2][(x & 0x03f)];
                temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));


                initV64(&v64);
                v64.V = (u64)temp_u8 << ((7 - (j % 8)) * 8);
                // if (j < 4)
                // {
                //     v64.V = (u64)temp_u32 << 32;
                // } else if (j < 8)
                // {
                //     v64.V = (u64)temp_u32;
                // } else if (j < 12)
                // {
                //     v64.V[1] = (u64)temp_u32 << 32;
                // } else
                // {
                //     v64.V[1] = (u64)temp_u32;
                // }
                
                MatMulVecM64(MB_inv[i], v64, &v64);
                MatMulVecM64(Out_L[i], v64, &v64);
                TypeIII[i][j][1][x] = TypeIII_out[i][j][1][0][v64.V & 0xf] | (TypeIII_out[i][j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
                v64.V >>= 32;
                TypeIII[i][j][0][x] = TypeIII_out[i][j][0][0][v64.V & 0xf] | (TypeIII_out[i][j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIII_out[i][j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIII_out[i][j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIII_out[i][j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIII_out[i][j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIII_out[i][j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIII_out[i][j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
            }
        }
    }

    // ODM
    for(j = 0; j < 8; j++)//type_II
    {
        u8 temp_u8;
        u8 lx, rx;
        for(x = 0; x < 4096; x++)
        {
            initV64(&v64);
            int ttmp = 3 - (j % 4);
            lx = TypeIV_III_out2_inv[ROUND - 1][columnindex[j]][ttmp * 2 + 1][(x & 0xfc0) >> 6];
            rx = TypeIV_III_out2_inv[ROUND - 1][columnindex[j]][ttmp * 2][(x & 0x03f)];
            temp_u8 = ((((lx >> 2) & 0xc) | ((lx & 0x3) ^ ((lx >> 2) & 0x3))) << 4) | (((rx >> 2) & 0xc) | ((rx & 0x3) ^ ((rx >> 2) & 0x3)));
            temp_u8 = MatMulNumM8(L_inv[ROUND - 1][j], temp_u8);
            temp_u8 = SBox[temp_u8 ^ key[j]];
            v64.V = (u64)temp_u8 << ((7 - (j % 8)) * 8);
            MatMulVecM64(slt_m64, v64, &v64);
            MatMulVecM64(ODM64_inv, v64, &v64);
            TypeIb[j][1][x] = TypeIb_out[j][1][0][v64.V & 0xf] | (TypeIb_out[j][1][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIb_out[j][1][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIb_out[j][1][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIb_out[j][1][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIb_out[j][1][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIb_out[j][1][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIb_out[j][1][7][(v64.V & 0xf0000000) >> 28] << 28);
            v64.V >>= 32;
            TypeIb[j][0][x] = TypeIb_out[j][0][0][v64.V & 0xf] | (TypeIb_out[j][0][1][(v64.V & 0xf0) >> 4] << 4) | (TypeIb_out[j][0][2][(v64.V & 0xf00) >> 8] << 8) | (TypeIb_out[j][0][3][(v64.V & 0xf000) >> 12] << 12) | (TypeIb_out[j][0][4][(v64.V & 0xf0000) >> 16] << 16) | (TypeIb_out[j][0][5][(v64.V & 0xf00000) >> 20] << 20) | (TypeIb_out[j][0][6][(v64.V & 0xf000000) >> 24] << 24) | (TypeIb_out[j][0][7][(v64.V & 0xf0000000) >> 28] << 28);
        }
    }
    for (i = 0; i < 2; ++i)
    {
        for (j = 0; j < 8; ++j)
        {
            for (k = 0; k < 4; ++k)
            {
                for (x = 0; x < 16; ++x)
                {
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_Ib[i][j][k][x][y] = TypeIV_Ib_out1[i][j][k][TypeIb_out_inv[k * 2][i][j][x] ^ TypeIb_out_inv[k * 2 + 1][i][j][y]];
                    }
                }
            }
            // for (k = 8; k < 12; ++k)
            // {
            //     int tmp = k % 8;
            //     for (x = 0; x < 16; ++x)
            //     {
            //         for (y = 0; y < 16; ++y)
            //         {
            //             TypeIV_Ib[i][j][k][x][y] = TypeIV_Ib_out1[i][j][k][TypeIV_Ib_out1_inv[i][j][tmp * 2][x] ^ TypeIV_Ib_out1_inv[i][j][tmp * 2 + 1][y]];
            //         }
            //     }
            // }
            for (x = 0; x < 16; ++x)
            {
                for (y = 0; y < 16; ++y)
                {
                    TypeIV_Ib[i][j][4][x][y] = TypeIV_Ib_out1[i][j][4][TypeIV_Ib_out1_inv[i][j][0][x] ^ TypeIV_Ib_out1_inv[i][j][1][y]];
                    TypeIV_Ib[i][j][5][x][y] = TypeIV_Ib_out1[i][j][5][TypeIV_Ib_out1_inv[i][j][2][x] ^ TypeIV_Ib_out1_inv[i][j][3][y]];
                    TypeIV_Ib[i][j][6][x][y] = TypeIV_Ib_out1_inv[i][j][4][x] ^ TypeIV_Ib_out1_inv[i][j][5][y];
                }
            }
        }
    }

    for (int rr = 0; rr < ROUND; ++rr)
    {
        for (i = 0; i < 2; ++i)
        {
            for (j = 0; j < 8; ++j)
            {
                for (k = 0; k < 4; ++k)
                {
                    for (x = 0; x < 16; ++x)
                    {
                        for (y = 0; y < 16; ++y)
                        {
                            TypeIV_II[rr][i][j][k][x][y] = TypeIV_II_out1[rr][i][j][k][TypeII_out_inv[rr][k * 2][i][j][x] ^ TypeII_out_inv[rr][k * 2 + 1][i][j][y]];
                            TypeIV_III[rr][i][j][k][x][y] = TypeIV_III_out1[rr][i][j][k][TypeIII_out_inv[rr][k * 2][i][j][x] ^ TypeIII_out_inv[rr][k * 2 + 1][i][j][y]];
                        }
                    }
                }
                // for (k = 8; k < 12; ++k)
                // {
                //     int tmp = k % 8;
                //     for (x = 0; x < 16; ++x)
                //     {
                //         for (y = 0; y < 16; ++y)
                //         {
                //             TypeIV_II[rr][i][j][k][x][y] = TypeIV_II_out1[rr][i][j][k][TypeIV_II_out1_inv[rr][i][j][tmp * 2][x] ^ TypeIV_II_out1_inv[rr][i][j][tmp * 2 + 1][y]];
                //             TypeIV_III[rr][i][j][k][x][y] = TypeIV_III_out1[rr][i][j][k][TypeIV_III_out1_inv[rr][i][j][tmp * 2][x] ^ TypeIV_III_out1_inv[rr][i][j][tmp * 2 + 1][y]];
                //         }
                //     }
                // }
                for (x = 0; x < 16; ++x)
                {
                    u8 px, py;
                    for (y = 0; y < 16; ++y)
                    {
                        TypeIV_II[rr][i][j][4][x][y] = TypeIV_II_out1[rr][i][j][4][TypeIV_II_out1_inv[rr][i][j][0][x] ^ TypeIV_II_out1_inv[rr][i][j][1][y]];
                        TypeIV_II[rr][i][j][5][x][y] = TypeIV_II_out1[rr][i][j][5][TypeIV_II_out1_inv[rr][i][j][2][x] ^ TypeIV_II_out1_inv[rr][i][j][3][y]];
                        px = TypeIV_II_out1_inv[rr][i][j][4][x];
                        py = TypeIV_II_out1_inv[rr][i][j][5][y];
                        TypeIV_II[rr][i][j][6][x][y] = TypeIV_II_out2[rr][i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];

                        TypeIV_III[rr][i][j][4][x][y] = TypeIV_III_out1[rr][i][j][4][TypeIV_III_out1_inv[rr][i][j][0][x] ^ TypeIV_III_out1_inv[rr][i][j][1][y]];
                        TypeIV_III[rr][i][j][5][x][y] = TypeIV_III_out1[rr][i][j][5][TypeIV_III_out1_inv[rr][i][j][2][x] ^ TypeIV_III_out1_inv[rr][i][j][3][y]];
                        px = TypeIV_III_out1_inv[rr][i][j][4][x];
                        py = TypeIV_III_out1_inv[rr][i][j][5][y];
                        TypeIV_III[rr][i][j][6][x][y] = TypeIV_III_out2[rr][i][j][(((px >> 2) ^ (py >> 2)) << 4) | ((px & 0x3) << 2) | (py & 0x3)];
                    }
                }
            }
        }
    }
}

void wbslt64_encrypt(u8 input[8], u8 output[8], int R) 
{
    int i, j;
    u32 a, b, c, d, ab, cd;
    u16 state[8];
    for(i = 0; i < 8; i++)
    {
        state[i] = input[i];
    }
    
    //IDM
    u16 tmp_state[8];
    for (j = 0; j < 8; j++)
    {
        a = j / 4; // column index
        b = 3 - (j % 4); // row index
        c = b * 2; // 
        d = b * 2 + 1;
        tmp_state[j] = 
        (TypeIV_Ia[a][c][6]
        [
            TypeIV_Ia[a][c][4]
            [
                TypeIV_Ia[a][c][0][(TypeIa[0][a][state[0]] >> (4 * c)) & 0xf][(TypeIa[1][a][state[1]] >> (4 * c)) & 0xf]
            ]
            [
                TypeIV_Ia[a][c][1][(TypeIa[2][a][state[2]] >> (4 * c)) & 0xf][(TypeIa[3][a][state[3]] >> (4 * c)) & 0xf]
            ]
        ]
        [
            TypeIV_Ia[a][c][5]
            [
                TypeIV_Ia[a][c][2][(TypeIa[4][a][state[4]] >> (4 * c)) & 0xf][(TypeIa[5][a][state[5]] >> (4 * c)) & 0xf]
            ]
            [
                TypeIV_Ia[a][c][3][(TypeIa[6][a][state[6]] >> (4 * c)) & 0xf][(TypeIa[7][a][state[7]] >> (4 * c)) & 0xf]
            ]
        ]) | 
        (TypeIV_Ia[a][d][6]
        [
            TypeIV_Ia[a][d][4]
            [
                TypeIV_Ia[a][d][0][(TypeIa[0][a][state[0]] >> (4 * d)) & 0xf][(TypeIa[1][a][state[1]] >> (4 * d)) & 0xf]
            ]
            [
                TypeIV_Ia[a][d][1][(TypeIa[2][a][state[2]] >> (4 * d)) & 0xf][(TypeIa[3][a][state[3]] >> (4 * d)) & 0xf]
            ]
        ]
        [
            TypeIV_Ia[a][d][5]
            [
                TypeIV_Ia[a][d][2][(TypeIa[4][a][state[4]] >> (4 * d)) & 0xf][(TypeIa[5][a][state[5]] >> (4 * d)) & 0xf]
            ]
            [
                TypeIV_Ia[a][d][3][(TypeIa[6][a][state[6]] >> (4 * d)) & 0xf][(TypeIa[7][a][state[7]] >> (4 * d)) & 0xf]
            ]
        ] << 6);
    }
    for (i = 0; i < 8; ++i)
    {
        state[i] = tmp_state[i];
    }

    // Round 1 to R-1
    for (i = 0; i < R - 1; i++) 
    {
        for (j = 0; j < 8; j++)
        {
            a = j / 4; // column index
            b = 3 - (j % 4); // row index
            c = b * 2; // 
            d = b * 2 + 1;
            tmp_state[j] = 
            (TypeIV_II[i][a][c][6]
            [
                TypeIV_II[i][a][c][4]
                [
                    TypeIV_II[i][a][c][0][(TypeII[i][0][a][state[0]] >> (4 * c)) & 0xf][(TypeII[i][1][a][state[1]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_II[i][a][c][1][(TypeII[i][2][a][state[2]] >> (4 * c)) & 0xf][(TypeII[i][3][a][state[3]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_II[i][a][c][5]
                [
                    TypeIV_II[i][a][c][2][(TypeII[i][4][a][state[4]] >> (4 * c)) & 0xf][(TypeII[i][5][a][state[5]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_II[i][a][c][3][(TypeII[i][6][a][state[6]] >> (4 * c)) & 0xf][(TypeII[i][7][a][state[7]] >> (4 * c)) & 0xf]
                ]
            ]) | 
            (TypeIV_II[i][a][d][6]
            [
                TypeIV_II[i][a][d][4]
                [
                    TypeIV_II[i][a][d][0][(TypeII[i][0][a][state[0]] >> (4 * d)) & 0xf][(TypeII[i][1][a][state[1]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_II[i][a][d][1][(TypeII[i][2][a][state[2]] >> (4 * d)) & 0xf][(TypeII[i][3][a][state[3]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_II[i][a][d][5]
                [
                    TypeIV_II[i][a][d][2][(TypeII[i][4][a][state[4]] >> (4 * d)) & 0xf][(TypeII[i][5][a][state[5]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_II[i][a][d][3][(TypeII[i][6][a][state[6]] >> (4 * d)) & 0xf][(TypeII[i][7][a][state[7]] >> (4 * d)) & 0xf]
                ]
            ] << 6);
        }
        for (ab = 0; ab < 8; ++ab)
        {
            state[ab] = tmp_state[ab];
        }

        for (j = 0; j < 8; j++)
        {
            a = j / 4; // column index
            b = 3 - (j % 4); // row index
            c = b * 2; // 
            d = b * 2 + 1;
            tmp_state[j] = 
            (TypeIV_III[i][a][c][6]
            [
                TypeIV_III[i][a][c][4]
                [
                    TypeIV_III[i][a][c][0][(TypeIII[i][0][a][state[0]] >> (4 * c)) & 0xf][(TypeIII[i][1][a][state[1]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_III[i][a][c][1][(TypeIII[i][2][a][state[2]] >> (4 * c)) & 0xf][(TypeIII[i][3][a][state[3]] >> (4 * c)) & 0xf]
                ]
            ]
            [
                TypeIV_III[i][a][c][5]
                [
                    TypeIV_III[i][a][c][2][(TypeIII[i][4][a][state[4]] >> (4 * c)) & 0xf][(TypeIII[i][5][a][state[5]] >> (4 * c)) & 0xf]
                ]
                [
                    TypeIV_III[i][a][c][3][(TypeIII[i][6][a][state[6]] >> (4 * c)) & 0xf][(TypeIII[i][7][a][state[7]] >> (4 * c)) & 0xf]
                ]
            ]) | 
            (TypeIV_III[i][a][d][6]
            [
                TypeIV_III[i][a][d][4]
                [
                    TypeIV_III[i][a][d][0][(TypeIII[i][0][a][state[0]] >> (4 * d)) & 0xf][(TypeIII[i][1][a][state[1]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_III[i][a][d][1][(TypeIII[i][2][a][state[2]] >> (4 * d)) & 0xf][(TypeIII[i][3][a][state[3]] >> (4 * d)) & 0xf]
                ]
            ]
            [
                TypeIV_III[i][a][d][5]
                [
                    TypeIV_III[i][a][d][2][(TypeIII[i][4][a][state[4]] >> (4 * d)) & 0xf][(TypeIII[i][5][a][state[5]] >> (4 * d)) & 0xf]
                ]
                [
                    TypeIV_III[i][a][d][3][(TypeIII[i][6][a][state[6]] >> (4 * d)) & 0xf][(TypeIII[i][7][a][state[7]] >> (4 * d)) & 0xf]
                ]
            ] << 6);
        }
        for (ab = 0; ab < 8; ++ab)
        {
            state[ab] = tmp_state[ab];
        }
    }
    
    //Round R
    for (j = 0; j < 8; j++)
    {
        a = j / 4; // column index
        b = 3 - (j % 4); // row index
        c = b * 2; // 
        d = b * 2 + 1;
        output[j] = 
        (TypeIV_Ib[a][c][6]
        [
            TypeIV_Ib[a][c][4]
            [
                TypeIV_Ib[a][c][0][(TypeIb[0][a][state[0]] >> (4 * c)) & 0xf][(TypeIb[1][a][state[1]] >> (4 * c)) & 0xf]
            ]
            [
                TypeIV_Ib[a][c][1][(TypeIb[2][a][state[2]] >> (4 * c)) & 0xf][(TypeIb[3][a][state[3]] >> (4 * c)) & 0xf]
            ]
        ]
        [
            TypeIV_Ib[a][c][5]
            [
                TypeIV_Ib[a][c][2][(TypeIb[4][a][state[4]] >> (4 * c)) & 0xf][(TypeIb[5][a][state[5]] >> (4 * c)) & 0xf]
            ]
            [
                TypeIV_Ib[a][c][3][(TypeIb[6][a][state[6]] >> (4 * c)) & 0xf][(TypeIb[7][a][state[7]] >> (4 * c)) & 0xf]
            ]
        ]) | 
        (TypeIV_Ib[a][d][6]
        [
            TypeIV_Ib[a][d][4]
            [
                TypeIV_Ib[a][d][0][(TypeIb[0][a][state[0]] >> (4 * d)) & 0xf][(TypeIb[1][a][state[1]] >> (4 * d)) & 0xf]
            ]
            [
                TypeIV_Ib[a][d][1][(TypeIb[2][a][state[2]] >> (4 * d)) & 0xf][(TypeIb[3][a][state[3]] >> (4 * d)) & 0xf]
            ]
        ]
        [
            TypeIV_Ib[a][d][5]
            [
                TypeIV_Ib[a][d][2][(TypeIb[4][a][state[4]] >> (4 * d)) & 0xf][(TypeIb[5][a][state[5]] >> (4 * d)) & 0xf]
            ]
            [
                TypeIV_Ib[a][d][3][(TypeIb[6][a][state[6]] >> (4 * d)) & 0xf][(TypeIb[7][a][state[7]] >> (4 * d)) & 0xf]
            ]
        ] << 4);
    }
}