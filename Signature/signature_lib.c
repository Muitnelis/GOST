#include "signature_lib.h"

int bytes = 512 >> 3;

curva curva_init(BigInt* p, BigInt* a, BigInt* b, BigInt* m, BigInt* q)
{
    curva E;

    if (p)
    {
        E.p = BigInt_init(p->size);
        copy_BigInt(&E.p, p);
    }
    else E.p = BigInt_init(bytes);

    if (a)
    {
        E.a = BigInt_init(a->size);
        copy_BigInt(&E.a, a);
    }
    else E.a = BigInt_init(bytes);

    if (b)
    {
        E.b = BigInt_init(b->size);
        copy_BigInt(&E.b, b);
    }
    else E.b = BigInt_init(bytes);

    if (m)
    {
        E.m = BigInt_init(m->size);
        copy_BigInt(&E.m, m);
    }
    else E.m = BigInt_init(bytes);

    if (q)
    {
        E.q = BigInt_init(q->size);
        copy_BigInt(&E.q, q);
    }
    else E.q = BigInt_init(bytes);

    return E;
}

point point_init(BigInt* x, BigInt* y)
{
    point P;

    if (x)
    {
        P.x = BigInt_init(x->size);
        copy_BigInt(&P.x, x);
    }
    else P.x = BigInt_init(bytes);

    if (y)
    {
        P.y = BigInt_init(y->size);
        copy_BigInt(&P.y, y);
    }
    else P.y = BigInt_init(bytes);

    return P;
}

void copy_point(point* destination, point* source)
{
    destination->x = source->x;
    destination->y = source->y;
}

void add_points(curva* E, point* P1, point* P2, point* P3)
{
    BigInt Jl = BigInt_init(64);
    BigInt tmp = BigInt_init(64);

    point result = point_init(NULL, NULL);

    if (cmp_BigInt(&P1->x, &P2->x) || cmp_BigInt(&P1->y, &P2->y))
    {
        sub_mod_p(&P2->y, &P1->y, &E->p, &Jl);
        sub_mod_p(&P2->x, &P1->x, &E->p, &tmp);
        inv_by_mod(&tmp, &E->p, &tmp);
        mul_mod_p(&Jl, &tmp, &E->p, &Jl);
    }
    else
    {
        mul_mod_p(&P1->x, &P1->x, &E->p, &tmp);
        add_mod_p(&Jl, &tmp, &E->p, &Jl);
        add_mod_p(&Jl, &tmp, &E->p, &Jl);
        add_mod_p(&Jl, &tmp, &E->p, &Jl);
        add_mod_p(&Jl, &E->a, &E->p, &Jl);
        add_mod_p(&P1->y, &P1->y, &E->p, &tmp);
        inv_by_mod(&tmp, &E->p, &tmp);
        mul_mod_p(&Jl, &tmp, &E->p, &Jl);
    }

    mul_mod_p(&Jl, &Jl, &E->p, &result.x);
    sub_mod_p(&result.x, &P1->x, &E->p, &result.x);
    sub_mod_p(&result.x, &P2->x, &E->p, &result.x);

    sub_mod_p(&P1->x, &result.x, &E->p, &result.y);
    mul_mod_p(&result.y, &Jl, &E->p, &result.y);
    sub_mod_p(&result.y, &P1->y, &E->p, &result.y);

    update_options(&result.x);
    update_options(&result.y);

    copy_point(P3, &result);
}

void mul_point(curva* E, point* P, BigInt* k, point* Q)
{
    int bit;

    point result = point_init(&P->x, &P->y);

    for (bit = 1 << 7; (bit & k->count[k->size - 1]) == 0; bit >>= 1);
    bit >>= 1;

    int i = k->size - 1;
    if (bit == 0)
    {
        bit = 1 << 7;
        i--;
    }

    for (; i >= 0; i--)
    {
        while (bit > 0)
        {
            add_points(E, &result, &result, &result);
            if (k->count[i] & bit) add_points(E, &result, P, &result);
            bit >>= 1;
        }

        bit = 1 << 7;
    }

    update_options(&result.x);
    update_options(&result.y);
    copy_point(Q, &result);
}

void random(BigInt* max, BigInt* destination)
{
    srand(time(NULL));

    BigInt k = BigInt_init(max->size);

    if (max->size > 0)
    {
        k.count[0] = rand() & 0xff;
        if (k.count[0] == 0) k.count[0]++;
    }

    for (int i = 1; i < max->size; i++) k.count[i] = rand() & 0xff;

    if (k.count[k.size - 1] > max->count[max->size - 1]) k.count[k.size - 1] -= max->count[max->size - 1];

    update_options(&k);

    copy_BigInt(destination, &k);
}

void sign(char* msg, curva* E, point* P, BigInt* d)
{
    streebog(msg, "sign_hash", bytes << 3);

    BigInt buffer = BigInt_init(bytes);
    int buffer_len;

    FILE* input_file = fopen("sign_hash", "rb");
    buffer_len = fread(buffer.count, sizeof(unsigned char), bytes, input_file);
    fclose(input_file);

    BigInt e = BigInt_init(bytes);
    mod_BigInt(&buffer, &E->q, &e);
    if (e.bits == 0) e.count[0] = 1;

    BigInt k = BigInt_init(bytes);
    BigInt r = BigInt_init(bytes);
    BigInt s = BigInt_init(bytes);

    point C = point_init(NULL, NULL);

generate_k:
    random(&E->q, &k);
    mul_point(E, P, &k, &C);
    mod_BigInt(&C.x, &E->q, &r);
    if (r.bits == 0) goto generate_k;

    mul_mod_p(&r, d, &E->q, &s);
    mul_mod_p(&k, &e, &E->q, &buffer);
    add_mod_p(&s, &buffer, &E->q, &s);
    if (s.bits == 0) goto generate_k;

    FILE* output_file = fopen("sign", "wb");

    print_BigInt(r);
    print_BigInt(s);

    fwrite(r.count, sizeof(unsigned char), bytes, output_file);
    fwrite(s.count, sizeof(unsigned char), bytes, output_file);
    fclose(output_file);
}

void verify(char* msg, char* sign, curva* E, point* P, BigInt* d)
{
    BigInt buffer = BigInt_init(bytes);
    int buffer_len;

    BigInt r = BigInt_init(bytes);
    BigInt s = BigInt_init(bytes);

    FILE* sign_file = fopen(sign, "rb");
    buffer_len = fread(r.count, sizeof(unsigned char), bytes, sign_file);
    buffer_len = fread(s.count, sizeof(unsigned char), bytes, sign_file);
    fclose(sign_file);

    update_options(&r);
    update_options(&s);

    streebog(msg, "prov_hash", bytes << 3);

    FILE* input_file = fopen("prov_hash", "rb");
    buffer_len = fread(buffer.count, sizeof(unsigned char), bytes, input_file);
    fclose(input_file);

    BigInt e = BigInt_init(bytes);
    BigInt v = BigInt_init(bytes);
    BigInt z1 = BigInt_init(bytes);
    BigInt z2 = BigInt_init(bytes);

    mod_BigInt(&buffer, &E->q, &e);
    if (e.bits == 0) e.count[0] = 1;

    inv_by_mod(&e, &E->q, &v);
    mul_mod_p(&s, &v, &E->q, &z1);
    mul_mod_p(&r, &v, &E->q, &z2);
    sub_mod_p(&E->q, &z2, &E->q, &z2);

    point Q = point_init(NULL, NULL);
    mul_point(E, P, d, &Q);

    point C = point_init(NULL, NULL);
    mul_point(E, P, &z1, &C);

    point tmp_point = point_init(NULL, NULL);
    mul_point(E, &Q, &z2, &tmp_point);

    add_points(E, &C, &tmp_point, &C);

    BigInt R = BigInt_init(bytes);
    mod_BigInt(&C.x, &E->q, &R);

    print_BigInt(s);
    print_BigInt(r);
    print_BigInt(R);

    if (cmp_BigInt(&r, &R) == 0) printf("YES\n");
    else printf("NO\n");
}

//int main()
//{
//    int bits = 256;
//    bytes = bits >> 3;
//
//    /*BigInt p = string_to_BigInt("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DF1D852741AF4704A0458047E80E4546D35B8336FAC224DD81664BBF528BE6373");
//    BigInt a = string_to_BigInt("0x7");
//    BigInt b = string_to_BigInt("0x1CFF0806A31116DA29D8CFA54E57EB748BC5F377E49400FDD788B649ECA1AC4361834013B2AD7322480A89CA58E0CF74BC9E540C2ADD6897FAD0A3084F302ADC");
//    BigInt m = string_to_BigInt("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DA82F2D7ECB1DBAC719905C5EECC423F1D86E25EDBE23C595D644AAF187E6E6DF");
//    BigInt q = string_to_BigInt("0x4531ACD1FE0023C7550D267B6B2FEE80922B14B2FFB90F04D4EB7C09B5D2D15DA82F2D7ECB1DBAC719905C5EECC423F1D86E25EDBE23C595D644AAF187E6E6DF");
//    curva E = curva_init(&p, &a, &b, &m, &q);
//
//    BigInt x = string_to_BigInt("0x24D19CC64572EE30F396BF6EBBFD7A6C5213B3B3D7057CC825F91093A68CD762FD60611262CD838DC6B60AA7EEE804E28BC849977FAC33B4B530F1B120248A9A");
//    BigInt y = string_to_BigInt("0x2BB312A43BD2CE6E0D020613C857ACDDCFBF061E91E5F2C3F32447C259F39B2C83AB156D77F1496BF7EB3351E1EE4E43DC1A18B91B24640B6DBB92CB1ADD371E");
//    point P = point_init(&x, &y);
//
//    BigInt d = string_to_BigInt("0xBA6048AADAE241BA40936D47756D7C93091A0E8514669700EE7508E508B102072E8123B2200A0563322DAD2827E2714A2636B7BFD18AADFC62967821FA18DD4");*/
//
//    BigInt p = string_to_BigInt("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD97");
//    BigInt a = string_to_BigInt("0xC2173F1513981673AF4892C23035A27CE25E2013BF95AA33B22C656F277E7335");
//    BigInt b = string_to_BigInt("0x295F9BAE7428ED9CCC20E7C359A9D41A22FCCD9108E17BF7BA9337A6F8AE9513");
//    BigInt m = string_to_BigInt("0x01000000000000000000000000000000003F63377F21ED98D70456BD55B0D8319C");
//    BigInt q = string_to_BigInt("0x400000000000000000000000000000000FD8CDDFC87B6635C115AF556C360C67");
//    curva E = curva_init(&p, &a, &b, &m, &q);
//
//    BigInt x = string_to_BigInt("0x91E38443A5E82C0D880923425712B2BB658B9196932E02C78B2582FE742DAA28");
//    BigInt y = string_to_BigInt("0x32879423AB1A0375895786C4BB46E9565FDE0B5344766740AF268ADB32322E5C");
//    point P = point_init(&x, &y);
//
//    BigInt d = string_to_BigInt("0xBA6048AADAE241BA40936D47756D7C93091A0E8514669700EE7508E508B10207");
//
//    time_t t = time(NULL);
//    sign("text.txt", &E, &P, &d);
//    printf("Time - %d\n", time(NULL) - t);
//
//    t = time(NULL);
//    verify("text.txt", "sign", &E, &P, &d);
//    printf("Time - %d\n", time(NULL) - t);
//
//    return 0;
//}