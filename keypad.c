#include <msp430.h>
#include "keypad.h"


void keypadInit(KEYPAD_4x3 *keypad) {

    // initialise P6OUT (aka Column)
    P6DIR |= 0x7; // initialise P6OUT<0>, P6OUT<1>, P6OUT<2>
    COLUMN_PIN = 0x7;       // initialise P6OUT to initiate scanning

    // initialise P2IN (aka Row)
    P2DIR &= ~0xF0; // P2OUT is an input now
    P2REN |= 0xF0; // enables internal resistors
    P2OUT &= ~0xF0; // pull-down resistors are on

    // initialise PORT2 interrupts
    P2IE |= ROW_MASK;   // enable interrupt on P2<7:4>
    P2IES &= ~ROW_MASK; // detects transition from L->H
    P2IFG = 0;

    // initialise LED
    P3DIR |= 0xFF;
    LED = 0;    // clear LED from previous states; P3OUT<7:0> are LOW

    // setting up the KEYPAD_4x3 variables
    keypad->currentKey = 0;
    keypad->currentKeyCoord = 0;
    keypad->keyBuffer[KEY_BUFF_SZ] = 0;
    keypad->keyPressCnt = 0;
}

unsigned char getKeyPress(KEYPAD_4x3 * keypad) {

   signed char returnValue = -1;   // a value to return: 0 if TRUE; -1 if FALSE
   unsigned int rowCntr;           // row counter
   unsigned int colCntr;           // column counter
   unsigned int arrayCntr;

   COLUMN_PIN = 0;
   rowCntr = 0;     // clear row counter for before scanning
   LED = 0;         // set LED to LOW
   // scanning algorithm
   for (colCntr = 0; colCntr < 3; colCntr++){
       COLUMN_PIN |= (COL_TEST << colCntr);     // activate one COLUMN at a time
       for (rowCntr = 0; rowCntr < 4; rowCntr++) {
           if (ROW_PIN & (ROW_TEST << rowCntr)){
               keypad->currentKeyCoord = (ROW_TEST << rowCntr) + (COL_TEST << colCntr);  // store the current key's coordinate
               arrayCntr = keypad->keyPressCnt;
               for (arrayCntr; arrayCntr >= 1; --arrayCntr) {     // shifting current keyCoords by one up the array
                    keypad->keyBuffer[arrayCntr] = keypad->keyBuffer[arrayCntr - 1];    // take the previous keyCoord and push it down the array
               }
               keypad->keyBuffer[0] = keypad->currentKeyCoord; // record the current coordinates
               keypad->keyPressCnt++;  // increase the key's counter
               returnValue = 0;
               // if a button is pressed, keeps looping;  
               while (ROW_PIN & (ROW_TEST << rowCntr)) {
                   continue;
               }
           }
       }
       COLUMN_PIN &= ~(COL_TEST << colCntr); // clear the current bit
   }

   return returnValue;
}

void updateLedArray (KEYPAD_4x3 *keypad) {
    unsigned int shiftBit;

    for (shiftBit = 0; shiftBit < 8; shiftBit++) {
        LED |= (keypad->currentKeyCoord & (LED_ON << shiftBit));
    }
}

int decodeKeyCoord (KEYPAD_4x3 *keypad) {
    signed int returnValue = -1;

    switch (keypad->currentKeyCoord) {
    case 0x11:
        keypad->currentKey = 0x1;
        returnValue = 0;
        break;
    case 0x12:
        keypad->currentKey = 0x2;
        returnValue = 0;
        break;
    case 0x14:
        keypad->currentKey = 0x3;
        returnValue = 0;
        break;
    case 0x21:
        keypad->currentKey = 0x4;
        returnValue = 0;
        break;
    case 0x22:
        keypad->currentKey = 0x5;
        returnValue = 0;
        break;
    case 0x24:
        keypad->currentKey = 0x6;
        returnValue = 0;
        break;
    case 0x41:
        keypad->currentKey = 0x7;
        returnValue = 0;
        break;
    case 0x42:
        keypad->currentKey = 0x8;
        returnValue = 0;
        break;
    case 0x44:
        keypad->currentKey = 0x9;
        returnValue = 0;
        break;
    case 0x81:
        keypad->currentKey = '*';
        returnValue = 0;
        break;
    case 0x82:
        keypad->currentKey = 0x0;
        returnValue = 0;
        break;
    case 0x84:
        keypad->currentKey = '#';
        returnValue = 0;
        break;
    }

    return returnValue;
}
