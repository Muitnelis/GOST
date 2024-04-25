#define bytes_count 64

extern unsigned char S[];

extern unsigned char P[];

extern unsigned char A[bytes_count][8];

extern unsigned char C[12][bytes_count];

void print_block(unsigned char* block, unsigned char bytes);

void reverse(unsigned char* block, int N);

void bit_form(unsigned char* block, int size);

void byte_form(unsigned char* block, int size);

void S_block(unsigned char* block);

void P_block(unsigned char* block);

void X_block(unsigned char* block, unsigned char* key);

void l_draft(unsigned char* b);

void L_block(unsigned char* block);

void key_schedule(unsigned char* key, unsigned char keys[13][bytes_count]);

void E(unsigned char* key, unsigned char* block);

void g_(unsigned char* n, unsigned char* h, unsigned char* m);

void Vec512plusVec512(unsigned char* Vec1, unsigned char* Vec2);

void complete(unsigned char* block, size_t len);

unsigned char* IntToVec(int IntA, int size);

unsigned char* streebog(unsigned char* input_file_path, unsigned char* output_file_path, int bytes);