#include "wbaes.h"

void encryption_evaluation(int ROUND, int BLOCK_SIZE)
{
    unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    u8 key[16] = {0};
    if (BLOCK_SIZE == 64)
    {
        general_wbslt64_gen(key, ROUND);
        
        clock_t start = clock();

        for (int i = 0; i < 1024 * 1024 / 16 * 1000; ++i)
        {
            wbslt64_encrypt(IN, IN, ROUND);
        }
        
        clock_t duration = clock() - start;

        printf("%.4lf\n", 1000.0 / ((double)duration / CLOCKS_PER_SEC));
    } else if (BLOCK_SIZE == 128)
    {
        general_wbslt128_gen(key, ROUND);
        
        clock_t start = clock();

        for (int i = 0; i < 1024 * 1024 / 16 * 1000; ++i)
        {
            wbslt128_encrypt(IN, IN, ROUND);
        }
        
        clock_t duration = clock() - start;

        printf("%.4lf\n", 1000.0 / ((double)duration / CLOCKS_PER_SEC));
    }
}

void tablegen_evaluation(int ROUND, int BLOCK_SIZE)
{
    u8 key[16] = {0};
    
    clock_t start = clock();
    if (BLOCK_SIZE == 64)
    {
        for (int i = 0; i < 1000; ++i)
        {
            general_wbslt64_gen(key, ROUND);
        }
    } else if (BLOCK_SIZE == 128)
    {
        for (int i = 0; i < 1000; ++i)
        {
            general_wbslt128_gen(key, ROUND);
        }
    }
    
    clock_t duration = clock() - start;

    printf("%.4lf\n", (double)duration / CLOCKS_PER_SEC / 1000.0);
}

int main(int argc, char * argv[])
{

    int ROUND = 10;  // Default value for -r
    int BLOCK_SIZE = 128;  // Default value for -n

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {  // Ensure there is a value after -r
                ROUND = atoi(argv[i + 1]);  // Convert the string to an integer
                i++;  // Skip the next argument since it's already processed
            } else {
                fprintf(stderr, "Error: Missing argument for -r\n");
                return 1;
            }
        } 
        else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {  // Ensure there is a value after -n
                BLOCK_SIZE = atoi(argv[i + 1]);  // Convert the string to an integer
                i++;  // Skip the next argument since it's already processed
            } else {
                fprintf(stderr, "Error: Missing argument for -n\n");
                return 1;
            }
        } 
        else {
            // Handle unknown arguments if necessary
            fprintf(stderr, "Error: Unknown argument %s\n", argv[i]);
            return 1;
        }
    }

    slt_init();
    init_table(ROUND, BLOCK_SIZE);

    tablegen_evaluation(ROUND, BLOCK_SIZE);
    encryption_evaluation(ROUND, BLOCK_SIZE);
    
    

    

    
    // int i;
    // unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    // unsigned char EX_IN[16];
    // unsigned char EX_OUT[16];
    // unsigned char OUT[16];
    // unsigned char OOUT[16];
    // u8 key[16] = {0};

    // int ROUND = 10;

    // slt_init();
    // init_table(ROUND, 64);

    // general_wbslt64_gen(key, ROUND);

    // // V128 v128;
    // // v128.V[0] = (u64)IN[7] | ((u64)IN[6] << 8) | ((u64)IN[5] << 16) | ((u64)IN[4] << 24) | ((u64)IN[3] << 32) | ((u64)IN[2] << 40) | ((u64)IN[1] << 48) | ((u64)IN[0] << 56);
    // // v128.V[1] = (u64)IN[15] | ((u64)IN[14] << 8) | ((u64)IN[13] << 16) | ((u64)IN[12] << 24) | ((u64)IN[11] << 32) | ((u64)IN[10] << 40) | ((u64)IN[9] << 48) | ((u64)IN[8] << 56);
    // // MatMulVecM128(IDM128, v128, &v128);
    // // for (int i = 0; i < 8; ++i)
    // // {
    // //     EX_IN[7 - i] = (v128.V[0] >> (8*i)) & 0xff;
    // // }
    // // for (int i = 0; i < 8; ++i)
    // // {
    // //     EX_IN[7 - i + 8] = (v128.V[1] >> (8*i)) & 0xff;
    // // }

    // V64 v64;
    // v64.V = (u64)IN[7] | ((u64)IN[6] << 8) | ((u64)IN[5] << 16) | ((u64)IN[4] << 24) | ((u64)IN[3] << 32) | ((u64)IN[2] << 40) | ((u64)IN[1] << 48) | ((u64)IN[0] << 56);
    // MatMulVecM64(IDM64, v64, &v64);
    // for (int i = 0; i < 8; ++i)
    // {
    //     EX_IN[7 - i] = (v64.V >> (8*i)) & 0xff;
    // }
    // // encryption
    // wbslt64_encrypt(EX_IN, EX_OUT, ROUND);

    // // v128.V[0] = (u64)EX_OUT[7] | ((u64)EX_OUT[6] << 8) | ((u64)EX_OUT[5] << 16) | ((u64)EX_OUT[4] << 24) | ((u64)EX_OUT[3] << 32) | ((u64)EX_OUT[2] << 40) | ((u64)EX_OUT[1] << 48) | ((u64)EX_OUT[0] << 56);
    // // v128.V[1] = (u64)EX_OUT[15] | ((u64)EX_OUT[14] << 8) | ((u64)EX_OUT[13] << 16) | ((u64)EX_OUT[12] << 24) | ((u64)EX_OUT[11] << 32) | ((u64)EX_OUT[10] << 40) | ((u64)EX_OUT[9] << 48) | ((u64)EX_OUT[8] << 56);
    // // MatMulVecM128(ODM128, v128, &v128);
    // // for (int i = 0; i < 8; ++i)
    // // {
    // //     OUT[7 - i] = (v128.V[0] >> (8*i)) & 0xff;
    // // }
    // // for (int i = 0; i < 8; ++i)
    // // {
    // //     OUT[7 - i + 8] = (v128.V[1] >> (8*i)) & 0xff;
    // // }


    // v64.V = (u64)EX_OUT[7] | ((u64)EX_OUT[6] << 8) | ((u64)EX_OUT[5] << 16) | ((u64)EX_OUT[4] << 24) | ((u64)EX_OUT[3] << 32) | ((u64)EX_OUT[2] << 40) | ((u64)EX_OUT[1] << 48) | ((u64)EX_OUT[0] << 56);
    // MatMulVecM64(ODM64, v64, &v64);
    // for (int i = 0; i < 8; ++i)
    // {
    //     OUT[7 - i] = (v64.V >> (8*i)) & 0xff;
    // }

    // printstate(OUT);
    
    // slt_encrypt(IN, key, OOUT, 8, ROUND);
    // printstate(OOUT);

    return 0;
}
