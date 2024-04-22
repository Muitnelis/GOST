#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define bytes_count 4

unsigned char pi[][16] = {
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
        if ((i & 0b11) == 0)
            printf("\n");

        printf(", 0x%02x", block[i]);
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

void key_schedule(unsigned char* keys, unsigned char* key_file_path)
{
    unsigned char buffer[32];
    size_t buffer_len;

    FILE* key_file = fopen(key_file_path, "rb");

    if (!key_file)
    {
        printf("fopen(key_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count << 3, key_file);

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

    for (int i = 0; i < bytes_count; i++)
    {
        left = block[i] >> 4;
        right = block[i] & 0xf;

        left = pi[7 - (i << 1)][left];
        right = pi[7 - (i << 1) - 1][right];

        block[i] = (left << 4) + right;
    }
}

void Vec32plusVec32(unsigned char* Vec1, unsigned char* Vec2)
{
    int ost = 0;

    for (int i = bytes_count - 1; i >= 0; i--)
    {
        ost += Vec1[i] + Vec2[i];
        Vec1[i] = ost & 0xff;
        ost >>= 8;
    }
}

void shift(unsigned char* block)
{
    unsigned long long answer = 0;

    for (int i = 0; i < bytes_count; i++)
    {
        answer = (answer << 8) + block[i];
    }

    answer <<= 11;
    answer = (answer & 0xffffffff) + (answer >> 32);

    for (int i = bytes_count - 1; i >= 0; i--)
    {
        block[i] = answer & 0xff;
        answer >>= 8;
    }
}

void g_draft(unsigned char* key, unsigned char* block)
{
    Vec32plusVec32(block, key);
    t_draft(block);
    shift(block);
}

void G(unsigned char* key, unsigned char* block)
{
    unsigned char a1[bytes_count], a0[bytes_count];

    memcpy(a1, block, bytes_count);
    memcpy(a0, block + bytes_count * sizeof(unsigned char), bytes_count);

    memcpy(block, a0, bytes_count);

    g_draft(key, a0);
    X_block(a1, a0, bytes_count);

    memcpy(block + bytes_count * sizeof(unsigned char), a1, bytes_count);
}

void G_star(unsigned char* key, unsigned char* block)
{
    unsigned char a1[bytes_count], a0[bytes_count];

    memcpy(a1, block, bytes_count);
    memcpy(a0, block + bytes_count * sizeof(unsigned char), bytes_count);

    memcpy(block + bytes_count * sizeof(unsigned char), a0, bytes_count);

    g_draft(key, a0);
    X_block(a1, a0, bytes_count);

    memcpy(block, a1, bytes_count);
}

void Magma_ENC(unsigned char* block, unsigned char* keys)
{
    for (int i = 0; i < 31; i++)
    {
        G(keys + i * bytes_count * sizeof(unsigned char), block);
    }
    G_star(keys + 31 * bytes_count * sizeof(unsigned char), block);
}

void Magma_DEC(unsigned char* block, unsigned char* keys)
{
    for (int i = 31; i > 0; i--)
    {
        G(keys + i * bytes_count * sizeof(unsigned char), block);
    }
    G_star(keys, block);
}

void ECB_Magma_ENC(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file)
{
    unsigned char buffer[16];
    size_t buffer_len = 0;

    unsigned char keys[32][bytes_count];

    key_schedule(keys, "key.txt");

    FILE* input_file = fopen(input_file_path, "rb");

    if (!input_file)
    {
        printf("fopen(input_file) error\n");
        exit(0);
    }

    FILE* encrypt_file = fopen(output_file_path, "wb");

    if (!encrypt_file)
    {
        printf("fopen(encrypt_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count << 1, input_file);

    while (buffer_len > 0)
    {
        Magma_ENC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), bytes_count << 1, encrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count << 1, input_file);
    }

    fclose(input_file);
    fclose(encrypt_file);
}

void ECB_Magma_DEC(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file)
{
    unsigned char buffer[16];
    size_t buffer_len = 0;

    unsigned char keys[32][bytes_count];

    key_schedule(keys, "key.txt");

    FILE* input_file = fopen(input_file_path, "rb");

    if (!input_file)
    {
        printf("fopen(input_file) error\n");
        exit(0);
    }

    FILE* decrypt_file = fopen(output_file_path, "wb");

    if (!decrypt_file)
    {
        printf("fopen(decrypt_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count << 1, input_file);

    while (buffer_len > 0)
    {
        Magma_DEC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), bytes_count << 1, decrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count << 1, input_file);
    }

    fclose(input_file);
    fclose(decrypt_file);
}

int main(int argc, char* argv[])
{
    ECB_Magma_ENC("input.txt", "encrypted.txt", "key.txt");
    ECB_Magma_DEC("encrypted.txt", "decrypted.txt", "key.txt");

    return 0;
}