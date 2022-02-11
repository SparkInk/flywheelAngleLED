/*
 * sevenSegDisplay.h
 *
 *  Created on: 24 Nov 2021
 *      Author: emine
 */

#ifndef SEVENSEGDISPLAY_H_
#define SEVENSEGDISPLAY_H_

    // preprocessor statements
    #define FRQ_RFRSH 7000 // refresh frequency of single display
    #define FRQ_MCLK 20000 // cycles for the main timer
    #define SEG P3OUT // Segment Port for 7 segment interface
    #define DIGIT P6OUT // you choose an output port for the digit display control.

    #define DISP1 BIT3 // display 1 on, display 2 off
    #define DISP2 BIT4
    #define DISP3 BIT5
    // 0000 0000 -> (dp)gfe dcba
    #define NUM_0 0x3F
    #define NUM_1 0x06
    #define NUM_2 0x5B
    #define NUM_3 0x4F
    #define NUM_4 0x66
    #define NUM_5 0x6D
    #define NUM_6 0x7D
    #define NUM_7 0x07
    #define NUM_8 0x7F
    #define NUM_9 0x6F
    #define DP BIT7
    // sysState bits
    #define PWR BIT0
    #define CLR BIT1
    #define REFRESH BIT2
    #define FLASH BIT3
    #define KEY_PRESS BIT4
    #define SIGN_BIT 0x1000
    #define SEC 1000000 // 1s delay
    #define OFF 0xFF    // an off state; for active low; for PORT3 and PORT6 to be used
    #define OFF_EX_DP 0x7F

    typedef struct SEVEN_SEG_DISP {
        char decDigit; // base 10 decimal digit -9 to + 9
        unsigned char binSegCode; // 7 segment code <a:g> plus
        unsigned char ctrl; // single bit indicating on/off state of display
    } SEVEN_SEG_DISP;

    //function prototypes
    void sevenSegDispInit (SEVEN_SEG_DISP * display);
    void decto7Seg (SEVEN_SEG_DISP * display);
    void dispRefresh(SEVEN_SEG_DISP * display, unsigned char dispIndex);
    void timerA0Init(void);
    void dispPwr(SEVEN_SEG_DISP * display, unsigned char digitPwr);
    void testFunction();
    //global variables

    extern char sysState; // system state vector which is updated in ISRs and defined as:
                          // sysState<0>: 1 PWR is ACTIVE, 0 PWR is INACTIVE.
                          //sysState<1>: 1 CLR is ACTIVE, 0 CLR is INACTIVE.
                          //sysState<2>: 1 REFRESH is ACTIVE, REFRESH is INACTIVE.
                          //sysState<3>: 1 FLASH is ACTIVE, 0 FLASH is INACTIVE.
                          //sysState<4>: 1 KEY_PRESS is ACTIVE, 0 KEY_PRESS is INACTIVE

#endif /* SEVENSEGDISPLAY_H_ */
