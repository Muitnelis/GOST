#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

//----------------------------

void print_block(unsigned char* block, int len);

void X_block(unsigned char* block, unsigned char* key, unsigned char bytes);

//----------------------------

extern unsigned char S[256];
extern unsigned char S_inv[256];
extern unsigned char pow2[256];
extern unsigned char c_const[32][16];

void S_block(unsigned char block[16], unsigned char substitution[256]);

unsigned char galua_mul(unsigned char a, unsigned char b);

unsigned char l_draft(unsigned char a[16]);

void R_block(unsigned char block[16]);

void L_block(unsigned char block[16]);

void Feistel(unsigned char block[32], unsigned char key[16]);

void key_schedule_Kuznechik(unsigned char keys[10][16], unsigned char* key_file_path);

void Kuznechik_ENC(unsigned char block[16], unsigned char keys[10][16]);

void R_block_inv(unsigned char block[16]);

void L_block_inv(unsigned char block[16]);

void Kuznechik_DEC(unsigned char block[16], unsigned char keys[10][16]);

//----------------------------

extern unsigned char pi[8][16];

void key_schedule_Magma(unsigned char keys[32][4], char* key_file_path);

void t_draft(unsigned char block[4]);

void Vec32plusVec32(unsigned char Vec1[4], unsigned char Vec2[4]);

void circle_shift(unsigned char block[4], int N);

void g_draft(unsigned char key[4], unsigned char block[4]);

void G(unsigned char key[4], unsigned char block[8]);

void G_star(unsigned char key[4], unsigned char block[8]);

void Magma_ENC(unsigned char block[4], unsigned char keys[32][4]);

void Magma_DEC(unsigned char block[4], unsigned char keys[32][4]);