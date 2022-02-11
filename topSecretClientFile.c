/*
 * topSecretClientFile.c
 *
 *  Created on: 27 Nov 2021
 *      Author: emine
 */

#include <msp430.h>
#include "quadEncDecode.h"
#include "sevenSegDisplay.h"
#include "keypad.h"

/******************* FUNCTION DEFINITION *******************/
signed int computeAngle (void);
signed int genBCD3 (int decNum3);
void swReset (void);
void dispPwr(SEVEN_SEG_DISP * display, unsigned char digitPwr);

/******************* GLOBAL VARIABLES *******************/
QUAD_ENC_DECODER qEdecoder;
SEVEN_SEG_DISP display[3];
char sysState = 0;
unsigned char dispIndex = 0;

unsigned char  stateSystem = 0; // need it for controlling the algorithm
unsigned char debounceLH = 0;
unsigned char debounceHL = 0;

int main()
{
    WDTCTL = WDTPW | WDTHOLD; 

    volatile signed int angle = 0;

    //volatile unsigned char encDir = 1;
    volatile unsigned char angleToDispCount = 0;
    volatile signed int angleToDisp = 0;
    volatile unsigned char digCount;
    volatile unsigned int arrayCntr;
    volatile unsigned char keypadState = 0;
    KEYPAD_4x3 keypad;

    // initialise the board pins and keypad's variables
    keypadInit(&keypad);
    COLUMN_PIN = 0x7;       // initialise P6OUT to initiate scanning

    sevenSegDispInit(&display); // initialise the display
    sysState &= ~REFRESH;       // reset the REFRESH bit in sysState

    qEdecoder.posCount = 0; // nullify posCount

    // setting up PORT1 as the input
     P1DIR &= ~CHA & ~CHB;   // input P1.3 & P1.2;
     P1REN |= CHA | CHB;   // enable internal resistors
    //P1DIR |= CHA | CHB; // output on P1.3 & P1.2 (for SW qEncoder)
    P1OUT = 0;   // resistors are pull-down

    // setting up the interrupts for PORT1
    P1IE |= CHA | CHB;
    CHAB_INTR_ES &= ~CHA & ~CHB;    // L->H
    // setting up the P1.3 & P1.2 as interrupts
    
    P1IFG = 0;  // clear interrupt's flags
    // enable Timer A0
    timerA0Init();
    // enable GIE
    _bis_SR_register(GIE);

    while(1) {
        if (sysState & PWR) { // request to turn the power ON
            dispPwr(&display, dispIndex);
        }
        else {

        }
        if (/*(P2IES & ROW_MASK) &&*/ debounceLH){ // debounce when L->H
            __delay_cycles(DELAY);
            //P2IE |= ROW_MASK;    // enable interrupts
            debounceLH = 0;
            stateSystem = 1;
            }
            if (/*((~P2IES) & ROW_MASK) &&*/ debounceHL) { // debounce when H->L
                __delay_cycles(DELAY);
                P2IE |= ROW_MASK;   // enable interrupts
                debounceHL = 0;
            }
            if (stateSystem){
                volatile unsigned int rowCntr;           // row counter
                volatile unsigned int colCntr;           // column counter

                COLUMN_PIN = 0;
                rowCntr = 0;     // clear row counter before scanning
                //LED = 0;    // turn off LEDs
                // scanning algorithm
                for (colCntr = 0; colCntr < 3; colCntr++){
                   COLUMN_PIN |= (COL_TEST << colCntr);     // activate one COLUMN at a time
                   for (rowCntr = 0; rowCntr < 4; rowCntr++) {
                       if (ROW_PIN & (ROW_TEST << rowCntr)){
                           keypad.currentKeyCoord = (ROW_TEST << rowCntr) + (COL_TEST << colCntr);
                            arrayCntr = keypad.keyPressCnt;
                            for (arrayCntr; arrayCntr >= 1; --arrayCntr) {     // shifting current keyCoords by one up the array
                                keypad.keyBuffer[arrayCntr] = keypad.keyBuffer[arrayCntr - 1];    // take the previous keyCoord and push it down the array
                            }
                            keypad.keyBuffer[0] = keypad.currentKeyCoord; // record the current coordinates
                            keypad.keyPressCnt++;
                            decodeKeyCoord(&keypad);
                       }
                   }
                    COLUMN_PIN &= ~(COL_TEST << colCntr); // clear the current bit
                }
                // turn on P6OUT to keep scanning
                COLUMN_PIN = 0x7;
                stateSystem = 0;
                P2IE |= ROW_MASK;   // enable interrupts
            }
        //sysState |= REFRESH;
        if ((sysState & REFRESH) /*&& (sysState & PWR)*/){ // the refresh bit is set
            // clear all the previous digits
            for(digCount = 0; digCount < 3; digCount++) {
                display[digCount].decDigit = 0;
            }
            // start convertion
            angle = computeAngle();
            if (angle > 360 || angle < -360) {
                qEdecoder.posCount = 0; // +- 360 exceeded; reset
            }
            // convert into separate digits
            angleToDisp = genBCD3(angle);
            // write these angles digits into dislay.decDigit
            for (digCount = 0; digCount < 3; digCount++) {
                display[digCount].decDigit |= (angleToDisp >> 4*digCount) & 0xF;
                if (angle < 0 && digCount == 2) {
                    display[digCount].decDigit |= BIT4;   //
                }
            }
            // diplay numbers
            sysState &= ~REFRESH;
        }
        // keep showing the angle
        if ((sysState & FLASH) /*&& (sysState & PWR)*/) {
            if (dispIndex < 3) {
                if (angle > 0) {
                    dispRefresh(display, dispIndex);
                    //SEG = OFF;  // clear the segment
                    SEG |= DP;  // clear the segment
                }
                else { // angle is negative
                    SEG &= ~DP;
                    dispRefresh(display, dispIndex);
                }
                sysState &= ~FLASH;
            }
            else {
                dispIndex = 0;
            }
        }
    }
    
    return 0;
}

#pragma vector = PORT1_VECTOR;
__interrupt void QUAD_ENC(void) { // interrupt for the qEncDecoder function
    //P1IE &= ~CHA & ~CHB; // turn off interrupts on channel A and B
   qEncDecode();
   if (!(sysState & REFRESH)) {
       sysState |= REFRESH;
   }
}
#pragma vector = TIMER0_A1_VECTOR
__interrupt void timer0A1ISR (void) {
    static volatile unsigned char counter;
    if (!(sysState & FLASH)) { // to allow the display for showing us what it's got ;)
        sysState |= FLASH;
        DIGIT |= display[dispIndex].ctrl; // turn off the segment 1
        SEG = OFF_EX_DP;  // clear the segment
        dispIndex++;
    }
    TA0CTL &= ~TAIFG;
}

// interrupt routing for PORT2 to scan ROW (P2IN)
#pragma vector = PORT2_VECTOR
__interrupt void ROW_DETECT (void) {
    if (!(sysState & PWR)) { // for keypad *; power is off; need to turn on
        sysState |= PWR;
    }
    else{
        sysState &= ~PWR;   // turn off the power;
    }
    if ((~P2IES) & ROW_MASK && !stateSystem) {   // check if the transition is L->H
        P2IE &= ~ROW_MASK;        // turn off interrupts
        P2IES |= ROW_MASK;   // switch transition to H->L
        debounceLH = 1;
    }
    else if (P2IES & ROW_MASK && !stateSystem){
        P2IE &= ~ROW_MASK;    // turn interrupts off
        P2IES &= ~ROW_MASK;  // switch transition to L->H
        debounceHL = 1;
    }
    P2IFG = 0;   // clear pending flags
}

/* ***********************************************************************************
* converts 3 digit signed decimal number +/-xyz -> 3 digit 13 bit BCD value 0xmpw.
* i.e 456d -> 0x456, -999d -> 0x1999, 0 -> 0x0
* arguments:
* decNum3 � 3 digit integer in the range +/- 999
* return: integer BCD value positive 0xmpw, negative 0x1mpw: m = x, p = y, w = z.
* Author:
* Date:
************************************************************************************/
signed int genBCD3 (signed int decNum3){
    volatile unsigned int i;
    volatile unsigned int bcdValue = 0;
    volatile unsigned char digitNum = 0;

    if (decNum3 > 0) {  // for positive numbers
        for (i = 0; i < 3; i++) {
            digitNum = decNum3 % 10;
            bcdValue |= digitNum << 4*i;
            decNum3 /= 10;
        }
    }
    else if (decNum3 < 0) { // for negative numbers
        decNum3 = ~decNum3 + 1;
        for (i = 0; i < 3; i++) {
            digitNum = decNum3 % 10;
            bcdValue |= digitNum << 4*i;
            decNum3 /= 10;
        }
    }
    return bcdValue;
}


/* ***********************************************************************************
* Uses posCount, encoder CPR and gearbox N to compute flywheel angle in degrees.
* arguments:
* void
* return: angle in degrees. range +/- 360 degrees
* Author:
* Date:
************************************************************************************/
signed int computeAngle (void) {
    
    return (qEdecoder.posCount / QUAD_PULSES);
}

/* ***********************************************************************************
* dispPwr turns OFF any of the 3 digits or turns ON only one digit, thereby preventing more than one
display from ever being active. It updates display[i].ctrl with digit activation state. There is no need to
store the previous ctrl activation state within this function before modifying it.
* display – address of the first element of the display array
* digitPwr – 3 bit ACTIVE HIGH code which represents the ON/OFF state of the display digit.
BIT0  display 1, BIT1display 2, BIT2display 3
* return: void
* Author:
* Date:
************************************************************************************/
void dispPwr(SEVEN_SEG_DISP * display, unsigned char digitPwr) {
    if (sysState & PWR) {   // power is off; needs to be turned on
        DIGIT &= ~display[digitPwr].ctrl;
    }
    if (!(sysState & PWR)) {  // power is on ; needs to be turned off
        DIGIT |= display[digitPwr].ctrl;
    }

}

/* ***********************************************************************************
* Software reset. Clears posCount, sets refresh frequency to 50Hz, brightness to 100%.
* arguments:
* void
* return: void
* Author:
* Date:
************************************************************************************/
void swReset (void) {
    qEdecoder.posCount = 0;

    // frequency to 0
    // brightness to 0

}
