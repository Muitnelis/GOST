#include "GOST_3412.h"

//----------------------------

cipher Kuznechik = { 16, NULL, key_schedule_Kuznechik, Kuznechik_ENC, Kuznechik_DEC };

cipher Magma = { 8, NULL, key_schedule_Magma, Magma_ENC, Magma_DEC };

//----------------------------

void print_block(unsigned char* block, int len)
{
    printf("%02x ", block[0]);

    for (int i = 1; i < len; i++)
    {
        printf("%02x ", block[i]);
        if ((i & 0b1111) == 0) printf("\n");
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

//----------------------------

unsigned char S[256] = {
    0xfc, 0xee, 0xdd, 0x11, 0xcf, 0x6e, 0x31, 0x16, 0xfb, 0xc4, 0xfa, 0xda, 0x23, 0xc5, 0x04, 0x4d,
    0xe9, 0x77, 0xf0, 0xdb, 0x93, 0x2e, 0x99, 0xba, 0x17, 0x36, 0xf1, 0xbb, 0x14, 0xcd, 0x5f, 0xc1,
    0xf9, 0x18, 0x65, 0x5a, 0xe2, 0x5c, 0xef, 0x21, 0x81, 0x1c, 0x3c, 0x42, 0x8b, 0x01, 0x8e, 0x4f,
    0x05, 0x84, 0x02, 0xae, 0xe3, 0x6a, 0x8f, 0xa0, 0x06, 0x0b, 0xed, 0x98, 0x7f, 0xd4, 0xd3, 0x1f,
    0xeb, 0x34, 0x2c, 0x51, 0xea, 0xc8, 0x48, 0xab, 0xf2, 0x2a, 0x68, 0xa2, 0xfd, 0x3a, 0xce, 0xcc,
    0xb5, 0x70, 0x0e, 0x56, 0x08, 0x0c, 0x76, 0x12, 0xbf, 0x72, 0x13, 0x47, 0x9c, 0xb7, 0x5d, 0x87,
    0x15, 0xa1, 0x96, 0x29, 0x10, 0x7b, 0x9a, 0xc7, 0xf3, 0x91, 0x78, 0x6f, 0x9d, 0x9e, 0xb2, 0xb1,
    0x32, 0x75, 0x19, 0x3d, 0xff, 0x35, 0x8a, 0x7e, 0x6d, 0x54, 0xc6, 0x80, 0xc3, 0xbd, 0x0d, 0x57,
    0xdf, 0xf5, 0x24, 0xa9, 0x3e, 0xa8, 0x43, 0xc9, 0xd7, 0x79, 0xd6, 0xf6, 0x7c, 0x22, 0xb9, 0x03,
    0xe0, 0x0f, 0xec, 0xde, 0x7a, 0x94, 0xb0, 0xbc, 0xdc, 0xe8, 0x28, 0x50, 0x4e, 0x33, 0x0a, 0x4a,
    0xa7, 0x97, 0x60, 0x73, 0x1e, 0x00, 0x62, 0x44, 0x1a, 0xb8, 0x38, 0x82, 0x64, 0x9f, 0x26, 0x41,
    0xad, 0x45, 0x46, 0x92, 0x27, 0x5e, 0x55, 0x2f, 0x8c, 0xa3, 0xa5, 0x7d, 0x69, 0xd5, 0x95, 0x3b,
    0x07, 0x58, 0xb3, 0x40, 0x86, 0xac, 0x1d, 0xf7, 0x30, 0x37, 0x6b, 0xe4, 0x88, 0xd9, 0xe7, 0x89,
    0xe1, 0x1b, 0x83, 0x49, 0x4c, 0x3f, 0xf8, 0xfe, 0x8d, 0x53, 0xaa, 0x90, 0xca, 0xd8, 0x85, 0x61,
    0x20, 0x71, 0x67, 0xa4, 0x2d, 0x2b, 0x09, 0x5b, 0xcb, 0x9b, 0x25, 0xd0, 0xbe, 0xe5, 0x6c, 0x52,
    0x59, 0xa6, 0x74, 0xd2, 0xe6, 0xf4, 0xb4, 0xc0, 0xd1, 0x66, 0xaf, 0xc2, 0x39, 0x4b, 0x63, 0xb6
};

unsigned char S_inv[256] = {
    0xa5, 0x2d, 0x32, 0x8f, 0x0e, 0x30, 0x38, 0xc0, 0x54, 0xe6, 0x9e, 0x39, 0x55, 0x7e, 0x52, 0x91,
    0x64, 0x03, 0x57, 0x5a, 0x1c, 0x60, 0x07, 0x18, 0x21, 0x72, 0xa8, 0xd1, 0x29, 0xc6, 0xa4, 0x3f,
    0xe0, 0x27, 0x8d, 0x0c, 0x82, 0xea, 0xae, 0xb4, 0x9a, 0x63, 0x49, 0xe5, 0x42, 0xe4, 0x15, 0xb7,
    0xc8, 0x06, 0x70, 0x9d, 0x41, 0x75, 0x19, 0xc9, 0xaa, 0xfc, 0x4d, 0xbf, 0x2a, 0x73, 0x84, 0xd5,
    0xc3, 0xaf, 0x2b, 0x86, 0xa7, 0xb1, 0xb2, 0x5b, 0x46, 0xd3, 0x9f, 0xfd, 0xd4, 0x0f, 0x9c, 0x2f,
    0x9b, 0x43, 0xef, 0xd9, 0x79, 0xb6, 0x53, 0x7f, 0xc1, 0xf0, 0x23, 0xe7, 0x25, 0x5e, 0xb5, 0x1e,
    0xa2, 0xdf, 0xa6, 0xfe, 0xac, 0x22, 0xf9, 0xe2, 0x4a, 0xbc, 0x35, 0xca, 0xee, 0x78, 0x05, 0x6b,
    0x51, 0xe1, 0x59, 0xa3, 0xf2, 0x71, 0x56, 0x11, 0x6a, 0x89, 0x94, 0x65, 0x8c, 0xbb, 0x77, 0x3c,
    0x7b, 0x28, 0xab, 0xd2, 0x31, 0xde, 0xc4, 0x5f, 0xcc, 0xcf, 0x76, 0x2c, 0xb8, 0xd8, 0x2e, 0x36,
    0xdb, 0x69, 0xb3, 0x14, 0x95, 0xbe, 0x62, 0xa1, 0x3b, 0x16, 0x66, 0xe9, 0x5c, 0x6c, 0x6d, 0xad,
    0x37, 0x61, 0x4b, 0xb9, 0xe3, 0xba, 0xf1, 0xa0, 0x85, 0x83, 0xda, 0x47, 0xc5, 0xb0, 0x33, 0xfa,
    0x96, 0x6f, 0x6e, 0xc2, 0xf6, 0x50, 0xff, 0x5d, 0xa9, 0x8e, 0x17, 0x1b, 0x97, 0x7d, 0xec, 0x58,
    0xf7, 0x1f, 0xfb, 0x7c, 0x09, 0x0d, 0x7a, 0x67, 0x45, 0x87, 0xdc, 0xe8, 0x4f, 0x1d, 0x4e, 0x04,
    0xeb, 0xf8, 0xf3, 0x3e, 0x3d, 0xbd, 0x8a, 0x88, 0xdd, 0xcd, 0x0b, 0x13, 0x98, 0x02, 0x93, 0x80,
    0x90, 0xd0, 0x24, 0x34, 0xcb, 0xed, 0xf4, 0xce, 0x99, 0x10, 0x44, 0x40, 0x92, 0x3a, 0x01, 0x26,
    0x12, 0x1a, 0x48, 0x68, 0xf5, 0x81, 0x8b, 0xc7, 0xd6, 0x20, 0x0a, 0x08, 0x00, 0x4c, 0xd7, 0x74
};

unsigned char pow2[256] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0xc3, 0x45, 0x8a, 0xd7, 0x6d, 0xda, 0x77, 0xee,
    0x1f, 0x3e, 0x7c, 0xf8, 0x33, 0x66, 0xcc, 0x5b, 0xb6, 0xaf, 0x9d, 0xf9, 0x31, 0x62, 0xc4, 0x4b,
    0x96, 0xef, 0x1d, 0x3a, 0x74, 0xe8, 0x13, 0x26, 0x4c, 0x98, 0xf3, 0x25, 0x4a, 0x94, 0xeb, 0x15,
    0x2a, 0x54, 0xa8, 0x93, 0xe5, 0x09, 0x12, 0x24, 0x48, 0x90, 0xe3, 0x05, 0x0a, 0x14, 0x28, 0x50,
    0xa0, 0x83, 0xc5, 0x49, 0x92, 0xe7, 0x0d, 0x1a, 0x34, 0x68, 0xd0, 0x63, 0xc6, 0x4f, 0x9e, 0xff,
    0x3d, 0x7a, 0xf4, 0x2b, 0x56, 0xac, 0x9b, 0xf5, 0x29, 0x52, 0xa4, 0x8b, 0xd5, 0x69, 0xd2, 0x67,
    0xce, 0x5f, 0xbe, 0xbf, 0xbd, 0xb9, 0xb1, 0xa1, 0x81, 0xc1, 0x41, 0x82, 0xc7, 0x4d, 0x9a, 0xf7,
    0x2d, 0x5a, 0xb4, 0xab, 0x95, 0xe9, 0x11, 0x22, 0x44, 0x88, 0xd3, 0x65, 0xca, 0x57, 0xae, 0x9f,
    0xfd, 0x39, 0x72, 0xe4, 0x0b, 0x16, 0x2c, 0x58, 0xb0, 0xa3, 0x85, 0xc9, 0x51, 0xa2, 0x87, 0xcd,
    0x59, 0xb2, 0xa7, 0x8d, 0xd9, 0x71, 0xe2, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0x03, 0x06, 0x0c,
    0x18, 0x30, 0x60, 0xc0, 0x43, 0x86, 0xcf, 0x5d, 0xba, 0xb7, 0xad, 0x99, 0xf1, 0x21, 0x42, 0x84,
    0xcb, 0x55, 0xaa, 0x97, 0xed, 0x19, 0x32, 0x64, 0xc8, 0x53, 0xa6, 0x8f, 0xdd, 0x79, 0xf2, 0x27,
    0x4e, 0x9c, 0xfb, 0x35, 0x6a, 0xd4, 0x6b, 0xd6, 0x6f, 0xde, 0x7f, 0xfe, 0x3f, 0x7e, 0xfc, 0x3b,
    0x76, 0xec, 0x1b, 0x36, 0x6c, 0xd8, 0x73, 0xe6, 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 0x23, 0x46, 0x8c,
    0xdb, 0x75, 0xea, 0x17, 0x2e, 0x5c, 0xb8, 0xb3, 0xa5, 0x89, 0xd1, 0x61, 0xc2, 0x47, 0x8e, 0xdf,
    0x7d, 0xfa, 0x37, 0x6e, 0xdc, 0x7b, 0xf6, 0x2f, 0x5e, 0xbc, 0xbb, 0xb5, 0xa9, 0x91, 0xe1, 0x01
};

unsigned char c_const[32][16] = {
    { 0x6e, 0xa2, 0x76, 0x72, 0x6c, 0x48, 0x7a, 0xb8, 0x5d, 0x27, 0xbd, 0x10, 0xdd, 0x84, 0x94, 0x01 },
    { 0xdc, 0x87, 0xec, 0xe4, 0xd8, 0x90, 0xf4, 0xb3, 0xba, 0x4e, 0xb9, 0x20, 0x79, 0xcb, 0xeb, 0x02 },
    { 0xb2, 0x25, 0x9a, 0x96, 0xb4, 0xd8, 0x8e, 0x0b, 0xe7, 0x69, 0x04, 0x30, 0xa4, 0x4f, 0x7f, 0x03 },
    { 0x7b, 0xcd, 0x1b, 0x0b, 0x73, 0xe3, 0x2b, 0xa5, 0xb7, 0x9c, 0xb1, 0x40, 0xf2, 0x55, 0x15, 0x04 },
    { 0x15, 0x6f, 0x6d, 0x79, 0x1f, 0xab, 0x51, 0x1d, 0xea, 0xbb, 0x0c, 0x50, 0x2f, 0xd1, 0x81, 0x05 },
    { 0xa7, 0x4a, 0xf7, 0xef, 0xab, 0x73, 0xdf, 0x16, 0x0d, 0xd2, 0x08, 0x60, 0x8b, 0x9e, 0xfe, 0x06 },
    { 0xc9, 0xe8, 0x81, 0x9d, 0xc7, 0x3b, 0xa5, 0xae, 0x50, 0xf5, 0xb5, 0x70, 0x56, 0x1a, 0x6a, 0x07 },
    { 0xf6, 0x59, 0x36, 0x16, 0xe6, 0x05, 0x56, 0x89, 0xad, 0xfb, 0xa1, 0x80, 0x27, 0xaa, 0x2a, 0x08 },
    { 0x98, 0xfb, 0x40, 0x64, 0x8a, 0x4d, 0x2c, 0x31, 0xf0, 0xdc, 0x1c, 0x90, 0xfa, 0x2e, 0xbe, 0x09 },
    { 0x2a, 0xde, 0xda, 0xf2, 0x3e, 0x95, 0xa2, 0x3a, 0x17, 0xb5, 0x18, 0xa0, 0x5e, 0x61, 0xc1, 0x0a },
    { 0x44, 0x7c, 0xac, 0x80, 0x52, 0xdd, 0xd8, 0x82, 0x4a, 0x92, 0xa5, 0xb0, 0x83, 0xe5, 0x55, 0x0b },
    { 0x8d, 0x94, 0x2d, 0x1d, 0x95, 0xe6, 0x7d, 0x2c, 0x1a, 0x67, 0x10, 0xc0, 0xd5, 0xff, 0x3f, 0x0c },
    { 0xe3, 0x36, 0x5b, 0x6f, 0xf9, 0xae, 0x07, 0x94, 0x47, 0x40, 0xad, 0xd0, 0x08, 0x7b, 0xab, 0x0d },
    { 0x51, 0x13, 0xc1, 0xf9, 0x4d, 0x76, 0x89, 0x9f, 0xa0, 0x29, 0xa9, 0xe0, 0xac, 0x34, 0xd4, 0x0e },
    { 0x3f, 0xb1, 0xb7, 0x8b, 0x21, 0x3e, 0xf3, 0x27, 0xfd, 0x0e, 0x14, 0xf0, 0x71, 0xb0, 0x40, 0x0f },
    { 0x2f, 0xb2, 0x6c, 0x2c, 0x0f, 0x0a, 0xac, 0xd1, 0x99, 0x35, 0x81, 0xc3, 0x4e, 0x97, 0x54, 0x10 },
    { 0x41, 0x10, 0x1a, 0x5e, 0x63, 0x42, 0xd6, 0x69, 0xc4, 0x12, 0x3c, 0xd3, 0x93, 0x13, 0xc0, 0x11 },
    { 0xf3, 0x35, 0x80, 0xc8, 0xd7, 0x9a, 0x58, 0x62, 0x23, 0x7b, 0x38, 0xe3, 0x37, 0x5c, 0xbf, 0x12 },
    { 0x9d, 0x97, 0xf6, 0xba, 0xbb, 0xd2, 0x22, 0xda, 0x7e, 0x5c, 0x85, 0xf3, 0xea, 0xd8, 0x2b, 0x13 },
    { 0x54, 0x7f, 0x77, 0x27, 0x7c, 0xe9, 0x87, 0x74, 0x2e, 0xa9, 0x30, 0x83, 0xbc, 0xc2, 0x41, 0x14 },
    { 0x3a, 0xdd, 0x01, 0x55, 0x10, 0xa1, 0xfd, 0xcc, 0x73, 0x8e, 0x8d, 0x93, 0x61, 0x46, 0xd5, 0x15 },
    { 0x88, 0xf8, 0x9b, 0xc3, 0xa4, 0x79, 0x73, 0xc7, 0x94, 0xe7, 0x89, 0xa3, 0xc5, 0x09, 0xaa, 0x16 },
    { 0xe6, 0x5a, 0xed, 0xb1, 0xc8, 0x31, 0x09, 0x7f, 0xc9, 0xc0, 0x34, 0xb3, 0x18, 0x8d, 0x3e, 0x17 },
    { 0xd9, 0xeb, 0x5a, 0x3a, 0xe9, 0x0f, 0xfa, 0x58, 0x34, 0xce, 0x20, 0x43, 0x69, 0x3d, 0x7e, 0x18 },
    { 0xb7, 0x49, 0x2c, 0x48, 0x85, 0x47, 0x80, 0xe0, 0x69, 0xe9, 0x9d, 0x53, 0xb4, 0xb9, 0xea, 0x19 },
    { 0x05, 0x6c, 0xb6, 0xde, 0x31, 0x9f, 0x0e, 0xeb, 0x8e, 0x80, 0x99, 0x63, 0x10, 0xf6, 0x95, 0x1a },
    { 0x6b, 0xce, 0xc0, 0xac, 0x5d, 0xd7, 0x74, 0x53, 0xd3, 0xa7, 0x24, 0x73, 0xcd, 0x72, 0x01, 0x1b },
    { 0xa2, 0x26, 0x41, 0x31, 0x9a, 0xec, 0xd1, 0xfd, 0x83, 0x52, 0x91, 0x03, 0x9b, 0x68, 0x6b, 0x1c },
    { 0xcc, 0x84, 0x37, 0x43, 0xf6, 0xa4, 0xab, 0x45, 0xde, 0x75, 0x2c, 0x13, 0x46, 0xec, 0xff, 0x1d },
    { 0x7e, 0xa1, 0xad, 0xd5, 0x42, 0x7c, 0x25, 0x4e, 0x39, 0x1c, 0x28, 0x23, 0xe2, 0xa3, 0x80, 0x1e },
    { 0x10, 0x03, 0xdb, 0xa7, 0x2e, 0x34, 0x5f, 0xf6, 0x64, 0x3b, 0x95, 0x33, 0x3f, 0x27, 0x14, 0x1f },
    { 0x5e, 0xa7, 0xd8, 0x58, 0x1e, 0x14, 0x9b, 0x61, 0xf1, 0x6a, 0xc1, 0x45, 0x9c, 0xed, 0xa8, 0x20 }
};

void S_block(unsigned char block[16], unsigned char substitution[256])
{
    for (int i = 0; i < 16; i++)
    {
        block[i] = substitution[block[i]];
    }
}

unsigned char galua_mul(unsigned char a, unsigned char b)
{
    unsigned char pow2a, pow2b;

    for (int i = 0; i < 256; i++)
        if (pow2[i] == a)
        {
            pow2a = i;
            break;
        }

    for (int i = 0; i < 256; i++)
        if (pow2[i] == b)
        {
            pow2b = i;
            break;
        }

    if ((pow2a + pow2b) > 255)
        return pow2[pow2a + pow2b - 255];

    return pow2[pow2a + pow2b];
}

unsigned char l_draft(unsigned char a[16])
{
    unsigned char answer = 0;

    if (a[0]) answer ^= galua_mul(148, a[0]);
    if (a[1]) answer ^= galua_mul(32, a[1]);
    if (a[2]) answer ^= galua_mul(133, a[2]);
    if (a[3]) answer ^= galua_mul(16, a[3]);
    if (a[4]) answer ^= galua_mul(194, a[4]);
    if (a[5]) answer ^= galua_mul(192, a[5]);
    if (a[6]) answer ^= galua_mul(1, a[6]);
    if (a[7]) answer ^= galua_mul(251, a[7]);
    if (a[8]) answer ^= galua_mul(1, a[8]);
    if (a[9]) answer ^= galua_mul(192, a[9]);
    if (a[10]) answer ^= galua_mul(194, a[10]);
    if (a[11]) answer ^= galua_mul(16, a[11]);
    if (a[12]) answer ^= galua_mul(133, a[12]);
    if (a[13]) answer ^= galua_mul(32, a[13]);
    if (a[14]) answer ^= galua_mul(148, a[14]);
    if (a[15]) answer ^= galua_mul(1, a[15]);

    return answer;
}

void R_block(unsigned char block[16])
{
    unsigned char new_block[16];

    memcpy(new_block + 1 * sizeof(unsigned char), block, 15);
    new_block[0] = l_draft(block);
    memcpy(block, new_block, 16);
}

void L_block(unsigned char block[16])
{
    for (int i = 0; i < 16; i++)
    {
        R_block(block);
    }
}

void Feistel(unsigned char block[32], unsigned char key[16])
{
    unsigned char answer[32] = { 0 };

    unsigned char a1[16] = { 0 };
    unsigned char a0[16] = { 0 };

    memcpy(a1, block, 16);
    memcpy(a0, block + 16 * sizeof(unsigned char), 16);
    memcpy(answer + 16 * sizeof(unsigned char), a1, 16);

    X_block(a1, key, 16);
    S_block(a1, S);
    L_block(a1);
    X_block(a1, a0, 16);

    memcpy(answer, a1, 16);
    memcpy(block, answer, 32);
}

void key_schedule_Kuznechik(unsigned char* key_file_path)
{
    if (Kuznechik.keys)
    {
        for (int i = 0; i < 10; i++)
        {
            free(Kuznechik.keys[i]);
        }
        free(Kuznechik.keys);
    }

    Kuznechik.keys = (unsigned char**)malloc(10 * sizeof(unsigned char*));
    for (int i = 0; i < 10; i++)
    {
        Kuznechik.keys[i] = (unsigned char*)malloc(16 * sizeof(unsigned char));
    }

    unsigned char buffer[32];
    size_t buffer_len;

    FILE* key_file = fopen(key_file_path, "rb");

    if (!key_file)
    {
        printf("fopen(key_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 32, key_file);

    if (buffer_len < 32)
    {
        printf("fread(key_file) error\n");
        exit(0);
    }

    memcpy(Kuznechik.keys[0], buffer, 16);
    memcpy(Kuznechik.keys[1], buffer + 16 * sizeof(unsigned char), 16);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Feistel(buffer, c_const[(i << 3) + j]);
        }

        memcpy(Kuznechik.keys[(i + 1) << 1], buffer, 16);
        memcpy(Kuznechik.keys[((i + 1) << 1) + 1], buffer + 16 * sizeof(unsigned char), 16);
    }

    fclose(key_file);
};

void Kuznechik_ENC(unsigned char block[16])
{
    for (int i = 0; i < 9; i++)
    {
        X_block(block, Kuznechik.keys[i], 16);
        S_block(block, S);
        L_block(block);
    }
    X_block(block, Kuznechik.keys[9], 16);
}

void R_block_inv(unsigned char block[16])
{
    unsigned char new_block[16];

    memcpy(new_block, block + 1 * sizeof(unsigned char), 15);
    new_block[15] = block[0];
    new_block[15] = l_draft(new_block);
    memcpy(block, new_block, 16);
}

void L_block_inv(unsigned char block[16])
{
    for (int i = 0; i < 16; i++)
    {
        R_block_inv(block);
    }
}

void Kuznechik_DEC(unsigned char block[16])
{
    X_block(block, Kuznechik.keys[9], 16);

    for (int i = 8; i >= 0; i--)
    {
        L_block_inv(block);
        S_block(block, S_inv);
        X_block(block, Kuznechik.keys[i], 16);
    }
}

//----------------------------

unsigned char pi[8][16] = {
    { 0xc, 0x4, 0x6, 0x2, 0xa, 0x5, 0xb, 0x9, 0xe, 0x8, 0xd, 0x7, 0x0, 0x3, 0xf, 0x1 },
    { 0x6, 0x8, 0x2, 0x3, 0x9, 0xa, 0x5, 0xc, 0x1, 0xe, 0x4, 0x7, 0xb, 0xd, 0x0, 0xf },
    { 0xb, 0x3, 0x5, 0x8, 0x2, 0xf, 0xa, 0xd, 0xe, 0x1, 0x7, 0x4, 0xc, 0x9, 0x6, 0x0 },
    { 0xc, 0x8, 0x2, 0x1, 0xd, 0x4, 0xf, 0x6, 0x7, 0x0, 0xa, 0x5, 0x3, 0xe, 0x9, 0xb },
    { 0x7, 0xf, 0x5, 0xa, 0x8, 0x1, 0x6, 0xd, 0x0, 0x9, 0x3, 0xe, 0xb, 0x4, 0x2, 0xc },
    { 0x5, 0xd, 0xf, 0x6, 0x9, 0x2, 0xc, 0xa, 0xb, 0x7, 0x8, 0x1, 0x4, 0x3, 0xe, 0x0 },
    { 0x8, 0xe, 0x2, 0x5, 0x6, 0x9, 0x1, 0xc, 0xf, 0x4, 0xb, 0x0, 0xd, 0xa, 0x3, 0x7 },
    { 0x1, 0x7, 0xe, 0xd, 0x0, 0x5, 0x8, 0x3, 0x4, 0xf, 0xa, 0x6, 0x9, 0xc, 0xb, 0x2 }
};

void key_schedule_Magma(char* key_file_path)
{
    if (Magma.keys)
    {
        for (int i = 0; i < 32; i++)
        {
            free(Magma.keys[i]);
        }
        free(Magma.keys);
    }

    Magma.keys = (unsigned char**)malloc(32 * sizeof(unsigned char*));
    for (int i = 0; i < 32; i++)
    {
        Magma.keys[i] = (unsigned char*)malloc(4 * sizeof(unsigned char));
    }

    unsigned char buffer[32];
    size_t buffer_len;

    FILE* key_file = fopen(key_file_path, "rb");

    if (!key_file)
    {
        printf("fopen(key_file) error\n");
        exit(0);
    }

    buffer_len = fread(buffer, sizeof(unsigned char), 32, key_file);

    if (buffer_len < 32)
    {
        printf("fread(key_file) error\n");
        exit(0);
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            memcpy(Magma.keys[(i << 3) + j], buffer + (j << 2) * sizeof(unsigned char), 4);
        }
    }

    for (int j = 0; j < 8; j++)
    {
        memcpy(Magma.keys[24 + j], buffer + ((7 - j) << 2) * sizeof(unsigned char), 4);
    }

    fclose(key_file);
};

void t_draft(unsigned char block[4])
{
    unsigned char left, right;

    for (int i = 0; i < 4; i++)
    {
        left = block[i] >> 4;
        right = block[i] & 0xf;

        left = pi[7 - (i << 1)][left];
        right = pi[7 - (i << 1) - 1][right];

        block[i] = (left << 4) + right;
    }
}

void Vec32plusVec32(unsigned char Vec1[4], unsigned char Vec2[4])
{
    int ost = 0;

    for (int i = 3; i >= 0; i--)
    {
        ost += Vec1[i] + Vec2[i];
        Vec1[i] = ost & 0xff;
        ost >>= 8;
    }
}

void circle_shift(unsigned char block[4], int N)
{
    unsigned long long answer = 0;

    for (int i = 0; i < 4; i++)
    {
        answer = (answer << 8) + block[i];
    }

    answer <<= N & 0xffffffff;
    answer = (answer & 0xffffffff) + (answer >> 32);

    for (int i = 3; i >= 0; i--)
    {
        block[i] = answer & 0xff;
        answer >>= 8;
    }
}

void g_draft(unsigned char key[4], unsigned char block[4])
{
    Vec32plusVec32(block, key);
    t_draft(block);
    circle_shift(block, 11);
}

void G(unsigned char key[4], unsigned char block[8])
{
    unsigned char a1[4], a0[4];

    memcpy(a1, block, 4);
    memcpy(a0, block + 4 * sizeof(unsigned char), 4);

    memcpy(block, a0, 4);

    g_draft(key, a0);
    X_block(a1, a0, 4);

    memcpy(block + 4 * sizeof(unsigned char), a1, 4);
}

void G_star(unsigned char key[4], unsigned char block[8])
{
    unsigned char a1[4], a0[4];

    memcpy(a1, block, 4);
    memcpy(a0, block + 4 * sizeof(unsigned char), 4);

    memcpy(block + 4 * sizeof(unsigned char), a0, 4);

    g_draft(key, a0);
    X_block(a1, a0, 4);

    memcpy(block, a1, 4);
}

void Magma_ENC(unsigned char block[4])
{
    for (int i = 0; i < 31; i++)
    {
        G(Magma.keys[i], block);
    }
    G_star(Magma.keys[31], block);
}

void Magma_DEC(unsigned char block[4])
{
    for (int i = 31; i > 0; i--)
    {
        G(Magma.keys[i], block);
    }
    G_star(Magma.keys[0], block);
}

//----------------------------