#ifndef GENTABLES_H
#define GENTABLES_H

#include "aes.h"
#include "WBMatrix/WBMatrix.h"

extern u32 TypeII[10][16][4096];//Type II
extern u32 TypeIII[9][16][4096];//Type III
extern u16 TypeIV_II[9][4][3][8][16][16];
extern u16 TypeIV_III[9][4][3][8][16][16];
extern u32 TypeIa[16][4][256];
extern u32 TypeIb[16][4][4096];
extern M128 IDM;
extern M128 ODM;

void optimized_wbaes_gen(u8 key[16]);
void general_wbaes_gen(u8 key[16]);
void wbaes_encrypt(u8 input[16], u8 output[16]);
void printstate(unsigned char * in);

#endif // GENTABLES_H