#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/*
    Dependencies:
        -   ../P 50.1.113-2016/KDF.c
*/

unsigned char* tlstree(unsigned char* Kroot_file_path, unsigned long long I, unsigned char* output_file_path, int bits)
{
    int bytes = bits >> 3;

    unsigned char* buffer = (unsigned char*)malloc(bytes * sizeof(unsigned char));
    size_t buffer_len;

    unsigned long long C[3] = { 0xffffffffe0000000 & I, 0xffffffffffff0000 & I, 0xfffffffffffffff8 & I };

    FILE* K_file = fopen("K.txt", "wb");
    FILE* Kroot_file = fopen(Kroot_file_path, "rb");

    buffer_len = fread(buffer, sizeof(unsigned char), bytes + 1, Kroot_file);
    if (buffer_len > bytes)
    {
        printf("fread(Kroot_file) error");
        exit(0);
    }
    fwrite(buffer, sizeof(unsigned char), bytes, K_file);

    fclose(Kroot_file);
    fclose(K_file);

    FILE* level_file, * D_file;
    unsigned char* result = (unsigned char*)malloc(bytes * sizeof(unsigned char));
    unsigned char I_and_C[8];

    for (int i = 0; i < 3; i++)
    {
        level_file = fopen("level.txt", "wb");
        fprintf(level_file, "level%d", i + 1);
        fclose(level_file);

        D_file = fopen("D.txt", "wb");
        for (int j = 7; j >= 0; j--)
        {
            I_and_C[j] = C[i] & 0xff;
            C[i] >>= 8;
        }
        fwrite(I_and_C, sizeof(unsigned char), 8, D_file);
        fclose(D_file);

        memcpy(result, KDF("K.txt", "level.txt", "D.txt", "K.txt", bits), bytes);
    }

    if (output_file_path)
    {
        FILE* tlstree_result = fopen(output_file_path, "wb");

        if (!tlstree_result)
        {
            printf("fopen(tlstree_result) error\n");
            exit(0);
        }

        fwrite(result, sizeof(unsigned char), bytes, tlstree_result);
        fclose(tlstree_result);
    }

    free(buffer);

    return result;
}

int main()
{
    tlstree("tlstreetest.txt", 0, "tlstree.txt", 256);

    return 0;
}