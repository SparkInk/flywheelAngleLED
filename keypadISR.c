/*
 * keypadISR.c
 *
 *  Created on: Nov 18, 2021
 *      Author: iumrikhin
 */

/*
 * keypadISR.c
 *
 *  Created on: Nov 18, 2021
 *      Author: iumrikhin
 */

#include <msp430.h>
#include "keypad.h"

// global variable
unsigned char keypadState = 0;  // used to store the coordinates of the key being pressed
unsigned char debounceLH = 0;
unsigned char debounceHL = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // variables
    volatile unsigned int arrayCntr;

    KEYPAD_4x3 keypad;
    // initialise the board pins and keypad's variables
    keypadInit(&keypad);
    COLUMN_PIN = 0x7;       // initialise P6OUT to initiate scanning

    // enabling GIE
    _bis_SR_register(GIE);

    // entering the main programme's loop
    while (1) {
        if ((P2IES & ROW_MASK) && debounceLH){ // debounce when L->H
            P2IE |= ROW_MASK;    // enable interrupts
            debounceLH = 0;
        }
        if (((~P2IES) & ROW_MASK) && debounceHL) { // debounce when H->L
            P2IE |= ROW_MASK;   // enable interrupts
            debounceHL = 0;
        }
        if (!COLUMN_PIN){
            LED = 0;    // turn off LEDs
            keypad.currentKeyCoord = keypadState;
            arrayCntr = keypad.keyPressCnt;
            for (arrayCntr; arrayCntr >= 1; --arrayCntr) {     // shifting current keyCoords by one up the array
                keypad.keyBuffer[arrayCntr] = keypad.keyBuffer[arrayCntr - 1];    // take the previous keyCoord and push it down the array
            }
            keypad.keyBuffer[0] = keypad.currentKeyCoord; // record the current coordinates
            keypad.keyPressCnt++;
            decodeKeyCoord(&keypad);
            updateLedArray(&keypad);
            // turn on P6OUT to keep scanning
            COLUMN_PIN = 0x7;
        }

        continue;
    }
    return 0;
}

// interrupt routing for PORT2 to scan ROW (P2IN)
#pragma vector = PORT2_VECTOR
__interrupt void ROW_DETECT (void) {

    if ((~P2IES) & ROW_MASK) {   // check if the transition is L->H
        __delay_cycles(DELAY);
        P2IE &= ~ROW_MASK;        // turn off interrupts
        P2IES |= ROW_MASK;   // switch transition to H->L

        volatile unsigned int rowCntr;           // row counter
        volatile unsigned int colCntr;           // column counter

        COLUMN_PIN = 0;
        rowCntr = 0;     // clear row counter before scanning
        // scanning algorithm
        for (colCntr = 0; colCntr < 3; colCntr++){
           COLUMN_PIN |= (COL_TEST << colCntr);     // activate one COLUMN at a time
           for (rowCntr = 0; rowCntr < 4; rowCntr++) {
               if (ROW_PIN & (ROW_TEST << rowCntr)){
                   keypadState = (ROW_TEST << rowCntr) + (COL_TEST << colCntr);
               }
           }
            COLUMN_PIN &= ~(COL_TEST << colCntr); // clear the current bit
        }
        debounceLH = 1;
    }
    else if (P2IES & ROW_MASK){
        __delay_cycles(DELAY);
        P2IE &= ~ROW_MASK;    // turn interrupts off
        P2IES &= ~ROW_MASK;  // switch transition to L->H
        debounceHL = 1;
    }
    P2IFG = 0;   // clear pending flags
}



