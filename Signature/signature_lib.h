#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "streebog_lib.h"
#include "long_digits.h"

extern int bytes;

typedef struct
{
    BigInt p;
    BigInt a;
    BigInt b;
    BigInt m;
    BigInt q;
} curva;

curva curva_init(BigInt* p, BigInt* a, BigInt* b, BigInt* m, BigInt* q);

typedef struct
{
    BigInt x;
    BigInt y;
} point;

point point_init(BigInt* x, BigInt* y);

void copy_point(point* destination, point* source);

void add_points(curva* E, point* P1, point* P2, point* P3);

void mul_point(curva* E, point* P, BigInt* k, point* Q);

void random(BigInt* max, BigInt* destination);

void sign(char* msg, curva* E, point* P, BigInt* d);

void verify(char* msg, char* sign, curva* E, point* P, BigInt* d);