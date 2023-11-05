/*************************************************HEADER***************************************************/
/*
Name: Carlos Gross-Martinez                              Z:23226341
Due Date: 10/17/2020                                     Course: CDA5326
Assignment: Assignment 2b                                Professor: Dr. Reza Azarderakhsh
*/
/***********************************************************************************************************/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BENCH_LOOPS 10000

typedef uint64_t bigint256[16]; //for operands
typedef uint64_t bigint512[32]; //for multiplication result

const bigint256 a_exp = { 0x13ab,0xfdb2,0xd231,0xc1b3,0xb1d0,0x0867,0x14d8,0x0102,0x5c99,0x380b,0x658a,0xc279,0x9b97,0x02d1,0xda40,0x52fa };
const bigint256 b_exp = { 0x79f5,0x5500,0xda19,0xed75,0xe4a0,0x48b6,0x695f,0x0ee7,0x6bee,0x52ca,0xdfa5,0xaeac,0x9d49,0x03f1,0xeb9b,0xc4a4 };

/* SageMath Code 1(a)
    a = [0x13ab,0xfdb2,0xd231,0xc1b3,0xb1d0,0x0867,0x14d8,0x0102,0x5c99,0x380b,0x658a,0xc279,0x9b97,0x02d1,0xda40,0x52fa]
    b = [0x79f5,0x5500,0xda19,0xed75,0xe4a0,0x48b6,0x695f,0x0ee7,0x6bee,0x52ca,0xdfa5,0xaeac,0x9d49,0x03f1,0xeb9b,0xc4a4]
    carry = 0

    for i in range(0,16):
        sum = a[i] + b[i] + carry
        carry = sum >> 16
        print(hex(sum & 0xffff))
*/

// Update the sum here
const bigint256 add_exp = { 0x8da0, 0x52b2, 0xac4b, 0xaf29, 0x9671, 0x511e, 0x7e37, 0xfe9, 0xc887, 0x8ad5, 0x452f, 0x7126, 0x38e1, 0x6c3, 0xc5db, 0x179f };


/* SageMath Code 2(a)
    a = [0x13ab,0xfdb2,0xd231,0xc1b3,0xb1d0,0x0867,0x14d8,0x0102,0x5c99,0x380b,0x658a,0xc279,0x9b97,0x02d1,0xda40,0x52fa]
    b = [0x79f5,0x5500,0xda19,0xed75,0xe4a0,0x48b6,0x695f,0x0ee7,0x6bee,0x52ca,0xdfa5,0xaeac,0x9d49,0x03f1,0xeb9b,0xc4a4]
    borrow = 0

    for i in range(0,16):
        difference = a[i] - b[i] - borrow
        borrow = (difference >> 16) & 0x1
        difference = (difference + (borrow * 0x10000)) & 0xffff
        print(hex(difference))
*/

// Update the difference here
const bigint256 sub_exp = { 0x99b6, 0xa8b1, 0xf818, 0xd43d, 0xcd2f, 0xbfb0, 0xab78, 0xf21a, 0xf0aa, 0xe540, 0x85e4, 0x13cc, 0xfe4e, 0xfedf, 0xeea4, 0x8e55 };


/* SageMath Code 3(a)
    a = [0x13ab,0xfdb2,0xd231,0xc1b3,0xb1d0,0x0867,0x14d8,0x0102,0x5c99,0x380b,0x658a,0xc279,0x9b97,0x02d1,0xda40,0x52fa]
    b = [0x79f5,0x5500,0xda19,0xed75,0xe4a0,0x48b6,0x695f,0x0ee7,0x6bee,0x52ca,0xdfa5,0xaeac,0x9d49,0x03f1,0xeb9b,0xc4a4]
    r = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]

    for i in range(0,16):
          for j in range(0,16):
              r[i + j] = a[i] * b[j]

    for k in range(0,32):
          print(hex(r[k]))
*/

const bigint512 mul_exp = { 0x0095ea5a7,0x07f63b45a,0x0c91ff598,0x18c71e6d8,0x244fa7f13,0x28f2eba45,0x263bdd778,0x205bef01a,
                            0x18963b6a1,0x154ed05bf,0x1b81ca315,0x299508e38,0x36ba623b7,0x398b26e1e,0x3f4c64d71,0x3e45ae57b,
                            0x3fb328c12,0x336181563,0x32fec692a,0x206882c5c,0x1c38aa08c,0x186558271,0x2039d413f,0x1c9efc02d,
                            0x1c681de82,0x1e281238d,0x1e3534cea,0x0b07209d1,0x0cc4dc1fe,0x0f4028c5e,0x03fbc9028,0x000000000 };


/*
// The following function get the time-stamp from processor, only works on Unix machines
int64_t cpucycles(void) {
    unsigned int hi, lo;

    asm volatile ("rdtsc\n\t" : "=a" (lo), "=d"(hi));
    return ((int64_t)lo) | (((int64_t)hi) << 32);
}*/

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

// r = a + b
void add256(bigint256 r, const bigint256 a, const bigint256 b) {
    int i;
    uint8_t carry = 0;

    // Write 1b here

    for (i = 0; i < 16; i++)
    {
        r[i] = a[i] + b[i] + carry;
        carry = r[i] >> 16;
        r[i] = r[i] & 0xffff;
    }
}

// r = a - b
void sub256(bigint256 r, const bigint256 a, const bigint256 b) {
    int i;
    uint8_t borrow = 0;

    // Write 2b here

    for (i = 0; i < 16; i++)
    {
        r[i] = a[i] - b[i] - borrow;
        borrow = (r[i] >> 16) & 0x1;
        r[i] = (r[i] + (borrow * 0x10000)) & 0xFFFF;
    }

}

// r = a * b using schoolbook method
void schoolbook_mul256(bigint512 r, const bigint256 a, const bigint256 b) {
    int i, j;
    // Initialize r with 0s
    memset(r, 0, 256);

    // Write 3b here

    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 16; j++)
        {
            r[i + j] += a[i] * b[j];
        }
    }
}


// r = a * b using comba method
void comba_mul256(bigint512 r, const bigint256 a, const bigint256 b) {
    int i, j;

    // Write 3c here
    for (i = 0; i < 16; i++)
    {
        r[i] = 0;
        for (j = 0; j <= i; j++)
        {
            r[i] += a[j] * b[i - j];
        }
    }

    for (i = 16; i < 31; i++)
    {
        r[i] = 0;
        for (j = i - 15; j < 16; j++)
        {
            r[i] += a[j] * b[i - j];
        }
    }

    r[31] = 0;
}

// r = a * b using Karatsuba method
void karatsuba_mul256(bigint512 r, const bigint256 a, const bigint256 b) {
    int i, j;

  /*  //initialize arrays
    uint64_t ma[8];
    uint64_t mb[8];
    uint64_t z0[16] = { 0 };
    uint64_t z1[16] = { 0 };
    uint64_t z2[16] = { 0 };

    // Write 3d here
    // Compute ma, mb
    ma[0] = a[2] + a[0];
    ma[1] = a[3] + a[1];

    mb[0] = b[2] + b[0];
    mb[1] = b[3] + b[1];

    // Compute z1, z2, z3
    z0[0] = a[0] * b[0];
    z0[1] = a[0] * b[1];
    z0[1] += a[1] * b[0];
    z0[2] = a[1] * b[1];

    z1[0] = ma[0] * mb[0];
    z1[1] = ma[0] * mb[1];
    z1[1] += ma[1] * mb[0];
    z1[2] = ma[1] * mb[1];

    z2[0] = a[2] * b[2];
    z2[1] = a[2] * b[3];
    z2[1] += a[3] * b[2];
    z2[2] = a[3] * b[3];

    // Perform subtraction z1 = z1 - z0 - z2
    z1[0] = z1[0] – z0[0] – z2[0];
    z1[1] = z1[1] – z0[1] – z2[1];
    z1[2] = z1[2] – z0[2] – z2[2];

    // Implement final addition and put the result in r
    r[0] = z0[0];
    r[1] = z0[1];
    r[2] = z0[2] + z1[0];
    r[3] = z1[1];
    r[4] = z1[2] + z2[0];
    r[5] = z2[1];
    r[6] = z2[2];
    r[7] = 0;

}*/

int main() {
    int pass = 0;

    bigint256 add_res;
    bigint256 sub_res;
    bigint512 mul1_res;
    bigint512 mul2_res;
    bigint512 mul3_res;

    printf("a = "); bigint256_print(a_exp); printf("\n");
    printf("b = "); bigint256_print(b_exp); printf("\n\n");

    //ex.1
    printf("###############################\n");
    printf("Exercise 1:\n");
    printf("###############################\n");
    add256(add_res, a_exp, b_exp);
    pass = !memcmp(add_res, add_exp, sizeof(uint64_t) * 16);
    printf("a + b: \n");
    printf("expect = "); bigint256_print(add_exp); printf("\n");
    printf("result = "); bigint256_print(add_res); printf("\n");
    printf("passed = %d\n\n", pass);

    // ex.2
    printf("###############################\n");
    printf("Exercise 2:\n");
    printf("###############################\n");
    sub256(sub_res, a_exp, b_exp);
    pass = !memcmp(sub_res, sub_exp, sizeof(uint64_t) * 16);
    printf("a - b: \n");
    printf("expect = "); bigint256_print(sub_exp); printf("\n");
    printf("result = "); bigint256_print(sub_res); printf("\n");
    printf("passed = %d\n", pass);
    printf("###############################\n\n");

    // ex. 3
    printf("###############################\n");
    printf("Exercise 3:\n");
    printf("###############################\n");
    schoolbook_mul256(mul1_res, a_exp, b_exp);
    pass = !memcmp(mul1_res, mul_exp, sizeof(uint64_t) * 32);
    printf("schoolbook: \n");
    printf("expect = "); bigint512_print(mul_exp); printf("\n");
    printf("result = "); bigint512_print(mul1_res); printf("\n");
    printf("passed = %d\n\n", pass);

    comba_mul256(mul2_res, a_exp, b_exp);
    pass = !memcmp(mul2_res, mul_exp, sizeof(uint64_t) * 32);
    printf("comba: \n");
    printf("expect = "); bigint512_print(mul_exp); printf("\n");
    printf("result = "); bigint512_print(mul2_res); printf("\n");
    printf("passed = %d\n\n", pass);

    karatsuba_mul256(mul3_res, a_exp, b_exp);
    pass = !memcmp(mul3_res, mul_exp, sizeof(uint64_t) * 32);
    printf("karatsuba: \n");
    printf("expect = "); bigint512_print(mul_exp); printf("\n");
    printf("result = "); bigint512_print(mul3_res); printf("\n");
    printf("passed = %d\n", pass);
    printf("###############################\n\n");
/*
    //ex.4
    int i;
    bigint256 a_rand;
    bigint256 b_rand;
    bigint512 c_rand;
    unsigned long long cycles, cycles1, cycles2;
    FILE* urandom = fopen("/dev/urandom", "r");  //for random input

    printf("###############################\n");
    printf("Exercise 4:\n");
    printf("###############################\n");

    cycles = 0;
    for (i = 0; i < BENCH_LOOPS; i++) {
        fread(a_rand, sizeof(uint64_t), 16, urandom);
        fread(b_rand, sizeof(uint64_t), 16, urandom);

        cycles1 = cpucycles();
        schoolbook_mul256(c_rand, a_rand, b_rand);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
    }
    printf("Schoolbook runs in %10llu cycles\n", cycles / BENCH_LOOPS);

    cycles = 0;
    for (i = 0; i < BENCH_LOOPS; i++) {
        fread(a_rand, sizeof(uint64_t), 16, urandom);
        fread(b_rand, sizeof(uint64_t), 16, urandom);

        cycles1 = cpucycles();
        comba_mul256(c_rand, a_rand, b_rand);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
    }
    printf("Comba runs in %10llu cycles\n", cycles / BENCH_LOOPS);

    cycles = 0;
    for (i = 0; i < BENCH_LOOPS; i++) {
        fread(a_rand, sizeof(uint64_t), 16, urandom);
        fread(b_rand, sizeof(uint64_t), 16, urandom);

        cycles1 = cpucycles();
        karatsuba_mul256(c_rand, a_rand, b_rand);
        cycles2 = cpucycles();
        cycles = cycles + (cycles2 - cycles1);
    }
    printf("Karatsuba runs in %10llu cycles\n", cycles / BENCH_LOOPS);

    fclose(urandom);
    printf("###############################\n\n");

    // Test bank:
    printf("###############################\n");
    printf("Test Bank:\n");
    printf("###############################\n");
    int j;
    bigint256 a_bank;
    bigint256 b_bank;
    bigint256 exp1_bank;
    bigint256 res1_bank;
    bigint256 exp2_bank;
    bigint256 res2_bank;
    bigint512 exp3_bank;
    bigint512 res3_bank;
    const int BANK_TOTAL = 1000;

    FILE* fbank = fopen("assign2b_bank", "r");

    if (fbank == NULL) {
        printf("'assign2b_bank' file is missing. Please include it in the same directory as where you execute the code\n");
        return 1;
    }

    // Test MUL bank
    for (j = 0; j < BANK_TOTAL; j++) {
        fread(a_bank, sizeof(uint64_t), 16, fbank);
        fread(b_bank, sizeof(uint64_t), 16, fbank);
        fread(exp1_bank, sizeof(uint64_t), 16, fbank);
        fread(exp2_bank, sizeof(uint64_t), 16, fbank);
        fread(exp3_bank, sizeof(uint64_t), 32, fbank);

        add256(res1_bank, a_bank, b_bank);
        if (memcmp(res1_bank, exp1_bank, sizeof(uint64_t) * 16)) {
            printf("add256 failed for these values:\n");
            printf("a = "); bigint256_print(a_bank); printf("\n");
            printf("b = "); bigint256_print(b_bank); printf("\n");
            printf("exp = "); bigint256_print(exp1_bank); printf("\n");
            printf("res = "); bigint256_print(res1_bank); printf("\n");
            return 2;
        }

        sub256(res2_bank, a_bank, b_bank);
        if (memcmp(res2_bank, exp2_bank, sizeof(uint64_t) * 16)) {
            printf("sub256 failed for these values:\n");
            printf("a   = "); bigint256_print(a_bank); printf("\n");
            printf("b   = "); bigint256_print(b_bank); printf("\n");
            printf("exp = "); bigint256_print(exp2_bank); printf("\n");
            printf("res = "); bigint256_print(res2_bank); printf("\n");
            return 2;
        }

        schoolbook_mul256(res3_bank, a_bank, b_bank);
        if (memcmp(res3_bank, exp3_bank, sizeof(uint64_t) * 32)) {
            printf("schoolbook_mul256 failed for these values:\n");
            printf("a   = "); bigint256_print(a_bank); printf("\n");
            printf("b   = "); bigint256_print(b_bank); printf("\n");
            printf("exp = "); bigint512_print(exp3_bank); printf("\n");
            printf("res = "); bigint512_print(res3_bank); printf("\n");
            return 2;
        }

        comba_mul256(res3_bank, a_bank, b_bank);
        if (memcmp(res3_bank, exp3_bank, sizeof(uint64_t) * 32)) {
            printf("comba_mul256 failed for these values:\n");
            printf("a   = "); bigint256_print(a_bank); printf("\n");
            printf("b   = "); bigint256_print(b_bank); printf("\n");
            printf("exp = "); bigint512_print(exp3_bank); printf("\n");
            printf("res = "); bigint512_print(res3_bank); printf("\n");
            return 2;
        }

        karatsuba_mul256(res3_bank, a_bank, b_bank);
        if (memcmp(res3_bank, exp3_bank, sizeof(uint64_t) * 32)) {
            printf("karatsuba_mul256 failed for these values:\n");
            printf("a   = "); bigint256_print(a_bank); printf("\n");
            printf("b   = "); bigint256_print(b_bank); printf("\n");
            printf("exp = "); bigint512_print(exp3_bank); printf("\n");
            printf("res = "); bigint512_print(res3_bank); printf("\n");
            return 2;
        }
    }

    printf("bank_passed = 1\n");*/

    return 0;
}
/* Output
*

*/
