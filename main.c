#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// a mod p1
uint32_t modp1(uint32_t a)
{
    const uint32_t p1 = 0x0001FFFF;
    uint32_t r = 0;

    // Write 1a here
    r = (a & p1) + (a >> 17);
    
    if (r > p1)
        r = r - p1;

    return r;
}

// a mod p2
uint32_t modp2(uint32_t a)
{
    const uint32_t p2 = 0x03FFFFFB;
    uint32_t r;

    // Write 1b here
    r = ((a << 6) >> 6) + ((a >> 26) * 5);

    if (r > p2)
        r = r - p2;

    return r;
}

// a mod b
uint32_t modb(uint32_t a) {
    const uint32_t b = 0x00010000;
    uint32_t r;

    // Write 1c here
    r = (a << 16) >> 16;

    return r;
}


// a^-1 mod p1 using FLT
uint32_t FLT(uint32_t a) {
    const uint32_t p1 = 0x0001FFFF;
    int i;
    uint32_t r = a;


    // Write 2a here

    for (i = 1; i < p1-2; i++)
        {             
            r = r * a;
            r = modp1(r);
        }  

    return r;
}

// a^-1 mod p1 using EEA
uint32_t EEA(uint32_t in) {
    const uint32_t p1 = 0x0001FFFF;
    int a = p1;
    int b = in;

    int u = 1;
    int v = 0;
    int d = a;
    int v1 = 0;
    int v3 = b;
    int q, t1, t3;

    // Write 2b here

    while (v3 != 0)
        {
            q = d / v3;
            t3 = d % v3;
            t1 = u - (q * v1);
            u = v1;
            d = v3;
            v1 = t1;
            v3 = t3;
        }

    v = (d - a * u) / b;
       
    if (v < 0)
        v = v + p1;

    return v;
}





int main(void)
{
    const uint32_t a1 = 0x3fffffee;
    printf("###############################\n");
    printf("Exercise 1:\n");
    printf("###############################\n");
    printf("a modulo p1 = %x\n", modp1(a1));
    printf("a modulo p2 = %x\n", modp2(a1));
    printf("a modulo b = %x\n", modb(a1));
    printf("###############################\n\n");

    //ex.2
    const uint32_t a2 = 51;
    printf("###############################\n");
    printf("Exercise 2:\n");
    printf("###############################\n");
    printf("Multipilcation inverse of 51 using FLT = %x\n", FLT(a2));
    printf("Multipilcation inverse of 51 using EEA = %x\n", EEA(a2));
    printf("###############################\n\n");

	return 0;
}


