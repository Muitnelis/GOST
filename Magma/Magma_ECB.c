#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "magma_lib.h"

void ECB_Magma_ENC(char* input_file_path, char* output_file_path, char* key_file)
{
    unsigned char buffer[8];
    size_t buffer_len = 0;

    unsigned char keys[32][4];

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

    buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);

    while (buffer_len > 0)
    {
        Magma_ENC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), 8, encrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);
    }

    fclose(input_file);
    fclose(encrypt_file);
}

void ECB_Magma_DEC(char* input_file_path, char* output_file_path, char* key_file)
{
    unsigned char buffer[8];
    size_t buffer_len = 0;

    unsigned char keys[32][4];

    key_schedule(keys, key_file);

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

    buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);

    while (buffer_len > 0)
    {
        Magma_DEC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), 8, decrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);
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