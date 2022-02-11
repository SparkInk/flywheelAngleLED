/*
 * sevenSegDisplay.c
 *
 *  Created on: 24 Nov 2021
 *      Author: emine
 */

#include "sevenSegDisplay.h"
#include "quadEncDecode.h"
#include <msp430.h>
/************************************************************************************
* Initialise sevenSegDisp variable by setting display-> decDigit to 0, display-> binSegCode to 0xFF.
*           SEG and DIGIT are initialised to outputs. display->ctrl is de-asserted (off)
*           and DIGIT port is updated with deactivated state.
*
*
* arguments:
*           display � address of display to be initialised
* return: void
*
* Author: Iakov Umrikhin
* Date: TBD
************************************************************************************/
void sevenSegDispInit (SEVEN_SEG_DISP * display) {

    /// enable PORT3 aka SEG
    P3DIR |= 0xff;  // output for P3<7:0>
    SEG = OFF;    // P3<7:0> are active LOW; set to 0xff
    // enable PORT6 aka DIGIT
    P6DIR |= 0x3f; // output for P6<5:0>
    DIGIT = OFF;    // set to 0xff

    display[0].decDigit = 0;
    display[1].decDigit = 0;
    display[2].decDigit = 0;
    display->binSegCode = 0;
    display[0].ctrl = DISP1;
    display[1].ctrl = DISP2;
    display[2].ctrl = DISP3;

}
/* ***********************************************************************************
* Convert display->decDigit to display->binSegCode
          display->ctrl is not affected

* arguments:
*           display � address of display to be updated
* return: void
*
* Author: Iakov Umrikhin
* Date: TBD
************************************************************************************/
void decto7Seg (SEVEN_SEG_DISP * display) {

    volatile unsigned int dispCount = 0;
    // each number save for 0 should have a negative equivalent
    for (dispCount; dispCount < 3; dispCount++) {
        switch(display[dispCount].decDigit) {
            case 0:
                display[dispCount].binSegCode = NUM_0; // needs to be inverted to be properly displayed on LED
                break;
            case 1:
                display[dispCount].binSegCode = NUM_1;
                break;
            case 2:
                display[dispCount].binSegCode = NUM_2;
                break;
            case 3:
                display[dispCount].binSegCode = NUM_3;
                break;
            case 4:
                display[dispCount].binSegCode = NUM_4;
                break;
            case 5:
                display[dispCount].binSegCode = NUM_5;
                break;
            case 6:
                display[dispCount].binSegCode = NUM_6;
                break;
            case 7:
                display[dispCount].binSegCode = NUM_7;
                break;
            case 8:
                display[dispCount].binSegCode = NUM_8;
                break;
            case 9:
                display[dispCount].binSegCode = NUM_9;
                break;
        }
    }

}

/* ***********************************************************************************
* All display[k].ctrl bits are deactivated and written to DIGIT port.
*           Writes display[dispIndex].binSegCode
*           to SEG port. Activates display[dispIndex].ctrl and writes it to DIGIT port.
*           This ensures only one display is on at any one time. Assumes the array size is 3.
*
*           The refresh rate is interrupt driven and is controlled by updating the REFRESH bit
*           within the sysState global variable
*
* Arguments:
*           display � address of the first element of the display array
*           dispIndex � index 0 to 2 refers to display 1 to 3.
* return: void
* Author: Iakov Umrikhin
* Date: TBD
************************************************************************************/
void dispRefresh(SEVEN_SEG_DISP * display, unsigned char dispIndex) {
    if (display[2].decDigit & BIT4) { // negative number
        display[2].decDigit &= ~BIT4;
        // negative number to be displayed
        decto7Seg(display);
        SEG &= ~display[dispIndex].binSegCode;
        DIGIT &= ~display[dispIndex].ctrl;     // turn on the segment 1

    }
    else {
        decto7Seg(display);
        SEG &= ~display[dispIndex].binSegCode;
        DIGIT &= ~display[dispIndex].ctrl;     // turn on the segment 1

    }
    
 }

/***************************************************
 * timerAInit()
 *
 *
 *
 *
 *
 **************************************************/
void timerA0Init(void) {
    TA0CTL |= TASSEL_2 | ID_0 | MC_1 | TAIE; // TASSEL_2 <- source select (SMCLK); ID_0 <- devider(1); MC_1 <- mode (up mode);
    TA0CCR0 = FRQ_RFRSH - 1;    // for 50 Hz
    TA0CTL &= ~TAIFG; // clear interrupt flags; if the first one does not work

}
void testFunction() {
    // enable PORT6
    P6DIR |= 0x3f; // output for P6<5:0>
    DIGIT = OFF;
    // enable PORT3
    P3DIR |= 0xff;  // output for P3<7:0>
    SEG = OFF;    // P3<7:0> are active LOW;

    // send 1
    DIGIT &= ~DISP1; // P6.3 is active HIGH
    SEG &= ~NUM_1;
    __delay_cycles(SEC);
    DIGIT |= DISP1;
    SEG = OFF;

    // send 3
    DIGIT &= ~DISP2;
    SEG &= ~NUM_3;
    __delay_cycles(SEC);
    DIGIT |= DISP2;
    SEG = OFF;

    // send 5
    DIGIT &= ~DISP3;
    SEG &= ~NUM_5;
    __delay_cycles(SEC);
    DIGIT |= DISP3;
    SEG = OFF;
}

