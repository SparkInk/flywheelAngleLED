
/*
 * qEncDecoderTestClient.c
 *
 *  Created on: Nov 25, 2021
 *      Author: iumrikhin
 */

#include <msp430.h>
#include "quadEncDecode.h"

QUAD_ENC_DECODER qEdecoder = {0};

int main(void)
{
    unsigned char swEnc[4] = {0b0000, 0b1000, 0b1100, 0b0100}; // software emulation of quadEncoder
    signed char swEncCount = 0;   // counter for swEnc
    unsigned char whileCount = 100;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    qEdecoder.posCount = 0; // nullify posCount

    // setting up PORT1 as the input
   // P1DIR &= ~CHA & ~CHB;   // input P1.3 & P1.2;
    P1DIR |= CHA | CHB; // output on P1.3 & P1.2 
   // P1REN |= CHA | CHB;   // enable internal resistors
    P1OUT = 0;   // resistors are pull-down

    // setting up the interrupts for PORT1
    P1IE |= CHA | CHB;
    CHAB_INTR_ES &= ~CHA & ~CHB;    // L->H
    // setting up the P1.3 & P1.2 as interrupts
    
    P1IFG = 0;

    // enable GIE
    _bis_SR_register(GIE);
    // software quadrature encoder 
    while(whileCount > 0) {
        for (swEncCount; swEncCount < 4; swEncCount++) {
                P1OUT = swEnc[swEncCount];
        }
        P1IFG = 0;
        swEncCount = 0;
        whileCount--;
        continue;   // do nothing
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void QUAD_ENC(void) { // interrupt for the qEncDecoder function

   qEncDecode();
}
