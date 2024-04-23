#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define bytes_count 8
#define half_block 4

extern unsigned char pi[][16];

void print_block(unsigned char* block, int len);

void X_block(unsigned char* block, unsigned char* key, unsigned char bytes);

void key_schedule(unsigned char* keys, unsigned char* key_file_path);

void t_draft(unsigned char* block);

void Vec32plusVec32(unsigned char* Vec1, unsigned char* Vec2);

void shift(unsigned char* destination, unsigned char* source, int N);

void circle_shift(unsigned char* block, int N);

void g_draft(unsigned char* key, unsigned char* block);

void G(unsigned char* key, unsigned char* block);

void G_star(unsigned char* key, unsigned char* block);

void Magma_ENC(unsigned char* block, unsigned char* keys);

void Magma_DEC(unsigned char* block, unsigned char* keys);

void complete(unsigned char* block, size_t len);

void input_IV(unsigned char* IV_path, unsigned char* IV);

void next_IV(unsigned char* IV);