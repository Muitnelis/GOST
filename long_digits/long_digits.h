#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct BigInt;

void print_BigInt(BigInt* bigint);

BigInt* BigInt_init(int size);

void update_bits(BigInt* bigint);

void update_size(BigInt* bigint);

void update_options(BigInt* bigint);

BigInt* string_to_BigInt512(char* number);

void left_shift(BigInt* destination, BigInt* source, int N);

void right_shift(BigInt* destination, BigInt* source, int N);

int cmp_BigInt(BigInt* a, BigInt* b);

void sub_BigInt(BigInt* a, BigInt* b, BigInt* destination);

void add_BigInt(BigInt* first, BigInt* second, BigInt* destination);

void inc_BigInt(BigInt* a, BigInt* destination);

void mod_BigInt(BigInt* a_, BigInt* b, BigInt* destination);

void mul_BigInt(BigInt* a, BigInt* b, BigInt* destination);

void inv_by_mod(BigInt* y, BigInt* p, BigInt* destination);

void add_mod_p(BigInt* x, BigInt* y, BigInt* p, BigInt* destination);

void sub_mod_p(BigInt* x_, BigInt* y_, BigInt* p, BigInt* destination);

void mul_mod_p(BigInt* x, BigInt* y, BigInt* p, BigInt* destination);