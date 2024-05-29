typedef struct
{
    unsigned char* count;
    int size;
    int bits;
} BigInt;

void print_BigInt(BigInt* bigint)
{
    for (int i = bigint->size - 1, j = 0; i >= 0; i--, j++)
    {
        if (!(j & 0b11111)) printf("\n");
        printf("%02x", bigint->count[i]);
    }
    printf("\n%d, %d\n\n", bigint->size, bigint->bits);
}

BigInt* BigInt_init(int size)
{
    BigInt* result = (BigInt*)malloc(sizeof(BigInt));
    result->count = (unsigned char*)calloc(size, sizeof(unsigned char));
    result->size = size;
    result->bits = size << 3;
    return result;
}

void update_bits(BigInt* bigint)
{
    bigint->bits = bigint->size << 3;

    for (int i = 1 << 7; i > 0; i >>= 1)
    {
        if (!(bigint->count[bigint->size - 1] & i)) bigint->bits--;
        else break;
    }
}

void update_size(BigInt* bigint)
{
    int count = bigint->size;

    for (int i = bigint->size - 1; i >= 0; i--)
    {
        if (!(bigint->count[i])) count--;
        else break;
    }

    bigint->size = count;
}

void update_options(BigInt* bigint)
{
    update_size(bigint);
    update_bits(bigint);
}

BigInt* string_to_BigInt(char* number)
{
    int len;
    for (len = 0; number[len] != '\0'; len++);

    if (!(number[0] == '0' && number[1] == 'x'))
    {
        printf("Error: incorrect format of string. It must begin \"0x...\".\n");
        exit(0);
    }

    BigInt* result = BigInt_init(((len - 2) >> 1) + (len & 1));
    
    char symbol;

    for (int i = 2, j = ((result->size - 1) << 1) + !(len & 1); i < len; i++, j--)
    {
        symbol = number[i];

        if (symbol >= '0' && symbol <= '9')
            symbol -= 48;
        else if (symbol >= 'A' && symbol <= 'F')
            symbol -= 55;
        else if (symbol >= 'a' && symbol <= 'f')
            symbol -= 87;
        else
        {
            printf("Error: incorrect string.\n");
            exit(0);
        }

        result->count[j >> 1] = (result->count[j >> 1] << 4) + symbol;
    }

    update_options(result);
    return result;
}

void left_shift(BigInt* destination, BigInt* source, int N)
{
    if (N < 1)
    {
        memcpy(destination, source, sizeof(BigInt));
        return;
    }

    int new_size = source->bits + N;
    new_size = (new_size >> 3) + ((new_size & 0b111) > 0 ? 1 : 0);
    BigInt *result = BigInt_init(new_size);
    result->bits = source->bits + N;
    int i = N >> 3;

    N &= 0b111;

    int buffer = 0;
    for (int j = 0; j < source->size; i++, j++)
    {
        buffer = (source->count[j] << N) + buffer;
        result->count[i] = buffer & 0xff;
        buffer >>= 8;
    }
    result->count[i] = buffer;
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

void right_shift(BigInt* destination, BigInt* source, int N)
{
    if (N < 1)
    {
        memcpy(destination, source, sizeof(BigInt));
        return;
    }
    
    int byte = N >> 3;
    BigInt* result = BigInt_init(source->size);

    if (byte >= source->size)
    {
        update_options(result);
        memcpy(destination, result, sizeof(BigInt));
        return;
    }

    N &= 0b111;

    int buffer = 0;

    result->count[0] = source->count[byte] >> N;
    for (int i = 1, j = byte + 1; i < source->size - byte; i++, j++)
    {
        result->count[i - 1] += source->count[j] << (8 - N);
        result->count[i] = source->count[j] >> N;
    }

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

int cmp_BigInt(BigInt* a, BigInt* b)
{
    if (a->bits > b->bits)
        return -1;
    if (a->bits < b->bits)
        return 1;

    int i = a->size - 1;

    while (a->count[i] == b->count[i])
    {
        i--;
        if (i < 0) return 0;
    }
    
    if (a->count[i] > b->count[i]) return -1;
    if (a->count[i] < b->count[i]) return 1;
}

void sub_BigInt(BigInt* a, BigInt* b, BigInt* destination)
{
    if (cmp_BigInt(a, b) == 1)
    {
        printf("Error: first argument is less than second.\n");
        exit(0);
    }

    int mark = 0, current_byte;
    BigInt* result = BigInt_init(a->size);

    for (int i = 0; i < a->size; i++)
    {
        current_byte = a->count[i] - mark;
        if (i < b->size) current_byte -= b->count[i];

        if (current_byte < 0)
        {
            result->count[i] = 256 + current_byte;
            mark = 1;
        }
        else
        {
            result->count[i] = current_byte;
            mark = 0;
        }
    }

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

void add_BigInt(BigInt* first, BigInt* second, BigInt* destination)
{
    BigInt* a = BigInt_init(0);
    BigInt* b = BigInt_init(0);

    if (first->size >= second->size)
    {
        memcpy(a, first, sizeof(BigInt));
        memcpy(b, second, sizeof(BigInt));
    }
    else
    {
        memcpy(b, first, sizeof(BigInt));
        memcpy(a, second, sizeof(BigInt));
    }

    int current_byte = 0;
    BigInt* result = BigInt_init(a->size + 1);

    for (int i = 0; i < a->size; i++)
    {
        current_byte += a->count[i];
        if (i < b->size) current_byte += b->count[i];

        result->count[i] = current_byte & 0xff;
        current_byte >>= 8;
    }
    result->count[result->size - 1] = current_byte;

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(a);
    free(b);
    free(result);
}

void inc_BigInt(BigInt* a, BigInt* destination)
{
    BigInt* result = BigInt_init(a->size + 1);
    memcpy(result->count, a->count, a->size);
    int i = 0;

    for (int i = 0; i < result->size; i++)
    {
        result->count[i]++;
        if (result->count[i]) break;
    }

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

void mod_BigInt(BigInt* a_, BigInt* b, BigInt* destination)
{
    if (cmp_BigInt(a_, b) == 1)
    {
        memcpy(destination, a_, sizeof(BigInt));
        return;
    }

    if (b->size == 0)
    {
        printf("Error: division by zero.\n");
        exit(0);
    }

    BigInt* a = BigInt_init(a_->size);
    memcpy(a, a_, sizeof(BigInt));

    BigInt* c = BigInt_init(a->size);
    left_shift(c, b, a->bits - b->bits);

    while (cmp_BigInt(c, a) == -1)
        right_shift(c, c, 1);

    while (cmp_BigInt(b, a) >= 0)
    {
        right_shift(c, c, c->bits - a->bits);
        if (cmp_BigInt(c, a) == -1)
            right_shift(c, c, 1);
        sub_BigInt(a, c, a);
    }

    update_options(a);
    memcpy(destination, a, sizeof(BigInt));
    free(a);
    free(c);
}

void mul_BigInt(BigInt* a, BigInt* b, BigInt* destination)
{
    if (a->size == 0 || b->size == 0)
    {
        BigInt* result = BigInt_init(0);
        memcpy(destination, result, sizeof(BigInt));
        free(result);
        return;
    }

    int bit;

    BigInt* result = BigInt_init(a->size + b->size);
    memcpy(result->count, a->count, a->size);

    for (bit = 1 << 7; (bit & b->count[b->size - 1]) == 0; bit >>= 1);
    bit >>= 1;

    for (int i = b->size - 1; i >= 0; i--)
    {
        while (bit > 0)
        {
            left_shift(result, result, 1);
            if (b->count[i] & bit) add_BigInt(result, a, result);
            bit >>= 1;
        }

        bit = 1 << 7;
    }

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

void inv_by_mod(BigInt* y, BigInt* p, BigInt* destination)
{
    BigInt* a = BigInt_init(p->size);
    memcpy(a, y, sizeof(BigInt));

    BigInt* b = BigInt_init(p->size);
    memcpy(b, p, sizeof(BigInt));

    BigInt* u = BigInt_init(p->size);
    inc_BigInt(u, u);

    BigInt* v = BigInt_init(p->size);
    BigInt* tmp = BigInt_init(p->size);

    update_bits(u);
    update_bits(v);

    while (a->bits)
    {
        if ((a->count[0] & 1) == 0)
        {
            right_shift(a, a, 1);
            if (u->count[0] & 1) add_BigInt(u, p, u);
            right_shift(u, u, 1);
        }
        else
        {
            if (cmp_BigInt(a, b) == 1)
            {
                memcpy(tmp, a, sizeof(BigInt));
                memcpy(a, b, sizeof(BigInt));
                memcpy(b, tmp, sizeof(BigInt));

                memcpy(tmp, u, sizeof(BigInt));
                memcpy(u, v, sizeof(BigInt));
                memcpy(v, tmp, sizeof(BigInt));
            }

            sub_BigInt(a, b, a);
            right_shift(a, a, 1);

            if (cmp_BigInt(u, v) == 1)
            {
                sub_BigInt(p, v, tmp);
                add_BigInt(u, tmp, u);
            }
            else sub_BigInt(u, v, u);

            if (u->count[0] & 1)
                add_BigInt(u, p, u);
            right_shift(u, u, 1);
        }
    }

    update_options(v);
    memcpy(destination, v, sizeof(BigInt));
    free(a);
    free(b);
    free(u);
    free(v);
    free(tmp);
}

void add_mod_p(BigInt* x, BigInt* y, BigInt* p, BigInt* destination)
{
    BigInt* result = BigInt_init(x->size + y->size);
    memcpy(result->count, x->count, x->size);

    add_BigInt(result, y, result);
    mod_BigInt(result, p, result);

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
}

void sub_mod_p(BigInt* x_, BigInt* y_, BigInt* p, BigInt* destination)
{
    BigInt* result = BigInt_init(x_->size + y_->size);
    BigInt* x = BigInt_init(x_->size);
    BigInt* y = BigInt_init(y_->size);

    memcpy(x, x_, sizeof(BigInt));
    memcpy(y, y_, sizeof(BigInt));

    mod_BigInt(x, p, x);
    mod_BigInt(y, p, y);

    if (cmp_BigInt(x, y) == 1)
    {
        sub_BigInt(p, y, result);
        add_BigInt(result, x, result);
    }
    else sub_BigInt(x, y, result);

    update_options(result);
    memcpy(destination, result, sizeof(BigInt));
    free(result);
    free(x);
    free(y);
}