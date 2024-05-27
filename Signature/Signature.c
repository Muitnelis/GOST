#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "streebog_lib.h"

void print1024(unsigned char a[128])
{
    print_block(a, 32);
    print_block(a + 32 * sizeof(unsigned char), 32);
    print_block(a + 64 * sizeof(unsigned char), 32);
    print_block(a + 96 * sizeof(unsigned char), 32);
    printf("\n");
}

typedef struct
{
    unsigned char a[128];
    unsigned char b[128];
    unsigned char p[128];
    unsigned char q[128];
    unsigned char m[128];
} curva;

curva* curva_init(unsigned char a[128], unsigned char b[128], unsigned char p[128], unsigned char q[128], unsigned char m[128])
{
    curva* E = (curva*)malloc(sizeof(curva));

    if (a) memcpy(E->a, a, 128);
    else memset(E->a, 0, 128);

    if (b) memcpy(E->b, b, 128);
    else memset(E->b, 0, 128);

    if (p) memcpy(E->p, p, 128);
    else memset(E->p, 0, 128);

    if (q) memcpy(E->q, q, 128);
    else memset(E->q, 0, 128);

    if (m) memcpy(E->m, m, 128);
    else memset(E->m, 0, 128);

    return E;
}

typedef struct
{
    unsigned char x[128];
    unsigned char y[128];
} point;

point* point_init(unsigned char x[128], unsigned char y[128])
{
    point* P = (point*)malloc(sizeof(point));

    if (x) memcpy(P->x, x, 128);
    else memset(P->x, 0, 128);

    if (y) memcpy(P->y, y, 128);
    else memset(P->y, 0, 128);

    return P;
}

int num_len(unsigned char num[128])
{
    int count = 1024;
    int i = 0;

    while (num[i] == 0)
    {
        count -= 8;
        i++;
    }

    if (i < 128)
    {
        int j = 1 << 7;

        while ((num[i] & j) == 0)
        {
            count--;
            j >>= 1;
        }
    }

    return count;
}

unsigned char* string_to_array(char* number_)
{
    unsigned char* result = (unsigned char*)malloc(128 * sizeof(unsigned char));
    memset(result, 0, 128);

    int len;
    for (len = 0; number_[len] != '\0'; len++);

    char* number = (char*)malloc(sizeof(char) * (len - 2));
    memcpy(number, number_ + sizeof(char) * 2, len - 2);

    len -= 2;

    char left, right;

    for (int i = len - 1, j = 127; i >= 0; i -= 2, j--)
    {
        if (i > 0)
        {
            left = number[i - 1];
            if (left >= '0' && left <= '9')
                left -= 48;
            else if (left >= 'A' && left <= 'F')
                left -= 55;
            else if (left >= 'a' && left <= 'f')
                left -= 87;
        }
        else left = 0;

        right = number[i];
        if (right >= '0' && right <= '9')
            right -= 48;
        else if (right >= 'A' && right <= 'F')
            right -= 55;
        else if (right >= 'a' && right <= 'f')
            right -= 87;

        result[j] = (left << 4) + right;
    }
    free(number);

    return result;
}

void left_shift(unsigned char* destination, unsigned char* source, int N)
{
    if (N < 1)
    {
        memcpy(destination, source, 128);
        return;
    }

    int N_div_8 = N >> 3;
    int i = 127;
    unsigned char result[128] = { 0 };

    for (int j = 0; j < N_div_8; j++)
    {
        result[i] = 0;
        i--;
    }

    N &= 0b111;

    unsigned long long buffer = 0;

    for (int j = 127; i >= 0; i--, j--)
    {
        buffer = (source[j] << N) + buffer;
        result[i] = buffer & 0xff;
        buffer >>= 8;
    }

    memcpy(destination, result, 128);
}

void right_shift(unsigned char* destination, unsigned char* source, int N)
{
    if (N < 1) return;

    int N_div_8 = N >> 3;
    int start;
    unsigned char result[128] = { 0 };

    for (start = 0; source[start] == 0; start++)
    {
        result[start] = 0;
    }

    for (int i = 0; i < N_div_8; i++)
    {
        result[start + i] = 0;
    }

    N &= 0b111;

    result[127] = source[127] >> N;

    for (int i = 126; i >= start; i--)
    {
        result[i + 1] += (source[i] & ((1 << N) - 1)) << (8 - N);
        result[i] = source[i] >> N;
    }

    memcpy(destination, result, 128);
}

int cmp512(unsigned char a[128], unsigned char b[128])
{
    int i = 0;

    while (i < 128)
    {
        if (a[i] == b[i]) i++;
        else break;
    }
    if (i == 128) return 0;
    if (a[i] > b[i]) return -1;
    if (a[i] < b[i]) return 1;
}

void sub512(unsigned char a[128], unsigned char b[128], unsigned char dest[128])
{
    int mark = 0, current_byte;
    unsigned char result[128] = { 0 };

    for (int i = 127; i >= 0; i--)
    {
        current_byte = a[i] - b[i] - mark;

        if (current_byte < 0)
        {
            result[i] = 256 + current_byte;
            mark = 1;
        }
        else
        {
            result[i] = current_byte;
            mark = 0;
        }
    }

    memcpy(dest, result, 128);
}

void add512(unsigned char a[128], unsigned char b[128], unsigned char dest[128])
{
    int mark = 0, current_byte;
    unsigned char result[128] = { 0 };

    for (int i = 127; i >= 0; i--)
    {
        current_byte = a[i] + b[i] + mark;

        if (current_byte > 255)
        {
            result[i] = current_byte - 256;
            mark = 1;
        }
        else
        {
            result[i] = current_byte;
            mark = 0;
        }
    }

    memcpy(dest, result, 128);
}

void inc512(unsigned char a[128], unsigned char dest[128])
{
    unsigned char result[128] = { 0 };

    for (int i = 127; i >= 0; i--)
    {
        result[i] = a[i] + 1;
        if (result[i]) break;
    }

    memcpy(dest, result, 128);
}

void div512(unsigned char a_[128], unsigned char b[128], unsigned char *div_dest, unsigned char* mod_dest)
{
    if (cmp512(a_, b) == 1)
    {
        //print_block(a_, 128);
        //print_block(b, 128);
        if (div_dest)
            memset(div_dest, 0, 128);
        if (mod_dest)
            memcpy(mod_dest, a_, 128);
        return;
    }

    unsigned char zero[128] = { 0 };
    if (cmp512(zero, b) == 0)
    {
        if (div_dest) memset(div_dest, 0xff, 128);
        if (mod_dest) memset(mod_dest, 0xff, 128);
        return;
    }

    unsigned char a[128];
    memcpy(a, a_, 128);

    int len_a = num_len(a);
    int len_b = num_len(b);

    unsigned char c[128];
    left_shift(c, b, len_a - len_b);

    while (cmp512(c, a) == -1)
        right_shift(c, c, 1);

    unsigned char div[128] = { 0 };

    while (cmp512(b, a) == 1)
    {
        while (cmp512(a, c) == 1)
        {
            left_shift(div, div, 1);
            right_shift(c, c, 1);
        }
        sub512(a, c, a);
        inc512(div, div);
    }

    int len_c = num_len(c);
    left_shift(div, div, len_c - len_b);

    if (div_dest) memcpy(div_dest, div, 128);
    if (mod_dest) memcpy(mod_dest, a, 128);
}

void mul512(unsigned char a[128], unsigned char b[128], unsigned char dest[128])
{
    int i, bit;

    unsigned char result[128];
    memcpy(result, a, 128);

    for (i = 0; b[i] == 0; i++);

    for (bit = 1 << 7; (bit & b[i]) == 0; bit >>= 1);
    bit >>= 1;

    for (; i < 128; i++)
    {
        for (int j = 0; bit > 0; j++)
        {
            left_shift(result, result, 1);
            if (b[i] & bit) add512(result, a, result);
            bit >>= 1;
        }

        bit = 1 << 7;
    }

    memcpy(dest, result, 128);
}

void inv_by_mod(unsigned char y[128], unsigned char p[128], unsigned char dest[128])
{
    unsigned char a[128], u[128] = { 0 }, b[128], v[128] = { 0 }, tmp[128], one[128] = {0};

    memcpy(a, y, 128);
    u[127] = 1;
    memcpy(b, p, 128);
    one[127] = 1;

    int k = 0;

    while (num_len(a))
    {
        if ((a[127] & 1) == 0)
        {
            right_shift(a, a, 1);
            //memset(tmp, 0, 128);
            if ((u[127] & 1))
            {
                add512(u, p, u);
            }
            right_shift(u, u, 1);
            //print_block(u, 128);
            //div512(u, E->p, NULL, u);
            //print_block(u, 128);
        }
        else
        {
            if (cmp512(a, b) == 1)
            {
                memcpy(tmp, a, 128);
                memcpy(a, b, 128);
                memcpy(b, tmp, 128);

                memcpy(tmp, u, 128);
                memcpy(u, v, 128);
                memcpy(v, tmp, 128);
            }

            sub512(a, b, a);
            right_shift(a, a, 1);

            if (cmp512(u, v) == 1)
            {
                sub512(p, v, tmp);
                add512(u, tmp, u);
            }
            else
            {
                sub512(u, v, u);
            }
            if ((u[127] & 1))
                add512(u, p, u);
            right_shift(u, u, 1);
        }

        /*printf("%d\n", k);
        print_block(a, 128);
        print_block(u, 128);
        print_block(b, 128);
        print_block(v, 128);
        printf("\n");
        k++;*/
    }

    memcpy(dest, v, 128);
}

void add_mod_p(unsigned char x[128], unsigned char y[128], unsigned char p[128], unsigned char dest[128])
{
    unsigned char result[128] = { 0 };

    memcpy(result, x, 128);
    add512(result, y, result);
    div512(result, p, NULL, result);

    memcpy(dest, result, 128);
}

void sub_mod_p(unsigned char x[128], unsigned char y[128], unsigned char p[128], unsigned char dest[128])
{
    unsigned char result[128] = { 0 };

    if (cmp512(x, y) == 1)
    {
        sub512(p, y, result);
        add512(result, x, result);
    }
    else sub512(x, y, result);

    memcpy(dest, result, 128);
}

void mul_mod_p(unsigned char x[128], unsigned char y[128], unsigned char p[128], unsigned char dest[128])
{
    unsigned char result[128] = { 0 };

    memcpy(result, x, 128);
    mul512(result, y, result);
    div512(result, p, NULL, result);

    memcpy(dest, result, 128);
}

void add_points(curva* E, point* P1, point* P2, point* P3)
{
    unsigned char Jl[128] = { 0 };
    unsigned char tmp[128] = { 0 };

    point* result = point_init(NULL, NULL);

    if (cmp512(P1->x, P2->x) || cmp512(P1->y, P2->y))
    {
        sub_mod_p(P2->y, P1->y, E->p, Jl);
        sub_mod_p(P2->x, P1->x, E->p, tmp);
        inv_by_mod(tmp, E->p, tmp);
        mul_mod_p(Jl, tmp, E->p, Jl);
    }
    else
    {
        mul_mod_p(P1->x, P1->x, E->p, tmp);
        add_mod_p(Jl, tmp, E->p, Jl);
        add_mod_p(Jl, tmp, E->p, Jl);
        add_mod_p(Jl, tmp, E->p, Jl);
        add_mod_p(Jl, E->a, E->p, Jl);
        add_mod_p(P1->y, P1->y, E->p, tmp);
        inv_by_mod(tmp, E->p, tmp);
        mul_mod_p(Jl, tmp, E->p, Jl);
    }

    mul_mod_p(Jl, Jl, E->p, result->x);
    sub_mod_p(result->x, P1->x, E->p, result->x);
    sub_mod_p(result->x, P2->x, E->p, result->x);

    sub_mod_p(P1->x, result->x, E->p, result->y);
    mul_mod_p(result->y, Jl, E->p, result->y);
    sub_mod_p(result->y, P1->y, E->p, result->y);

    memcpy(P3, result, sizeof(point));
}

void mul_point(curva* E, point* P, unsigned char k[128], point* Q)
{
    int i, bit;

    point* result = point_init(P->x, P->y);

    for (i = 0; k[i] == 0; i++);

    for (bit = 1 << 7; (bit & k[i]) == 0; bit >>= 1);
    bit >>= 1;

    for (; i < 128; i++)
    {
        for (int j = 0; bit > 0; j++)
        {
            add_points(E, result, result, result);
            if ((k[i] & bit)) add_points(E, result, P, result);
            bit >>= 1;
        }

        bit = 1 << 7;
    }

    memcpy(Q, result, sizeof(point));
}

unsigned char* random(unsigned char max[128])
{
    srand(time(NULL));
    int max_bytes = (num_len(max) >> 3) + ((num_len(max) & 0b111) ? 1 : 0);

    unsigned char* k = (unsigned char*)calloc(128, sizeof(unsigned char));

    if (max_bytes > 0)
    {
        k[127] = rand() & 0xff;
        if (!(k[127])) k++;
    }

    for (int i = 1; i < max_bytes; i++) k[127 - i] = rand() & 0xff;

    if (k[128 - max_bytes] > max[128 - max_bytes]) k[128 - max_bytes] -= max[128 - max_bytes];

    return k;
}

void sign(char* msg, curva* E, point* P, unsigned char d[128])
{
    streebog(msg, "sign_hash", 512);

    unsigned char buffer[128], e[128];
    int buffer_len;

    FILE* input_file = fopen("sign_hash", "rb");
    buffer_len = fread(buffer + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, input_file);
    fclose(input_file);

    div512(buffer, E->q, NULL, e);
    if (num_len(e) == 0) e[127] = 1;

    unsigned char k[128], r[128], s[128];
    point* C = point_init(NULL, NULL);

    generate_k:
    memcpy(k, random(E->q), 128);
    mul_point(E, P, k, C);
    div512(C->x, E->q, NULL, r);
    if (num_len(r) == 0) goto generate_k;

    mul_mod_p(r, d, E->q, s);
    mul_mod_p(k, e, E->q, buffer);
    add_mod_p(s, buffer, E->q, s);
    if (num_len(s) == 0) goto generate_k;

    FILE* output_file = fopen("sign", "wb");

    print1024(r);
    print1024(s);

    fwrite(r + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, output_file);
    fwrite(s + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, output_file);
    fclose(input_file);
}

void prov(char* msg, char* sign, curva* E, point* P, unsigned char d[128])
{
    unsigned char buffer[128];
    int buffer_len;

    unsigned char r[128] = { 0 }, s[128] = { 0 };
    FILE* sign_file = fopen(sign, "rb");
    buffer_len = fread(r + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, sign_file);
    buffer_len = fread(s + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, sign_file);
    fclose(sign_file);

    streebog(msg, "prov_hash", 512);

    FILE* input_file = fopen("prov_hash", "rb");
    buffer_len = fread(buffer + 64 * sizeof(unsigned char), sizeof(unsigned char), 64, input_file);
    fclose(input_file);

    unsigned char e[128], v[128], z1[128], z2[128];
    div512(buffer, E->q, NULL, e);
    if (num_len(e) == 0) e[127] = 1;

    inv_by_mod(e, E->q, v);

    mul_mod_p(s, v, E->q, z1);

    mul_mod_p(r, v, E->q, z2);
    sub_mod_p(E->q, z2, E->q, z2);

    point* Q = point_init(NULL, NULL);
    mul_point(E, P, d, Q);

    point* C = point_init(NULL, NULL);
    mul_point(E, P, z1, C);

    point* tmp_point = point_init(NULL, NULL);
    mul_point(E, Q, z2, tmp_point);

    add_points(E, C, tmp_point, C);

    unsigned char R[128];
    div512(C->x, E->q, NULL, R);

    print1024(s);
    print1024(r);
    print1024(R);

    if (cmp512(r, R) == 0) printf("YES\n");
    else printf("NO\n");
}

int main()
{
    unsigned char* a = string_to_array("0x7");
    unsigned char* b = string_to_array("0x1CFF0806A31116DA29D8CFA54E57EB748BC5F377E49400FDD788B649ECA1AC4361834013B2AD7322480A89CA58E0CF74BC9E540C2ADD6897FAD0A3084F302ADC");
    unsigned char* p = string_to_array("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DF1D852741AF4704A0458047E80E4546D35B8336FAC224DD81664BBF528BE6373");
    unsigned char* q = string_to_array("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DA82F2D7ECB1DBAC719905C5EECC423F1D86E25EDBE23C595D644AAF187E6E6DF");
    unsigned char* m = string_to_array("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DA82F2D7ECB1DBAC719905C5EECC423F1D86E25EDBE23C595D644AAF187E6E6DF");
    curva* E = curva_init(a, b, p, q, m);

    unsigned char* x = string_to_array("0x24D19CC64572EE30F396BF6EBBFD7A6C5213B3B3D7057CC825F91093A68CD762FD60611262CD838DC6B60AA7EEE804E28BC849977FAC33B4B530F1B120248A9A");
    unsigned char* y = string_to_array("0x2BB312A43BD2CE6E0D020613C857ACDDCFBF061E91E5F2C3F32447C259F39B2C83AB156D77F1496BF7EB3351E1EE4E43DC1A18B91B24640B6DBB92CB1ADD371E");
    point* P = point_init(x, y);

    unsigned char* d = string_to_array("0xBA6048AADAE241BA40936D47756D7C93091A0E8514669700EE7508E508B102072E8123B2200A0563322DAD2827E2714A2636B7BFD18AADFC62967821FA18DD4");

    time_t t = time(NULL);
    sign("text.txt", E, P, d);
    printf("Time - %d\n", time(NULL) - t);

    t = time(NULL);
    prov("text.txt", "sign", E, P, d);
    printf("Time - %d\n", time(NULL) - t);

    return 0;
}