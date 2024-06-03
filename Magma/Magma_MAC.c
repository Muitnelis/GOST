#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "magma_lib.h"

void MAC_Magma(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file, int size)
{
    unsigned char R[8] = { 0 }, K1[8] = { 0 }, K2[8] = { 0 }, C[8] = { 0 };
    unsigned char B[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b };

    unsigned char keys[32][4];
    key_schedule(keys, key_file);

    Magma_ENC(R, keys);

    shift(K1, R, 1);
    if (R[0] & 0b10000000)
    {
        X_block(K1, B, 8);
    }

    shift(K2, K1, 1);
    if (K1[0] & 0b10000000)
    {
        X_block(K2, B, 8);
    }

    print_block(R, 8);
    print_block(K1, 8);
    print_block(K2, 8);

    unsigned char buffer[8];
    size_t buffer_len = 0;

    FILE* input_file = fopen(input_file_path, "rb");

    if (!input_file)
    {
        printf("fopen(input_file) error\n");
        exit(0);
    }

    FILE* MAC_file = fopen(output_file_path, "wb");

    if (!MAC_file)
    {
        printf("fopen(MAC_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);
    if (buffer_len < 8)
    {
        complete(buffer, buffer_len);
    }

    while (buffer_len > 0)
    {
        X_block(C, buffer, 8);

        buffer_len = fread(buffer, sizeof(unsigned char), 8, input_file);

        if (buffer_len < 8)
        {
            if (buffer_len == 0)
            {
                X_block(C, K1, 8);
            }
            else
            {
                complete(buffer, buffer_len);
                X_block(C, K2, 8);
            }

            Magma_ENC(C, keys);
            break;
        }

        Magma_ENC(C, keys);
    }

    fwrite(C, sizeof(unsigned char), size, MAC_file);

    fclose(input_file);
    fclose(MAC_file);
}

int main(int argc, char* argv[])
{
    MAC_Magma("input.txt", "MAC.txt", "key.txt", 4);

    return 0;
}