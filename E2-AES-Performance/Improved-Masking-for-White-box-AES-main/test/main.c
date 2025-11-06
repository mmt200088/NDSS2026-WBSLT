#include "wbaes.h"

void encryption_evaluation()
{
    unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    u8 key[16] = {0};
    
    wbaes_gen(key); 
    
    clock_t start = clock();

    for (int i = 0; i < 1024 * 1024 / 16 * 1000; ++i)
    {
        wbaes_encrypt(IN, IN);
    }
    
    clock_t duration = clock() - start;

    printf("%.4lf\n", 1000.0 / ((double)duration / CLOCKS_PER_SEC));
}

void tablegen_evaluation()
{
    u8 key[16] = {0};
    
    clock_t start = clock();

    for (int i = 0; i < 1000; ++i)
    {
        wbaes_gen(key);
    }
    
    clock_t duration = clock() - start;

    printf("%.4lf\n", (double)duration / CLOCKS_PER_SEC / 1000.0);
}

int main(int argc, char * argv[])
{
    tablegen_evaluation();
    encryption_evaluation();

    // unsigned char OUT[16];
    // unsigned char OOUT[16];
    // u8 key[16] = {0};
    
    // unsigned char IN[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};

    // wbaes_gen(key);
    // wbaes_encrypt(IN, OUT);
    // printstate(OUT);

    // aes_128_encrypt(IN, key, OOUT);
    // printstate(OOUT);
    return 0;
}
