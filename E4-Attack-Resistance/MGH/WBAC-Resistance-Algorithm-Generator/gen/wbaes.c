#include "./table/table.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const unsigned char kShiftTab[16] = 
{
  0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, 1, 6, 11
};

void ShiftRows(unsigned int *state) {
  unsigned int temp[16];
  for (int i = 0; i < 16; ++i) {
    temp[i] = state[i];
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[kShiftTab[i]];
  }
}

void Encrypt(unsigned int *state) {
  unsigned int temp[16][32];
  // idm
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_i0[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv0[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv0[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[15][i*2]*offset0 + temp[15][i*2+1];
  }
  int r, c;

  // round 1
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii1[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv1a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv1a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset1a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii1[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv1b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv1b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset1b + temp[3][i*2+1];
  }

  // round 2
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii2[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv2a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv2a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset2a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii2[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv2b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv2b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset2b + temp[3][i*2+1];
  }

  // round 3
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii3[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv3a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv3a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset3a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii3[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv3b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv3b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset3b + temp[3][i*2+1];
  }
  
  // round 4
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii4[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv4a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv4a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset4a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii4[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv4b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv4b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset4b + temp[3][i*2+1];
  }

  // round 5
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii5[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv5a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv5a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset5a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii5[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv5b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv5b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset5b + temp[3][i*2+1];
  }

  // round 6
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii6[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv6a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv6a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset6a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii6[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv6b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv6b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset6b + temp[3][i*2+1];
  }

  // round 7
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii7[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv7a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv7a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset7a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii7[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv7b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv7b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset7b + temp[3][i*2+1];
  }

  // round 8
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii8[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv8a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv8a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset8a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii8[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv8b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv8b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset8b + temp[3][i*2+1];
  }

  // round 9
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_ii9[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv9a[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv9a[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset9a + temp[3][i*2+1];
  }
  for (int i = 0; i < 16; ++i) {
    r = i % 4, c = i / 4;
    for (int j = 0; j < 8; ++j) {
      temp[r][c*8+j] = type_iii9[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv9b[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv9b[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[3][i*2]*offset9b + temp[3][i*2+1];
  }

  // round 10 with odm
  ShiftRows(state);
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_i10[i][state[i]][j];
    }
  }
  for (int i = 0; i < 32; ++i) {
    temp[1][i] = type_iv10[0][i][temp[0][i]][temp[1][i]];
  }
  for (int i = 2; i < 16; ++i) {
    for (int j = 0; j < 32; ++j) {
      temp[i][j] = type_iv10[i-1][j][temp[i-1][j]][temp[i][j]];
    }
  }
  for (int i = 0; i < 16; ++i) {
    state[i] = temp[15][i*2]*offset10 + temp[15][i*2+1];
  }
}

unsigned int fsize(FILE *fp){
  unsigned int n;
  fpos_t fpos;
  fgetpos(fp, &fpos);
  fseek(fp, 0, SEEK_END);
  n = ftell(fp);
  fsetpos(fp,&fpos);
  return n;
}

void PrintState(unsigned int *state) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      printf("%.2x ", state[i*4+j]);
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {
  unsigned int state[16];
  for (int i = 0; i < 16; ++i) state[i] = i;
  Encrypt(state);
  PrintState(state);
  return 0;
}