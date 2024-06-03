#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

extern unsigned char pi[8][16];

void print_block(unsigned char* block, int len);

void X_block(unsigned char* block, unsigned char* key, unsigned char bytes);

void key_schedule(unsigned char* keys, char* key_file_path);

void t_draft(unsigned char* block);

void Vec32plusVec32(unsigned char Vec1[4], unsigned char Vec2[4]);

void shift(unsigned char* destination, unsigned char* source, int N);

void circle_shift(unsigned char block[4], int N);

void g_draft(unsigned char* key, unsigned char* block);

void G(unsigned char key[4], unsigned char block[8]);

void G_star(unsigned char key[4], unsigned char block[8]);

void Magma_ENC(unsigned char block[4], unsigned char keys[32][4]);

void Magma_DEC(unsigned char block[4], unsigned char keys[32][4]);

void complete(unsigned char* block, size_t len);

void input_IV(char* IV_path, unsigned char IV[8]);

void next_IV(unsigned char IV[8]);