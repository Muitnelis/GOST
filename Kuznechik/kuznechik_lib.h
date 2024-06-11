#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define bytes_count 16

extern unsigned char S[];
extern unsigned char S_inv[];
extern unsigned char pow2[];
extern unsigned char c_const[][16];

void print_block(unsigned char* block, int len);

void S_block(unsigned char* block, unsigned char* substitution);

unsigned char galua_mul(a, b);

unsigned char l_draft(unsigned char* a);

void R_block(unsigned char* block);

void L_block(unsigned char* block);

void X_block(unsigned char* block, unsigned char* key, unsigned char bytes);

void Feistel(unsigned char* block, unsigned char* key, int bytes);

void key_schedule(unsigned char* keys, unsigned char* key_file_path);

void Kuznechik_ENC(unsigned char* block, unsigned char* keys);

void complete(unsigned char* block, size_t len);

void R_inv(unsigned char* block);

void L_inv(unsigned char* block);

void Kuznechik_DEC(unsigned char* block, unsigned char* keys);

void shift(unsigned char* destination, unsigned char* source, int N);

void input_IV(char* IV_path, unsigned char IV[16]);

void next_IV(unsigned char IV[16]);