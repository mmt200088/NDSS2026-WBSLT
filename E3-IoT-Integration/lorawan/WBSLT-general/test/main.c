#include "wbaes.h"

void encryption_evaluation()
{
    unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    u8 key[16] = {0};
    
    general_wbaes_gen(key); 
    
    clock_t start = clock();

    for (int i = 0; i < 1024 * 1024 / 16 * 1000; ++i)
    {
        wbaes_encrypt(IN, IN);
    }
    
    clock_t duration = clock() - start;

    // printf("Enc speed: %.4lf MB/s \n", 1.0 / ((double)duration / CLOCKS_PER_SEC / 10.0));
    printf("%.4lf\n", 1000.0 / ((double)duration / CLOCKS_PER_SEC));
}

void tablegen_evaluation()
{
    u8 key[16] = {0};
    
    clock_t start = clock();

    for (int i = 0; i < 10; ++i)
    {
        general_wbaes_gen(key);
    }
    
    clock_t duration = clock() - start;

    // printf("Table generation time: %.4lf s \n", (double)duration / CLOCKS_PER_SEC / 10.0);
    printf("%.4lf\n", (double)duration / CLOCKS_PER_SEC / 1000.0);
}

int main(int argc, char * argv[])
{
    tablegen_evaluation();
    encryption_evaluation();
    // int i;
    // unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    // unsigned char EX_IN[16];
    // unsigned char EX_OUT[16];
    // unsigned char OUT[16];
    // unsigned char OOUT[16];
    // u8 key[16] = {0};
    
    // general_wbaes_gen(key);
    // // external encodings
    // // for(i = 0; i < 16; i++)
    // // {
    // //     EX_IN[i] = TypeIa[i][IN[i]];
    // // }
    // V128 v128;
    // v128.V[0] = (u64)IN[7] | ((u64)IN[6] << 8) | ((u64)IN[5] << 16) | ((u64)IN[4] << 24) | ((u64)IN[3] << 32) | ((u64)IN[2] << 40) | ((u64)IN[1] << 48) | ((u64)IN[0] << 56);
    // v128.V[1] = (u64)IN[15] | ((u64)IN[14] << 8) | ((u64)IN[13] << 16) | ((u64)IN[12] << 24) | ((u64)IN[11] << 32) | ((u64)IN[10] << 40) | ((u64)IN[9] << 48) | ((u64)IN[8] << 56);
    // MatMulVecM128(IDM, v128, &v128);
    // for (int i = 0; i < 8; ++i)
    // {
    //     EX_IN[7 - i] = (v128.V[0] >> (8*i)) & 0xff;
    // }
    // for (int i = 0; i < 8; ++i)
    // {
    //     EX_IN[7 - i + 8] = (v128.V[1] >> (8*i)) & 0xff;
    // }
    // // encryption
    // wbaes_encrypt(EX_IN, EX_OUT);
    // // external encodings
    // // for(i = 0; i < 16; i++)
    // // {
    // //     OUT[i] = TypeIb[i][EX_OUT[i]];
    // // }
    // v128.V[0] = (u64)EX_OUT[7] | ((u64)EX_OUT[6] << 8) | ((u64)EX_OUT[5] << 16) | ((u64)EX_OUT[4] << 24) | ((u64)EX_OUT[3] << 32) | ((u64)EX_OUT[2] << 40) | ((u64)EX_OUT[1] << 48) | ((u64)EX_OUT[0] << 56);
    // v128.V[1] = (u64)EX_OUT[15] | ((u64)EX_OUT[14] << 8) | ((u64)EX_OUT[13] << 16) | ((u64)EX_OUT[12] << 24) | ((u64)EX_OUT[11] << 32) | ((u64)EX_OUT[10] << 40) | ((u64)EX_OUT[9] << 48) | ((u64)EX_OUT[8] << 56);
    // MatMulVecM128(ODM, v128, &v128);
    // for (int i = 0; i < 8; ++i)
    // {
    //     OUT[7 - i] = (v128.V[0] >> (8*i)) & 0xff;
    // }
    // for (int i = 0; i < 8; ++i)
    // {
    //     OUT[7 - i + 8] = (v128.V[1] >> (8*i)) & 0xff;
    // }
    // printstate(OUT);
    
    // aes_128_encrypt(IN, key, OOUT);
    // printstate(OOUT);

    return 0;
}
