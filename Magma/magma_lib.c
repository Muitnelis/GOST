#include "magma_lib.h"

unsigned char pi[8][16] = {
    { 0xc, 0x4, 0x6, 0x2, 0xa, 0x5, 0xb, 0x9, 0xe, 0x8, 0xd, 0x7, 0x0, 0x3, 0xf, 0x1 },
    { 0x6, 0x8, 0x2, 0x3, 0x9, 0xa, 0x5, 0xc, 0x1, 0xe, 0x4, 0x7, 0xb, 0xd, 0x0, 0xf },
    { 0xb, 0x3, 0x5, 0x8, 0x2, 0xf, 0xa, 0xd, 0xe, 0x1, 0x7, 0x4, 0xc, 0x9, 0x6, 0x0 },
    { 0xc, 0x8, 0x2, 0x1, 0xd, 0x4, 0xf, 0x6, 0x7, 0x0, 0xa, 0x5, 0x3, 0xe, 0x9, 0xb },
    { 0x7, 0xf, 0x5, 0xa, 0x8, 0x1, 0x6, 0xd, 0x0, 0x9, 0x3, 0xe, 0xb, 0x4, 0x2, 0xc },
    { 0x5, 0xd, 0xf, 0x6, 0x9, 0x2, 0xc, 0xa, 0xb, 0x7, 0x8, 0x1, 0x4, 0x3, 0xe, 0x0 },
    { 0x8, 0xe, 0x2, 0x5, 0x6, 0x9, 0x1, 0xc, 0xf, 0x4, 0xb, 0x0, 0xd, 0xa, 0x3, 0x7 },
    { 0x1, 0x7, 0xe, 0xd, 0x0, 0x5, 0x8, 0x3, 0x4, 0xf, 0xa, 0x6, 0x9, 0xc, 0xb, 0x2 }
};

void print_block(unsigned char* block, int len)
{
    for (int i = 0; i < len; i++)
    {
        if ((i & 0b1111) == 0)
            printf("\n");

        printf("%02x ", block[i]);
    }

    printf("\n");
}

void X_block(unsigned char* block, unsigned char* key, unsigned char bytes)
{
    for (int i = 0; i < bytes; i++)
    {
        block[i] ^= key[i];
    }
}

void key_schedule(unsigned char* keys, char* key_file_path)
{
    unsigned char buffer[32];
    size_t buffer_len;

    FILE* key_file = fopen(key_file_path, "rb");

    if (!key_file)
    {
        printf("fopen(key_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 32, key_file);

    if (buffer_len < 32)
    {
        printf("fread(key_file) error\n");
        exit(0);
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            memcpy(keys + (((i << 3) + j) << 2) * sizeof(unsigned char), buffer + (j << 2) * sizeof(unsigned char), 4);
        }
    }

    for (int j = 0; j < 8; j++)
    {
        memcpy(keys + ((24 + j) << 2) * sizeof(unsigned char), buffer + ((7 - j) << 2) * sizeof(unsigned char), 4);
    }

    fclose(key_file);
};

void t_draft(unsigned char* block)
{
    unsigned char left, right;

    for (int i = 0; i < 4; i++)
    {
        left = block[i] >> 4;
        right = block[i] & 0xf;

        left = pi[7 - (i << 1)][left];
        right = pi[7 - (i << 1) - 1][right];

        block[i] = (left << 4) + right;
    }
}

void Vec32plusVec32(unsigned char Vec1[4], unsigned char Vec2[4])
{
    int ost = 0;

    for (int i = 3; i >= 0; i--)
    {
        ost += Vec1[i] + Vec2[i];
        Vec1[i] = ost & 0xff;
        ost >>= 8;
    }
}


void shift(unsigned char* destination, unsigned char* source, int N)
{
    unsigned long long buffer = 0;

    for (int i = 7; i >= 0; i--)
    {
        buffer = (source[i] << N) + buffer;
        destination[i] = buffer & 0xff;
        buffer >>= 8;
    }
}

void circle_shift(unsigned char block[4], int N)
{
    unsigned long long answer = 0;

    for (int i = 0; i < 4; i++)
    {
        answer = (answer << 8) + block[i];
    }

    answer <<= N & 0xffffffff;
    answer = (answer & 0xffffffff) + (answer >> 32);

    for (int i = 3; i >= 0; i--)
    {
        block[i] = answer & 0xff;
        answer >>= 8;
    }
}

void g_draft(unsigned char* key, unsigned char* block)
{
    Vec32plusVec32(block, key);
    t_draft(block);
    circle_shift(block, 11);
}

void G(unsigned char key[4], unsigned char block[8])
{
    unsigned char a1[4], a0[4];

    memcpy(a1, block, 4);
    memcpy(a0, block + 4 * sizeof(unsigned char), 4);

    memcpy(block, a0, 4);

    g_draft(key, a0);
    X_block(a1, a0, 4);

    memcpy(block + 4 * sizeof(unsigned char), a1, 4);
}

void G_star(unsigned char key[4], unsigned char block[8])
{
    unsigned char a1[4], a0[4];

    memcpy(a1, block, 4);
    memcpy(a0, block + 4 * sizeof(unsigned char), 4);

    memcpy(block + 4 * sizeof(unsigned char), a0, 4);

    g_draft(key, a0);
    X_block(a1, a0, 4);

    memcpy(block, a1, 4);
}

void Magma_ENC(unsigned char block[4], unsigned char keys[32][4])
{
    for (int i = 0; i < 31; i++)
    {
        G(keys[i], block);
    }
    G_star(keys[31], block);
}

void Magma_DEC(unsigned char block[4], unsigned char keys[32][4])
{
    for (int i = 31; i > 0; i--)
    {
        G(keys[i], block);
    }
    G_star(keys[0], block);
}

void complete(unsigned char* block, size_t len)
{
    block[len] = 0b10000000;
    memset(block + (len + 1) * sizeof(unsigned char), 0, 8 - len);
}

void input_IV(char* IV_path, unsigned char IV[8])
{
    unsigned char buffer[8] = { 0 };
    size_t buffer_len = 0;

    FILE* IV_file = fopen(IV_path, "rb");

    if (!IV_file)
    {
        printf("fopen(IV_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 4, IV_file);

    if (buffer_len < 4)
    {
        printf("fread(IV_file) error\n");
        exit(0);
    }

    fclose(IV_file);

    memcpy(IV, buffer, 8);
}

void next_IV(unsigned char IV[8])
{
    int buffer = 1;

    for (int i = 7; i >= 0; i--)
    {
        buffer += IV[i];
        IV[i] = buffer & 0xff;
        buffer >>= 8;

        if (buffer == 0) break;
    }
}
