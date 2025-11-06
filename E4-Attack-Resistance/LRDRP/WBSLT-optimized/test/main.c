#include "wbaes.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h> // for memset
#include <stdbool.h>

typedef uint8_t byte;

// AES S-box
byte lrdrp_sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// AES inverse S-box
byte inv_sbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

// GF(256) multiplication
byte gf_mul(byte a, byte b) {
  byte p = 0;
  for (int i = 0; i < 8; i++) {
    if (b & 1) p ^= a;
    byte hi = a & 0x80;
    a <<= 1;
    if (hi) a ^= 0x1b;
    b >>= 1;
  }
  return p;
}

// GF(256) inverse
byte gf_inv(byte a) {
  if (a == 0) return 0;
  for (int i = 1; i < 256; i++) if (gf_mul(a, i) == 1) return i;
  return 0;
}

// MixColumns coefficients
byte mc[4][4] = {
  {0x02, 0x03, 0x01, 0x01},
  {0x01, 0x02, 0x03, 0x01},
  {0x01, 0x01, 0x02, 0x03},
  {0x03, 0x01, 0x01, 0x02}
};

// Test key bytes for the subround (example, can be changed)
byte k[4] = {0x00, 0x11, 0x22, 0x33};

int mask = 0x7;

// Function to compute the l-th output byte of the encoded subround (for test, with identity encodings)
byte fprime(int l, byte x0, byte x1, byte x2, byte x3) {
  x0 = (((x0 >> 4) ^ (x0 & 0xf)) << 4) | (x0 & 0xf);
  x1 = (((x1 >> 4) ^ (x1 & 0xf)) << 4) | (x1 & 0xf);
  x2 = (((x2 >> 4) ^ (x2 & 0xf)) << 4) | (x2 & 0xf);
  x3 = (((x3 >> 4) ^ (x3 & 0xf)) << 4) | (x3 & 0xf);
  byte s0 = lrdrp_sbox[x0 ^ k[0]];
  byte s1 = lrdrp_sbox[x1 ^ k[1]];
  byte s2 = lrdrp_sbox[x2 ^ k[2]];
  byte s3 = lrdrp_sbox[x3 ^ k[3]];
  byte y = gf_mul(mc[l][0], s0) ^ gf_mul(mc[l][1], s1) ^ gf_mul(mc[l][2], s2) ^ gf_mul(mc[l][3], s3);
  y = ((y >> 4) << 8) | (((y & 0xf) ^ mask) << 4) | (y & 0xf);
  return y;
}

int main() {

  u8 key[16] = {0};
  optimized_wbaes_gen(key);

  // Step 1: Precompute left and right for collisions on S0 and S1
  byte left[4][256];
  byte right[4][256];
  for (int a = 0; a < 256; a++) {
    for (int l = 0; l < 4; l++) {
      left[l][a] = fprime(l, a, 0, 0, 0);
    }
  }
  for (int b = 0; b < 256; b++) {
    for (int l = 0; l < 4; l++) {
      right[l][b] = fprime(l, 0, b, 0, 0);
    }
  }

  // Step 2: Find the matching b for each a and l for collisions
  byte beta[4][256];
  for (int l = 0; l < 4; l++) {
    byte inv_right[256];
    for (int b = 0; b < 256; b++) inv_right[right[l][b]] = b;
    for (int a = 1; a < 256; a++) {
      byte b = inv_right[left[l][a]];
      if (b == 0) {
        printf("Error: invalid collision b=0 for a=%d l=%d\n", a, l);
        return 1;
      }
      beta[l][a] = b;
    }
  }

  // Step 3: Set up the augmented matrix for the linear system (homogeneous except for added equation)
  byte aug[1021][511];
  memset(aug, 0, sizeof(aug));
  int row = 0;
  for (int l = 0; l < 4; l++) {
    byte m0 = mc[l][0];
    byte m1 = mc[l][1];
    for (int a = 1; a < 256; a++) {
      int ua_idx = a - 1;
      int vb_idx = 255 + (beta[l][a] - 1);
      aug[row][ua_idx] = m0;
      aug[row][vb_idx] = m1;
      // constant remains 0
      row++;
    }
  }
  // Add equation for u'1 = 1 (var 0 = 1)
  aug[1020][0] = 1;
  aug[1020][510] = 1;

  // Step 4: Gaussian elimination over GF(256)
  int rank = 0;
  for (int c = 0; c < 510; c++) {
    int pr = -1;
    for (int r = rank; r < 1021; r++) {
      if (aug[r][c] != 0) {
        pr = r;
        break;
      }
    }
    if (pr == -1) continue;

    // Swap rows
    if (pr != rank) {
      for (int k = 0; k <= 510; k++) {
        byte tmp = aug[rank][k];
        aug[rank][k] = aug[pr][k];
        aug[pr][k] = tmp;
      }
    }

    // Normalize pivot
    byte inv = gf_inv(aug[rank][c]);
    for (int k = c; k <= 510; k++) {
      aug[rank][k] = gf_mul(aug[rank][k], inv);
    }

    // Eliminate
    for (int r = 0; r < 1021; r++) {
      if (r == rank) continue;
      byte f = aug[r][c];
      for (int k = c; k <= 510; k++) {
        aug[r][k] ^= gf_mul(f, aug[rank][k]);
      }
    }
    rank++;
  }

  // Check for consistency
  for (int r = rank; r < 1021; r++) {
    if (aug[r][510] != 0) {
      printf("System inconsistent\n");
      return 1;
    }
  }
  if (rank != 510) {
    printf("Unexpected rank %d\n", rank);
    return 1;
  }

  // Extract coefficients a and b
  byte a[256];
  a[0] = 0; // not used
  for (int i = 1; i < 256; i++) a[i] = aug[i - 1][510];

  byte b[256];
  b[0] = 0;
  for (int i = 1; i < 256; i++) b[i] = aug[255 + i - 1][510];

  // Step 5: Exhaustive search for S0 (u0, u1)
  byte S0[256];
  byte delta = 0;
  bool found = false;
  for (int u0 = 0; u0 < 256 && !found; u0++) {
    for (int u1 = 0; u1 < 256 && !found; u1++) {
      byte temp_delta = u0 ^ u1;
      if (temp_delta == 0) continue;

      byte temp_S0[256];
      temp_S0[0] = u0;
      for (int i = 1; i < 256; i++) temp_S0[i] = gf_mul(a[i], temp_delta) ^ u0;

      byte g[256];
      for (int x = 0; x < 256; x++) g[x] = inv_sbox[temp_S0[x]];

      bool good = true;
      for (int chk = 0; chk < 16; chk++) {
        byte xx = chk * 16;
        byte phi = 0;
        for (int alpha = 0; alpha < 16; alpha++) phi ^= g[xx ^ alpha];
        if (phi != 0) {
          good = false;
          break;
        }
      }

      if (good) {
        found = true;
        delta = temp_delta;
        memcpy(S0, temp_S0, 256);
      }
    }
  }
  if (!found) {
    printf("S0 not found\n");
    return 1;
  }

  // Step 6: Exhaustive search for S1 (v0)
  byte S1[256];
  found = false;
  for (int v0 = 0; v0 < 256; v0++) {
    byte temp_S1[256];
    temp_S1[0] = v0;
    for (int i = 1; i < 256; i++) temp_S1[i] = gf_mul(b[i], delta) ^ v0;

    byte g[256];
    for (int x = 0; x < 256; x++) g[x] = inv_sbox[temp_S1[x]];

    bool good = true;
    for (int chk = 0; chk < 16; chk++) {
      byte xx = chk * 16;
      byte phi = 0;
      for (int alpha = 0; alpha < 16; alpha++) phi ^= g[xx ^ alpha];
      if (phi != 0) {
        good = false;
        break;
      }
    }

    if (good) {
      found = true;
      memcpy(S1, temp_S1, 256);
      break;
    }
  }
  if (!found) {
    printf("S1 not found\n");
    return 1;
  }

  // Step 7: Recover S2 using collisions with S0 (positions 0 and 2)
  byte right2[4][256];
  for (int b = 0; b < 256; b++) {
    for (int l = 0; l < 4; l++) right2[l][b] = fprime(l, 0, 0, b, 0);
  }

  byte beta2[4][256];
  for (int l = 0; l < 4; l++) {
    byte inv_right[256];
    for (int b = 0; b < 256; b++) inv_right[right2[l][b]] = b;
    for (int a = 1; a < 256; a++) {
      byte b = inv_right[left[l][a]];
      beta2[l][a] = b;
    }
  }

  byte vprime[256];
  memset(vprime, 0, sizeof(vprime));
  vprime[0] = 0;
  for (int l = 0; l < 4; l++) {
    for (int a = 1; a < 256; a++) {
      byte b = beta2[l][a];
      byte ua = S0[a] ^ S0[0];
      byte m0 = mc[l][0];
      byte m2 = mc[l][2];
      byte val = gf_mul(gf_inv(m2), gf_mul(m0, ua));
      if (vprime[b] == 0 || vprime[b] == val) {
        vprime[b] = val;
      } else {
        printf("Inconsistent v' for S2 b=%d l=%d\n", b, l);
        return 1;
      }
    }
  }

  // Exhaustive search for S2 (v0)
  byte S2[256];
  found = false;
  for (int v0 = 0; v0 < 256; v0++) {
    byte temp_S2[256];
    temp_S2[0] = v0;
    for (int i = 1; i < 256; i++) temp_S2[i] = vprime[i] ^ v0;

    byte g[256];
    for (int x = 0; x < 256; x++) g[x] = inv_sbox[temp_S2[x]];

    bool good = true;
    for (int chk = 0; chk < 16; chk++) {
      byte xx = chk * 16;
      byte phi = 0;
      for (int alpha = 0; alpha < 16; alpha++) phi ^= g[xx ^ alpha];
      if (phi != 0) {
        good = false;
        break;
      }
    }

    if (good) {
      found = true;
      memcpy(S2, temp_S2, 256);
      break;
    }
  }
  if (!found) {
    printf("S2 not found\n");
    return 1;
  }

  // Step 8: Recover S3 using collisions with S0 (positions 0 and 3)
  byte right3[4][256];
  for (int b = 0; b < 256; b++) {
    for (int l = 0; l < 4; l++) right3[l][b] = fprime(l, 0, 0, 0, b);
  }

  byte beta3[4][256];
  for (int l = 0; l < 4; l++) {
    byte inv_right[256];
    for (int b = 0; b < 256; b++) inv_right[right3[l][b]] = b;
    for (int a = 1; a < 256; a++) {
      byte b = inv_right[left[l][a]];
      beta3[l][a] = b;
    }
  }

  vprime[0] = 0;
  memset(vprime, 0, sizeof(vprime));
  for (int l = 0; l < 4; l++) {
    for (int a = 1; a < 256; a++) {
      byte b = beta3[l][a];
      byte ua = S0[a] ^ S0[0];
      byte m0 = mc[l][0];
      byte m3 = mc[l][3];
      byte val = gf_mul(gf_inv(m3), gf_mul(m0, ua));
      if (vprime[b] == 0 || vprime[b] == val) {
        vprime[b] = val;
      } else {
        printf("Inconsistent v' for S3 b=%d l=%d\n", b, l);
        return 1;
      }
    }
  }

  // vprime[0] = 0;
  // for (int l = 0; l < 4; l++) {
  //   for (int a = 1; a < 256; a++) {
  //     byte b = beta3[l][a];
  //     byte ua = S0[a] ^ S0[0];
  //     byte m0 = mc[l][0];
  //     byte m3 = mc[l][3];
  //     byte val = gf_mul(gf_inv(m3), gf_mul(m0, ua));
  //     if (vprime[b] == 0 || vprime[b] == val) {
  //       vprime[b] = val;
  //     }
  //   }
  // }

  // Exhaustive search for S3 (v0)
  byte S3[256];
  found = false;
  for (int v0 = 0; v0 < 256; v0++) {
    byte temp_S3[256];
    temp_S3[0] = v0;
    for (int i = 1; i < 256; i++) temp_S3[i] = vprime[i] ^ v0;

    byte g[256];
    for (int x = 0; x < 256; x++) g[x] = inv_sbox[temp_S3[x]];

    bool good = true;
    for (int chk = 0; chk < 16; chk++) {
      byte xx = chk * 16;
      byte phi = 0;
      for (int alpha = 0; alpha < 16; alpha++) phi ^= g[xx ^ alpha];
      if (phi != 0) {
        good = false;
        break;
      }
    }

    if (good) {
      found = true;
      memcpy(S3, temp_S3, 256);
      break;
    }
  }
  if (!found) {
    printf("S3 not found\n");
    return 1;
  }

  // Step 9: Recover the key bytes (for test setup with identity encodings)
  byte recovered_k[4];
  recovered_k[0] = inv_sbox[ S0[0] ];
  recovered_k[1] = inv_sbox[ S1[0] ];
  recovered_k[2] = inv_sbox[ S2[0] ];
  recovered_k[3] = inv_sbox[ S3[0] ];
  
  for (int i = 0; i < 4; ++i) {
    if (recovered_k[i] != k[i]) {
      printf("fail\n");
      return 1;
    }
  }
  // printf("Actual key bytes: %02x %02x %02x %02x\n", k[0], k[1], k[2], k[3]);
  // printf("Recovered key bytes: %02x %02x %02x %02x\n", recovered_k[0], recovered_k[1], recovered_k[2], recovered_k[3]);

  printf("success\n");

  return 0;
}