/*
 * quadEncDecode.h
 *
 *  Author: Iakov Umrikhin
 *
 */

#ifndef QUADENCDECODE_H_
#define QUADENCDECODE_H_
    // preprocessor statements
    #define CHAB_PORT P1IN // use P1.3 and P1.2
    #define CHAB_INTR_ES P1IES  // use IES on PORT1<3:2>
    #define CHA BIT3 // CHA bit position
    #define CHB BIT2 // CHB bit position
    #define QUAD_PULSES 13 // 13 pulses = 1 degree

    typedef struct QUAD_ENC_DECODER {
        unsigned char channelState[4]; // current and previous channel A,B states
                                      // channelState[0] > CHB_PREV, channelState[1] > CHA_PREV
                                      // channelState[2] > CHB_CURR, channelState[3] > CHA_CURR
        long posCount; // 16 bit signed position counter updated every quadrature event
    } QUAD_ENC_DECODER;

    // functions prototypes
    void qEncDecode ();
    unsigned int direction(QUAD_ENC_DECODER *qEdecoder);
    // global variables
    extern QUAD_ENC_DECODER qEdecoder;
    extern char sysState; // system state vector which is updated in ISRs and defined as:
            // sysState<0>: 1 PWR is ACTIVE, 0 PWR is INACTIVE.
            //sysState<1>: 1 CLR is ACTIVE, 0 CLR is INACTIVE.
            //sysState<2>: 1 REFRESH is ACTIVE, REFRESH is INACTIVE.
            //sysState<3>: 1 FLASH is ACTIVE, 0 FLASH is INACTIVE.
            //sysState<4>: 1 KEY_PRESS is ACTIVE, 0 KEY_PRESS is INACTIVE


#endif /* QUADENCDECODE_H_ */
