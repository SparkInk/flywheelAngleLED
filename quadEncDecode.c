/*
 * quadEncDecode.c
 *
 *  Created on: 24 Nov 2021
 *      Author: emine
 */

#include <msp430.h>
#include "quadEncDecode.h"

void qEncDecode () {

    // previous state into the last position
    qEdecoder.channelState[3] = qEdecoder.channelState[1];
    qEdecoder.channelState[2] = qEdecoder.channelState[0];

    // THE MAIN ALGORITHM. The Beginning
    if (P1IFG & CHA) {   // channel A is being triggered; L->H

        if (~CHAB_INTR_ES & CHA) { // channel A is L->H; first income
            qEdecoder.channelState[1] ^= CHA;
            CHAB_INTR_ES |= CHA;   // set to H->L detection; channel A
        }
        else if (CHAB_INTR_ES & CHA) {    // channel A is being triggered; H->L; second income
            qEdecoder.channelState[1] ^= CHA;
            CHAB_INTR_ES &= ~CHA;      // set to L->H detection; channel A
        }
    }
    else if (P1IFG & CHB) { // channel B is being triggered
        if (~CHAB_INTR_ES & CHB)  {  // channel B is L->H; first income
            qEdecoder.channelState[0] ^= CHB;
            CHAB_INTR_ES |= CHB;   // set to H->L detection; channel B
        }
        else if (CHAB_INTR_ES & CHB) { // channel B is H->L; second income
            qEdecoder.channelState[0] ^= CHB;
            CHAB_INTR_ES &= ~CHB;   // set to L->H detection; channel B
        }
    }
    // THE MAIN ALGORITHM. The Final
    if (direction(&qEdecoder)) {     // positive direction
        qEdecoder.posCount++;
    }
    else {  // negative direction
        qEdecoder.posCount--;
    }
    P1IFG = 0;
  }
  
unsigned int direction(QUAD_ENC_DECODER *qEdecoder) {
    unsigned char result;
    // first term is CHA_CURR; second term is CHB_PREV
    // if CHA_CURR xor CHB_PREV = 1 (positive); 0 - negative
    result = qEdecoder->channelState[1] ^ (qEdecoder->channelState[2] << 1);

    return result;
}
/*******
qEncDecode
0 0 0 0  <- initial values
0 0 1 0
0 0 1 1
1 0 0 1
1 1 0 0  <- after 4 counts
***************/
