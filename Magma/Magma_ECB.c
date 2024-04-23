#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "magma_lib.h"

void ECB_Magma_ENC(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file)
{
    unsigned char buffer[bytes_count];
    size_t buffer_len = 0;

    unsigned char keys[32][half_block];

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

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);

    while (buffer_len > 0)
    {
        Magma_ENC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), bytes_count, encrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);
    }

    fclose(input_file);
    fclose(encrypt_file);
}

void ECB_Magma_DEC(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file)
{
    unsigned char buffer[bytes_count];
    size_t buffer_len = 0;

    unsigned char keys[32][bytes_count];

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

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);

    while (buffer_len > 0)
    {
        Magma_DEC(buffer, keys);
        fwrite(buffer, sizeof(unsigned char), bytes_count, decrypt_file);

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);
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