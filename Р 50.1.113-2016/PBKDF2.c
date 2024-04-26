#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

/*
    Dependencies:
        -   ../Streebog/streebog_lib.h
        -   ../P 50.1.113-2016/HMAC.c
*/

#include "streebog_lib.h"

unsigned char* PBKDF2(unsigned char* P_file_path, unsigned char* S_file_path, unsigned char* output_file_path, int c, int dklen)
{
    unsigned char buffer[bytes_count];
    size_t buffer_len;

    int n = dklen >> 6;

    if ((dklen & 0x3f) > 0) n++;

    unsigned char T_i[bytes_count], U[bytes_count];
    unsigned char* DK = (unsigned char*)malloc(sizeof(unsigned char) * (bytes_count * n));

    FILE* S_file, * S_i;

    for (int i = 0; i < n; i++)
    {
        S_file = fopen(S_file_path, "rb");

        if (!S_file)
        {
            printf("fopen(S_file) error\n");
            exit(0);
        }

        S_i = fopen("S_i.txt", "wb");

        if (!S_i)
        {
            printf("fopen(S_i) error\n");
            exit(0);
        }

        buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, S_file);

        while (buffer_len > 0)
        {
            fwrite(buffer, sizeof(unsigned char), buffer_len, S_i);
            buffer_len = fread(buffer, sizeof(unsigned char), bytes_count, S_file);
        }

        fclose(S_file);

        fwrite(IntToVec(i + 1, 4), sizeof(unsigned char), 4, S_i);

        fclose(S_i);

        memcpy(U, HMAC(P_file_path, "S_i.txt", "U.txt", 512), bytes_count);
        memcpy(T_i, U, bytes_count);

        for (int j = 1; j < c; j++)
        {
            memcpy(U, HMAC(P_file_path, "U.txt", "U.txt", 512), bytes_count);
            X_block(T_i, U);
        }

        memcpy(DK + i * bytes_count * sizeof(unsigned char), T_i, bytes_count);
    }

    unsigned char* result = (unsigned char*)malloc(sizeof(unsigned char) * dklen);
    memcpy(result, DK, dklen);

    free(DK);

    if (output_file_path)
    {
        FILE* PBKDF2_result = fopen(output_file_path, "wb");

        if (!PBKDF2_result)
        {
            printf("fopen(PBKDF2_result) error\n");
            exit(0);
        }

        fwrite(result, sizeof(unsigned char), dklen, PBKDF2_result);
        fclose(PBKDF2_result);
    }

    return result;
}

int main()
{
    PBKDF2("P.txt", "S.txt", "PBKDF2.txt", 1, 64);

    return 0;
}