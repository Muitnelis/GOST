#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "streebog_lib.h"
#include "HMAC_lib.h"

unsigned char ipad[] = {
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
    0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36
};

unsigned char opad[] = {
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c
};

unsigned char* HMAC(unsigned char* K_file_path, unsigned char* T_file_path, unsigned char* output_file_path, int bytes)
{
    unsigned char K_star[bytes_count] = { 0 }, buffer[bytes_count] = { 0 };
    size_t buffer_len;

    if (K_file_path)
    {
        FILE* K_file = fopen(K_file_path, "rb");

        if (!K_file)
        {
            printf("fopen(K_file) error\n");
            exit(0);
        }

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, K_file);
        memcpy(K_star, buffer, buffer_len);

        fclose(K_file);
    }

    byte_form(K_star, bytes_count);

    FILE* streebog_input_1 = fopen("streebog_input.txt", "wb");

    if (!streebog_input_1)
    {
        printf("fopen(streebog_input_1) error\n");
        exit(0);
    }

    FILE* T_file = fopen(T_file_path, "rb");

    if (!T_file)
    {
        printf("fopen(T_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, T_file);

    while (buffer_len > 0)
    {
        reverse(buffer, buffer_len);
        fwrite(buffer, sizeof(unsigned char), buffer_len, streebog_input_1);
        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, T_file);
    }

    fclose(T_file);

    memcpy(buffer, K_star, bytes_count);
    X_block(buffer, ipad);
    byte_form(buffer, bytes_count);
    bit_form(buffer, bytes_count);

    fwrite(buffer, sizeof(unsigned char), bytes_count, streebog_input_1);

    fclose(streebog_input_1);

    streebog("streebog_input.txt", "streebog_output.txt", bytes);

    FILE* streebog_input_2 = fopen("streebog_input.txt", "wb");

    if (!streebog_input_2)
    {
        printf("fopen(streebog_input_2) error\n");
        exit(0);
    }

    FILE* streebog_output_1 = fopen("streebog_output.txt", "rb");

    if (!streebog_output_1)
    {
        printf("fopen(streebog_output_1) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), bytes >> 3, streebog_output_1);
    fwrite(buffer, sizeof(unsigned char), bytes >> 3, streebog_input_2);

    memcpy(buffer, K_star, bytes_count);
    X_block(buffer, opad);
    byte_form(buffer, bytes_count);
    bit_form(buffer, bytes_count);
    fwrite(buffer, sizeof(unsigned char), bytes_count, streebog_input_2);

    fclose(streebog_input_2);
    fclose(streebog_output_1);

    memcpy(buffer, streebog("streebog_input.txt", "streebog_output.txt", bytes), bytes >> 3);
    reverse(buffer, bytes >> 3);

    unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char) * (bytes >> 3));
    memcpy(result, buffer, bytes >> 3);

    if (output_file_path)
    {
        FILE* HMAC_result = fopen(output_file_path, "wb");

        if (!HMAC_result)
        {
            printf("fopen(HMAC_result) error\n");
            exit(0);
        }

        fwrite(result, sizeof(unsigned char), bytes >> 3, HMAC_result);
        fclose(HMAC_result);
    }

    return result;
}