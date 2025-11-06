#ifndef GENTABLES_H
#define GENTABLES_H

#include "slt.h"
#include "WBMatrix/WBMatrix.h"

extern u32**** TypeII;
extern u32**** TypeIII;
extern u16****** TypeIV_II;
extern u16****** TypeIV_III;
extern u32*** TypeIa;
extern u32*** TypeIb;

extern M128 IDM128;
extern M128 ODM128;

extern M64 IDM64;
extern M64 ODM64;

void init_table(int R, int block_size);

void general_wbslt128_gen(u8 key[16], int R);
void wbslt128_encrypt(u8 input[16], u8 output[16], int R);

void general_wbslt64_gen(u8 key[8], int R);
void wbslt64_encrypt(u8 input[8], u8 output[8], int R);

void printstate(unsigned char * in);

#endif // GENTABLES_H


// u32 TypeII[9][8][2][4096];
// u32 TypeIII[9][8][2][4096];
// u32 TypeIa[8][2][256];
// u32 TypeIb[8][2][4096];

// u16* TypeIV_Ia[2][8][7][16][16];
// u8* TypeIV_Ib[2][8][7][16][16];
// u16 TypeIV_II[9][2][8][7][16][16];
// u16 TypeIV_III[9][2][8][7][16][16];