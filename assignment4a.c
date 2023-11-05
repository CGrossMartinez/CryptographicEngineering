#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REAL_RANDOM 0

FILE* fbank;

#if REAL_RANDOM
#define RANDOM_DATA(ptr, size) \
FILE *frandom = fopen("/dev/urandom", "r"); \
fread((ptr), (size), 1, frandom); \
fclose(frandom);
#else
#define RANDOM_DATA(ptr, size) \
fread((ptr), (size), 1, fbank);
#endif // REAL_RANDOM


typedef uint64_t bigint256[16]; //for operands
typedef uint64_t bigint512[32]; //for multiplication result
/*
// The following function get the time-stamp from processor, only works on Unix machines
int64_t cpucycles(void) {
    unsigned int hi, lo;

    asm volatile ("rdtsc\n\t" : "=a" (lo), "=d"(hi));
    return ((int64_t)lo) | (((int64_t)hi) << 32);
}
*/
// Print 256 bit number as 1 hex integer (Helper)
void print_hex256(const char* s, const bigint256 a) {
    int i;
    printf("%s", s);
    for (i = 15; i >= 0; i--)
        printf("%04lx", a[i]);
    printf("\n");
}

// Read a 256-bit hex integer into bigint256 (Helper). 1 indicates output valid. 0 not valid.
int read_hex256(const char* s, const char* a, bigint256 r) {
    static const long hextable[] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1, 0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,-1,10,11,12,13,14,15,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
    };

    int i, j;
    uint8_t c;
    long hex_value;

    if (strlen(a) != 64) {
        printf("Error: Length of %s is (%ld). It should be 64.\n", s, strlen(a));
        return 0;
    }

    memset(r, 0, sizeof(bigint256));

    for (i = 15; i >= 0; i--) {
        for (j = 3; j >= 0; j--) {
            c = *a++;
            hex_value = hextable[c];
            if (hex_value == -1) {
                printf("Error: Found invalid hex digit '%c'.\n", c);
                return 0;
            }
            r[i] += hex_value << (4 * j);
        }
    }
    return 1;
}

// Print 256 bit number (Helper)
void bigint256_print(const bigint256 a) {
    int i;
    printf("{");
    for (i = 0; i < 15; i++)
        printf("0x%04lx,", a[i]);
    printf("0x%04lx}", a[i]);
}

//print 512 bit number (Helper)
void bigint512_print(const bigint512 a) {
    int i;
    printf("{");
    for (i = 0; i < 8; i++)
        printf("0x%09lx,", a[i]);
    printf("\n");
    for (i = 8; i < 16; i++)
        printf("0x%09lx,", a[i]);
    printf("\n");
    for (i = 16; i < 24; i++)
        printf("0x%09lx,", a[i]);
    printf("\n");
    for (i = 24; i < 31; i++)
        printf("0x%09lx,", a[i]);
    printf("0x%09lx}", a[i]);
}

// r = a % PRIME where  0 <= a < 2p
void mod_correct(bigint256 r, const bigint256 a) {
    int i;
    uint8_t carry;
    uint64_t mask;

    // a-p
    r[0] = a[0] + 0x0013;
    carry = r[0] >> 16;
    r[0] &= 0xffff;
    for (i = 1; i < 15; i++) {
        r[i] = a[i] + carry;
        carry = r[i] >> 16;
        r[i] &= 0xffff;
    }
    r[15] = a[15] + 0x18000 + carry;
    r[15] &= 0x1ffff;

    // Get the mask from sign
    mask = 0 - (r[15] >> 16);

    // a+(mask&p)
    r[0] = r[0] - (mask & 0x0013);
    carry = r[0] >> 63;
    r[0] &= 0xffff;
    for (i = 1; i < 15; i++) {
        r[i] = r[i] - carry;
        carry = r[i] >> 63;
        r[i] &= 0xffff;
    }
    r[15] = r[15] - (mask & 0x18000) - carry;
    r[15] &= 0x1ffff;
}

// r = a * b using Karatsuba method
void mul(bigint512 r, const bigint256 a, const bigint256 b) {
    int i, j;

    //initialize arrays
    uint64_t ma[8];
    uint64_t mb[8];
    uint64_t z0[16] = { 0 };
    uint64_t z1[16] = { 0 };
    uint64_t z2[16] = { 0 };

    // Write 3d here
    // Compute ma, mb
    for (i = 0; i < 8; i++) {
        ma[i] = a[i] + a[8 + i];
        mb[i] = b[i] + b[8 + i];
    }

    // Compute z0, z1, z2
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            z0[i + j] += a[i] * b[j];
            z1[i + j] += ma[i] * mb[j];
            z2[i + j] += a[8 + i] * b[8 + j];
        }
    }

    // Perform subtraction z1 = z1 - z0 - z2
    for (i = 0; i < 15; i++) // z0[15],z1[15],z2[15] are 0
        z1[i] = z1[i] - z0[i] - z2[i];


    // Implement final addition and put the result in r
    for (i = 0; i < 8; i++) {
        r[i] = z0[i];
        r[8 + i] = z0[8 + i] + z1[i];
        r[16 + i] = z1[8 + i] + z2[i];
        r[24 + i] = z2[8 + i];
    }
}

// r = a % 2PRIME using pseudo mersenne reduction
void psu_reduce(bigint256 r, const bigint512 a) {
    int i;
    uint64_t carry;

    // First round of pseudo-mersenne with carry propagation
    // a[0-15] + 38*a[16-31]
    carry = 0;
    for (i = 0; i < 15; i++) {
        r[i] = a[i] + 38 * a[16 + i] + carry;
        carry = r[i] >> 16;
        r[i] &= 0xffff;
    }
    r[i] = a[i] + 38 * a[16 + i] + carry;

    // Second round of pseudo-mersenne 
    r[0] = r[0] + 19 * (r[15] >> 15);
    r[15] &= 0x7fff; // Remove the high bits.

    // Perform carry propagation
    carry = 0;
    for (i = 0; i < 15; i++) {
        r[i] = r[i] + carry;
        carry = r[i] >> 16;
        r[i] &= 0xffff;
    }
    r[i] = r[i] + carry;
}

void mod_mul(bigint256 r, const bigint256 a, const bigint256 b) {
    bigint512 t;
    mul(t, a, b);
    psu_reduce(r, t);
}

void mod_sqr(bigint256 r, const bigint256 a) {
    bigint512 t;
    mul(t, a, a);
    psu_reduce(r, t);
}

// If option == 1, then r = b. If option == 0 then r = a.
void select_bigint256(bigint256 r, const bigint256 a, const bigint256 b, uint8_t option) {
    int i;
    uint64_t t;
    uint64_t mask;

    // Write 1 here
    mask = 0 - option;
    for (i = 0; i < 16; i++)
    {
        t = a[i] ^ b[i];
        t = mask & t;
        r[i] = a[i] ^ t;
    }


}

// r = a^e mod p
void mod_exp(bigint256 r, const bigint256 a, const bigint256 e) {
    int i, j;
    uint8_t bit;
    bigint256 t, t2;

    // Write 2 here (Do not write into r until the very end)
}

// Generate secret key and public key.
void keyGen(bigint256 sk, bigint256 pk) {
    // Generator
    const bigint256 g = { 0x4855,0xafb3,0xe21b,0x24a6,0x98f0,0x02cd,0xff08,0xaaa1,0x379a,0x461e,0x08ea,0xc8e8,0xa1dd,0x2cd7,0x1d71,0x4f62 };
    int i;

    // Write 3 here
    // Generate secret key here (first line is given to you)
    RANDOM_DATA(sk, sizeof(bigint256)); // This will read from the file once and fill sk with random data.


    // Generate public key here


}

// Generate shared secret from your secret key and other party's public key
void sharedSecret(bigint256 ss, const bigint256 sk, const bigint256 pk) {
    // Write 4 here
    // Compute shared secret here


}

int main(int argc, char* argv[]) {
#if REAL_RANDOM
    int rtn;
    bigint256 sk;
    bigint256 pk;
    bigint256 ss;

    if (argc < 2) {
        printf("Arguments are required:\n");
        printf("For keygen: pass 'keygen'\n");
        printf("For shared: pass 'shared sk pk' where sk and pk are secret and public key in hex, respectively\n");
        return 1;
    }

    if (!strcmp(argv[1], "keygen")) {
        keyGen(sk, pk);
        print_hex256("sk = ", sk);
        print_hex256("pk = ", pk);
    }
    else if (!strcmp(argv[1], "shared")) {
        if (argc < 4) {
            printf("2 additional arguments are required:\n");
            printf("'sk pk' where sk and pk are secret and public key in hex, respectively\n");
            return 1;
        }
        rtn = read_hex256("sk", argv[2], sk);
        if (!rtn) {
            printf("sk = %s", argv[2]);
            return 1;
        }
        rtn = read_hex256("pk", argv[3], pk);
        if (!rtn) {
            printf("pk = %s", argv[3]);
            return 1;
        }

        sharedSecret(ss, sk, pk);
        print_hex256("sk = ", sk);
        print_hex256("pk = ", pk);
        print_hex256("ss = ", ss);
    }
    else {
        printf("Invalid Argument '%s' : \n", argv[1]);
        printf("For keygen: pass 'keygen'\n");
        printf("For shared: pass 'shared sk pk' where sk and pk are secret and public key in hex, respectively\n");
        return 1;
    }
#else
    const int BANK_TOTAL = 1000;
    int i, pass;

    bigint256 a, b;
    uint8_t option = 0;
    bigint256 exp, act;

    bigint256 exp_sk, exp_pk, exp_ss;
    bigint256 act_sk, act_pk, act_ss;

    unsigned long long cycles, cycles1, cycles2;

    fbank = fopen("assign4a_bank", "r");

    if (fbank == NULL) {
        printf("'assign4a_bank' file is missing. Please copy it to the same directory as where you execute the code.\n");
        return 1;
    }

    fseek(fbank, sizeof(bigint256) * 6 * BANK_TOTAL, SEEK_SET);

    printf("###############################\n");
    printf("Exercise 1:\n");
    printf("###############################\n");
    pass = 1;
    cycles = 0;
    for (i = 0; i < BANK_TOTAL; i++) {
        fread(a, sizeof(bigint256), 1, fbank);
        fread(b, sizeof(bigint256), 1, fbank);
        fread(&option, 1, 1, fbank);
        fread(exp, sizeof(bigint256), 1, fbank);

        //cycles1 = cpucycles();
        select_bigint256(act, a, b, option);
       /*cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);*/
        if (memcmp(act, exp, sizeof(bigint256))) {
            printf("select_bigint256 failed for these values:\n");
            printf("a   = "); bigint256_print(a); printf("\n");
            printf("b   = "); bigint256_print(b); printf("\n");
            printf("option = %d\n", option);
            printf("exp = "); bigint256_print(exp); printf("\n");
            printf("act = "); bigint256_print(act); printf("\n");
            pass = 0;
            break;
        }
    }

    if (pass == 1) {
        printf("select_bigint256 passed!\n");
        printf("select_bigint256 runs in %10llu cycles\n", cycles / BANK_TOTAL);
    }
    printf("###############################\n\n");

    /*
    printf("###############################\n");
    printf("Exercise 2:\n");
    printf("###############################\n");
    pass = 1;
    cycles = 0;
    for (i = 0; i < BANK_TOTAL; i++) {
        fread(a, sizeof(bigint256), 1, fbank);
        fread(b, sizeof(bigint256), 1, fbank);
        fread(exp, sizeof(bigint256), 1, fbank);

        cycles1 = cpucycles();
        mod_exp(act, a, b);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
        if (memcmp(act, exp, sizeof(bigint256))) {
            printf("mod_exp failed for these values:\n");
            printf("a   = "); bigint256_print(a); printf("\n");
            printf("b   = "); bigint256_print(b); printf("\n");
            printf("exp = "); bigint256_print(exp); printf("\n");
            printf("res = "); bigint256_print(act); printf("\n");
            pass = 0;
            break;
        }
    }

    if (pass == 1) {
        printf("mod_exp passed!\n");
        printf("mod_exp runs in %10llu cycles\n", cycles / BANK_TOTAL);
    }
    printf("###############################\n\n");

    rewind(fbank);

    printf("###############################\n");
    printf("Exercise 3:\n");
    printf("###############################\n");
    pass = 1;
    cycles = 0;
    for (i = 0; i < BANK_TOTAL; i++) {
        fread(exp_sk, sizeof(bigint256), 1, fbank);
        fread(exp_pk, sizeof(bigint256), 1, fbank);

        cycles1 = cpucycles();
        keyGen(act_sk, act_pk);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
        if (memcmp(exp_sk, act_sk, sizeof(bigint256)) || memcmp(exp_pk, act_pk, sizeof(bigint256))) {
            printf("keyGen failed for these values:\n");
            printf("exp_sk   = "); bigint256_print(exp_sk); printf("\n");
            printf("exp_pk   = "); bigint256_print(exp_pk); printf("\n");
            printf("act_sk   = "); bigint256_print(act_sk); printf("\n");
            printf("act_pk   = "); bigint256_print(act_pk); printf("\n");
            pass = 0;
            break;
        }
    }

    if (pass == 1) {
        printf("keyGen passed!\n");
        printf("keyGen runs in %10llu cycles\n", cycles / BANK_TOTAL);
    }
    printf("###############################\n\n");


    printf("###############################\n");
    printf("Exercise 4:\n");
    printf("###############################\n");
    pass = 1;
    cycles = 0;
    for (i = 0; i < BANK_TOTAL; i++) {
        fread(exp_sk, sizeof(bigint256), 1, fbank);
        fread(exp_pk, sizeof(bigint256), 1, fbank);
        fread(exp_ss, sizeof(bigint256), 1, fbank);

        cycles1 = cpucycles();
        sharedSecret(act_ss, exp_sk, exp_pk);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
        if (memcmp(exp_ss, act_ss, sizeof(bigint256))) {
            printf("sharedSecret failed for these values:\n");
            printf("sk       = "); bigint256_print(exp_sk); printf("\n");
            printf("pk       = "); bigint256_print(exp_pk); printf("\n");
            printf("exp_ss   = "); bigint256_print(exp_ss); printf("\n");
            printf("act_ss   = "); bigint256_print(act_ss); printf("\n");
            pass = 0;
            break;
        }
    }

    if (pass == 1) {
        printf("sharedSecret passed!\n");
        printf("sharedSecret runs in %10llu cycles\n", cycles / BANK_TOTAL);
    }
    printf("###############################\n\n");
*/
    fclose(fbank);
#endif // REAL_RANDOM

    return 0;
}

/* Output
###############################
Exercise 1:
###############################
select_bigint256 passed!
select_bigint256 runs in        250 cycles
###############################

###############################
Exercise 2:
###############################
mod_exp passed!
mod_exp runs in     875274 cycles
###############################

###############################
Exercise 3:
###############################
keyGen passed!
keyGen runs in     869152 cycles
###############################

###############################
Exercise 4:
###############################
sharedSecret passed!
sharedSecret runs in     881207 cycles
###############################
*/