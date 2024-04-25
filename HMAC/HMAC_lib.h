#define bytes_count 64

extern unsigned char ipad[];

extern unsigned char opad[];

unsigned char* HMAC(unsigned char* K_file_path, unsigned char* T_file_path, unsigned char* output_file_path, int bytes);