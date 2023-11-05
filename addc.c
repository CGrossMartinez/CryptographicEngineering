//Carlos Gross-Martinez
//Z#: Z-23226341
//Assignment: 2a
//Cryptographic Engineering
//Reza Azarderakhsh


/*
Solution:    1a, Carry Propagation

Description: the function will carry addition computation of 32 bit numbers where two arguments of the function
             will be outpus: sum, carryOut. Remaining arguments will be inputs: carryIn, addend1, addend2.
             
Constraints: sum must similarly be 32-bit datatype with the lowest 8-bit filled and the remaining 24-bit 0s at 
             the end of the function. The carryIn and carryOut are 1 bit only. No loops, no conditional statements,
             and only use specified operators.
*/




void addc(carryIn, addend1, addend2, sum, carryOut)
{
    sum = addend1 + addend2 + carryIn;
    carryOut = sum >> 8;
    sum = sum & 0xFF;
}

void add64(r, a, b)
{
    addc(carry, a, b, r, carry);
    printf("%d", r);
}


/*
Solution:    2a, Borrow Propagation

Description: the function will carry substration computation of 32 bit numbers where two arguments of the function
             will be outpus: sum, carryOut. Remaining arguments will be inputs: carryIn, addend1, addend2.

Constraints: sum must similarly be 32-bit datatype with the lowest 8-bit filled and the remaining 24-bit 0s at
             the end of the function. The carryIn and carryOut are 1 bit only. No loops, no conditional statements,
             and only use specified operators.
*/


void subc(borrowIn, minuend, subtrahend, sum, borrowOut)
{
    sum = (minuend + (borrowIn * 0x100)) - subtrahend;
    borrowOut = sum >> 8;
    sum = sum & 0xFF;
}

void sub64(r, a, b)
{
    subc(borrow, a, b, r, borrow);
    printf("%d", r);
}

 
void schoolbook_mul64(r, a, b)
{
    r0 = (b0 * a0) + (b0 * a1) + (b0 * a2) + (b0 * a3)
    r1 = (b1 * a0) + (b1 * a1) + (b1 * a2) + (b1 * a3)
    r2 = (b2 * a0) + (b2 * a1) + (b2 * a2) + (b2 * a3)    
}

