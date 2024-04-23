#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "Kuznechik.h"

void MAC_Kuznechik(unsigned char* input_file_path, unsigned char* output_file_path, unsigned char* key_file, int size)
{
    unsigned char R[bytes_count] = { 0 }, K1[bytes_count] = { 0 }, K2[bytes_count] = { 0 }, C[bytes_count] = { 0 };
    unsigned char B[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87 };

    unsigned char keys[10][bytes_count];
    key_schedule(keys, key_file);

    Kuznechik_ENC(R, keys);

    shift(K1, R, 1);
    if (R[0] & 0b10000000)
    {
        X_block(K1, B, bytes_count);
    }

    shift(K2, K1, 1);
    if (K1[0] & 0b10000000)
    {
        X_block(K2, B, bytes_count);
    }

    unsigned char buffer[16];
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

    buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);
    if (buffer_len < bytes_count)
    {
        complete(buffer, buffer_len);
    }

    while (buffer_len > 0)
    {
        X_block(C, buffer, bytes_count);

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, input_file);

        if (buffer_len < bytes_count)
        {
            if (buffer_len == 0)
            {
                X_block(C, K1, bytes_count);
            }
            else
            {
                complete(buffer, buffer_len);
                X_block(C, K2, bytes_count);
            }

            Kuznechik_ENC(C, keys);
            break;
        }

        Kuznechik_ENC(C, keys);
    }

    fwrite(C, sizeof(unsigned char), size, MAC_file);

    fclose(input_file);
    fclose(MAC_file);
}

int main(int argc, char* argv[])
{
    MAC_Kuznechik("input.txt", "MAC.txt", "key.txt", 8);

    return 0;
}