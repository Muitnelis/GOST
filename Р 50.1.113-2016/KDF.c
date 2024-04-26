#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/*
    Dependencies:
        -   ../P 50.1.113-2016/HMAC.c
*/

unsigned char* KDF(unsigned char* Kin_file_path, unsigned char* label_file_path, unsigned char* seed_file_path, unsigned char* output_file_path, int bits)
{
    int bytes = bits >> 3;

    unsigned char buffer[bytes_count];
    size_t buffer_len;

    unsigned char numbers[2] = { 0x01, 0x00 };

    FILE* label_file = fopen(label_file_path, "rb");

    if (!label_file)
    {
        printf("fopen(label_file) error\n");
        exit(0);
    }

    FILE* seed_file = fopen(seed_file_path, "rb");

    if (!seed_file)
    {
        printf("fopen(seed_file) error\n");
        exit(0);
    }

    FILE* HMAC_input_file = fopen("HMAC_input_file.txt", "wb");

    if (!HMAC_input_file)
    {
        printf("fopen(HMAC_input_file) error\n");
        exit(0);
    }

    fwrite(numbers, sizeof(unsigned char), 1, HMAC_input_file);

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, label_file);

    while (buffer_len > 0)
    {
        fwrite(buffer, sizeof(unsigned char), buffer_len, HMAC_input_file);
        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, label_file);
    }

    fwrite(numbers + sizeof(unsigned char), sizeof(unsigned char), 1, HMAC_input_file);

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, seed_file);

    while (buffer_len > 0)
    {
        fwrite(buffer, sizeof(unsigned char), buffer_len, HMAC_input_file);
        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, seed_file);
    }

    fwrite(numbers, sizeof(unsigned char), 2, HMAC_input_file);

    fclose(label_file);
    fclose(seed_file);
    fclose(HMAC_input_file);

    unsigned char* result = (unsigned char*)malloc(bytes * sizeof(unsigned char));

    memcpy(result, HMAC(Kin_file_path, "HMAC_input_file.txt", NULL, bits), bytes);

    if (output_file_path)
    {
        FILE* KDF_result = fopen(output_file_path, "wb");

        if (!KDF_result)
        {
            printf("fopen(KDF_result) error\n");
            exit(0);
        }

        fwrite(result, sizeof(unsigned char), bytes, KDF_result);
        fclose(KDF_result);
    }

    return result;
}

int main()
{
    KDF("Kin.txt", "label.txt", "seed.txt", "KDF.txt", 256);

    return 0;
}