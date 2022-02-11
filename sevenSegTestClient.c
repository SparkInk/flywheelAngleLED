/*
 * sevenSegTestClient.c
 *
 * Author: Iakov Umrikhin
 * Verification: ��, ��� � ������
 *
 */
#include <msp430.h>
#include "sevenSegDisplay.h"

#define OFF 0xff
#define DIGIT P6OUT
#define DISP1 BIT3
#define DISP2 BIT4
#define DISP3 BIT5
#define SEG P3OUT

char sysState;

int main(void)
 {

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    unsigned char dispIndex = 0;

    //testFunction();

    SEVEN_SEG_DISP display[3];

    sevenSegDispInit(&display); // initialise the display
    sysState &= ~REFRESH;

    display[0].binSegCode = NUM_1;
    display[1].binSegCode = NUM_3;
    display[2].binSegCode = NUM_5;

    while(1) {
        sysState |= REFRESH;
        if (sysState & REFRESH){ // the refresh bit is set
            for (dispIndex; dispIndex < 3; dispIndex++) {
                dispRefresh(display, dispIndex);
            }
            SEG |= OFF;
            dispIndex = 0;
            sysState &= ~REFRESH;
        }

    }
    return 0;
}


