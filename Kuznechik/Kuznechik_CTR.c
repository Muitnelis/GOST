#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "kuznechik_lib.h"

void CTR_Kuznechik_ENC(char* input_file_path, char* output_file_path, char* key_file, char* IV_file_path)
{
    unsigned char buffer[32], block[16];
    size_t buffer_len = 0;

    unsigned char IV[16] = { 0 };
    input_IV(IV_file_path, IV);

    unsigned char keys[10][bytes_count];

    key_schedule(keys, key_file);

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

    buffer_len = fread(buffer, sizeof(unsigned char), 16, input_file);

    while (buffer_len > 0)
    {
        if (buffer_len < 16)
        {
            complete(buffer, buffer_len);
        }

        memcpy(block, IV, 16);

        Kuznechik_ENC(block, keys);
        X_block(buffer, block, 16);

        fwrite(buffer, sizeof(unsigned char), 16, encrypt_file);

        memset(buffer, 0, 16);
        buffer_len = fread(buffer, sizeof(unsigned char), 16, input_file);
        next_IV(IV);
    }

    fclose(input_file);
    fclose(encrypt_file);
}

void CTR_Kuznechik_DEC(char* input_file_path, char* output_file_path, char* key_file, char* IV_file_path)
{
    unsigned char buffer[32], block[16];
    size_t buffer_len = 0;

    unsigned char IV[16] = { 0 };
    input_IV(IV_file_path, IV);

    unsigned char keys[10][bytes_count];

    key_schedule(keys, key_file);

    FILE* encrypt_file = fopen(input_file_path, "rb");

    if (!encrypt_file)
    {
        printf("fopen(encrypt_file) error\n");
        exit(0);
    }

    FILE* decrypt_file = fopen(output_file_path, "wb");

    if (!decrypt_file)
    {
        printf("fopen(decrypt_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 16, encrypt_file);

    while (buffer_len > 0)
    {
        memcpy(block, IV, 16);

        Kuznechik_ENC(block, keys);
        X_block(buffer, block, 16);

        fwrite(buffer, sizeof(unsigned char), 16, decrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), 16, encrypt_file);
        next_IV(IV);
    }

    fclose(encrypt_file);
    fclose(decrypt_file);
}

int main(int argc, char* argv[])
{
    CTR_Kuznechik_ENC("input.txt", "encrypted.txt", "key.txt", "IV.txt");
    CTR_Kuznechik_DEC("encrypted.txt", "decrypted.txt", "key.txt", "IV.txt");

    return 0;
}