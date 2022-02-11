
#ifndef KEYPAD_H_
#define KEYPAD_H_

    // preprocessor statements
    #define COLUMN_PIN P6OUT
    #define ROW_PIN P2IN
    #define COL_TEST BIT0
    #define ROW_TEST BIT4
    #define ROW_MASK 0xF0
    #define COL_MASK 0x07
    #define LED P3OUT
    #define KEY_BUFF_SZ 256
    #define LED_ON BIT0
    #define DELAY 10000     // 10 msec

    typedef struct KEYPAD_4x3 {
        unsigned char currentKeyCoord; // the row, column coordinate of the pressed key.
        unsigned char keyPressCnt;     // number of key presses since last reset
        unsigned char keyBuffer[KEY_BUFF_SZ]; // stores history of key presses
        unsigned char currentKey;      // The literal key from keypad.
    } KEYPAD_4x3;

    // function prototypes
    void keypadInit(KEYPAD_4x3 *keypad); // Initialises keypad structure members and
                                          // initialises the I/O ports that are connected to keypad
                                          // and LED array
    unsigned char getKeyPress(KEYPAD_4x3 * keypad); // Detects a key press from the key pad (if it occurred) and
                                                    // updates the following keypad members
                                                    // after the key is released
    int decodeKeyCoord (KEYPAD_4x3 *keypad);    // decodes coordinates into ASCII code
    void updateLedArray (KEYPAD_4x3 *keypad);   // updates the current LED array

#endif /* KEYPAD_H_ */
