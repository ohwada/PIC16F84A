/**
 * PIC16F84A
 * Slot Game
 * 2015-05-10 K.OHWADA
 */

/* --- pin assign --- */
// RA0  7SEG F
// RA1  Button (input)
// RA2  LED
// RA3  Buzzer
// RA4  7SEG Anode 3
// RB0  7SEG B	
// RB1  7SEG Anode 2
// RB2  7SEG A
// RB3  7SEG Anode 1
// RB4  7SEG G
// RB5  7SEG E
// RB6  7SEG D
// RB7  7SEG C

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <pic16f84a.h>  /* PIC16F84A */ 
#include <stdbool.h>    /* bool */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
#define _XTAL_FREQ 4000000     // 4MHz

/**
 * OPTION_REG
 *   7: RBPU: PORTB Pull-up Enable bit: enabled
 *   6: INTEDG: Interrupt Edge Select bit: falling edge of RB0/INT pin
 *   5: T0CS: TMR0 Clock Source Select bit: Internal instruction cycle clock
 *   4: T0SE: TMR0 Source Edge Select bit: low-to-high transition on RA4/T0CKI pin
 *   3: PSA: Prescaler Assignment bit: Timer0
 *   2-0: PS2-0: Prescaler Rate Select bits: 
 *      000 1/2,   001 1/4,    010 1/8,     011 1/16
 *      100 1/32, 101  1/64, 110 1/128, 111  1/256
 */
#define OPTION_REG_VALUE  0x02

// _XTAL_FREQ / ( 4 * 8 ) = 125 KHz

// status
#define ST_INIT  0
#define ST_WAIT  1
#define ST_PLAY_1  2
#define ST_BUTTON_1  3
#define ST_PLAY_2  4
#define ST_BUTTON_2  5
#define ST_PLAY_3  6
#define ST_BUTTON_3  7
#define ST_777  8
#define ST_END  9

/**
 * PIC16F84A configuraton
 * Watchdog Timer: WDT disabled
 * Power-up Timer Enable bit: Power-up Timer is disabled
 * Code Protection bit: Code protection disabled
 * Oscillator Selection bits: HS oscillator
 */
#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, FOSC = HS 

/* Function Declaration */
void endGame(void);
void tone( char n, char t1, char t2 );
void noTone( char t );
void timer(void);
void t333ms(void);
void t1sec(void);
void upDigit( bool flag );

/* Constant */
// 7 seg
#define ROW_NUM  3
#define SEG_NUM  7

// 7 Seg Pattern
const char PAT[] = {	
    1,1,1,1, 1,1,0,	// 0
    0,1,1,0, 0,0,0,	// 1
    1,1,0,1, 1,0,1,	// 2
    1,1,1,1, 0,0,1,	// 3
    0,1,1,0, 0,1,1,	// 4	
    1,0,1,1, 0,1,1,	// 5	
    1,0,1,1, 1,1,1,	// 6
    1,1,1,0, 0,1,0,	// 7				
    1,1,1,1, 1,1,1,	// 8		
    1,1,1,1, 0,1,1,	// 9
    0,0,1,1, 1,1,0	// 7 reverse			
};

// index of tone
#define DO4 0
#define RE 1
#define MI 2
#define FA 3
#define SO 4
#define RA 5
#define SI 6
#define DO5 7

// tone timer 
const unsigned char TMR0_VALUES[] = {
17,  // 262Hz
43,   // 293
67,   // 330
77,   // 349
97,   // 392
114,  // 440
129,  // 494
137, // 523	
};

// 333ms timer
const unsigned char T333MS_VALUES[] = {
79,  // 262Hz
88,   // 293
100,   // 330
106,   // 349
119,   // 392
133,  // 440
150,  // 494
158 // 523
};

/* Gloval Variable */
// status
unsigned char status = 0;   // status
unsigned char mode_777 = 0;  // mode 777
bool is_debug = 0;   // debug

// timer
unsigned char tmr0_value = 0;    // TMR0 value
unsigned int t333ms_max = 0;  // 1/3 sec
unsigned int t333ms_cnt = 0;  // 1/3 sec counter
unsigned char t1sec_cnt = 0; // 1 sec counter
bool is_1sec_status = 0; // 1 sec status 

// tone
bool is_buzzer_on = 0; // buzzer value
bool is_tone_enable = 0; // tone enable

// 7 seg
unsigned char row_cnt = 0;  // row 0-2
bool is_7seg_on = 0;  // display 7seg

// digit	
unsigned char digit[ ROW_NUM ]; // value 0-9
bool is_digit[ ROW_NUM ];  // display digit

// button
bool is_button_prev = 0;   // prev status

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void) {
    /* Initialize I/O and Peripherals for application */
    TRISA = 0x02;   // RA1 input, others outpot
    TRISB = 0x00;   // PortB all output
    PORTA = 0x00;   // PortA all low
    PORTB = 0x00;   // PortB all low

    // OPTION_REG
    OPTION_REG = OPTION_REG_VALUE ;
    // Timer 0
    tmr0_value = 114;   // RA
    t333ms_max = 133;
    TMR0 = tmr0_value ;
    // TMR0 Overflow Interrupt Enable bit = Enables
    INTCONbits.TMR0IE =1; 
    // Global Interrupt Enable bit = Enables
    INTCONbits.GIE = 1;

    // start game
    is_debug = !PORTAbits.RA1;    // button
    is_digit[0] = false;
    is_digit[1] = false;
    is_digit[2] = false;
    digit[0] = 2;
    digit[1] = 1;
    digit[2] = 0;
    tone( DO5, 50, 50 );
    status = ST_WAIT ;

    // loop if push button
    while( status == ST_WAIT ) {
        upDigit(true);
        __delay_ms(500);
    }

    // endless loop
    while(1) {
        if ( status == ST_BUTTON_1 ) {
            noTone( 10 );
            status = ST_PLAY_2 ;
        } else if ( status == ST_BUTTON_2 ) {
            noTone( 10 );
            status = ST_PLAY_3 ;
        } else if ( status == ST_BUTTON_3 ) {
            endGame();
        } else if (( status == ST_PLAY_1 )||( status == ST_PLAY_2 )||( status == ST_PLAY_3 )) {
            tone( FA, 10, 3 );
            tone( SO, 15, 3 );
            tone( FA, 10, 3 );
            tone( RA, 15, 3 );
        }
    }
}

/**
 * end game
 */
void endGame(void) {
    int i;
    noTone( 20 );
    if (( digit[0] == 7 )&&( digit[1] == 7 )&&( digit[2] == 7 )) {
        // 777
        status = ST_777;
        mode_777 = 0;
        for ( i=0; i<3; i++ ) {
            tone( SO, 20, 20 );
            tone( SO, 20, 20 );
            tone( SO, 10, 10 );
            tone( MI, 10, 10 );
            tone( DO5, 40, 100 );
        }
    } else {
        // NG
        tone( SO, 30, 30 );
        tone( MI, 30, 30 );
        tone( DO4, 60, 30 );
        status = ST_END ;
    }
}

/**
 * tone
 */
void tone( char n, char t1, char t2 ) {
    tmr0_value = TMR0_VALUES[n];
    t333ms_max = T333MS_VALUES[n];
    TMR0 = tmr0_value;
    is_tone_enable = true;
    for( ; t1>0; t1-- ) {
        __delay_ms(10);
    }
    noTone( t2 );
}

/**
 * no tone
 */
void noTone( char t ) {
    is_tone_enable = false;
    for( ; t>0; t-- ) {
        __delay_ms(10);
    }
}

/**
 * interrupt
 */
static void interrupt isr(void) {
/* 880 Hz @ RA */
    // TMR0 Overflow Interrupt Flag bit : overflowed ?
    if (INTCONbits.TMR0IF == 1) {
        timer();
        // 333ms timer
        t333ms_cnt ++;
        if ( t333ms_cnt >= t333ms_max ) {
            t333ms_cnt = 0; // reset
            t333ms();
        }
        // timer
        TMR0 = tmr0_value;
        INTCONbits.TMR0IF = 0; // clear flag
    }
}

/**
 * timer
 */
void timer(void) {
    // buzzer tone
    if ( is_tone_enable ) {
        PORTAbits.RA3 = is_buzzer_on;   // Buzzer
        is_buzzer_on = !is_buzzer_on;
    } else {
        PORTAbits.RA3 = 0;   // Buzzer
    }

    // --- 7 seg ---
    // turn off 7seg
    PORTBbits.RB3 = 0;
    PORTBbits.RB1 = 0;
    PORTAbits.RA4 = 0;

    // blank mode
    if ( !is_7seg_on ) return;

    // countup 7 seg row
    row_cnt++;
    if ( row_cnt >= ROW_NUM ) {
        row_cnt = 0; // reset
    }
    int n = SEG_NUM * digit[ row_cnt ];

// 7Seg : RB2 RB0 RB7 RB6 RB5 RA0 RB4
    if ( PAT[n+0] == 1 ) {
        PORTBbits.RB2 = 0;
    } else {
        PORTBbits.RB2 = 1;
    }
    if ( PAT[n+1] == 1 ) {
        PORTBbits.RB0 = 0;
    } else {
        PORTBbits.RB0 = 1;
    }
    if ( PAT[n+2] == 1 ) {
        PORTBbits.RB7 = 0;
    } else {
        PORTBbits.RB7 = 1;
    }
    if ( PAT[n+3] == 1 ) {
        PORTBbits.RB6 = 0;
    } else {
        PORTBbits.RB6 = 1;
    }
    if ( PAT[n+4] == 1 ) {
        PORTBbits.RB5 = 0;
    } else {
        PORTBbits.RB5 = 1;
    }
    if ( PAT[n+5] == 1 ) {
        PORTAbits.RA0 = 0;
    } else {
        PORTAbits.RA0 = 1;
    }
    if ( PAT[n+6] == 1 ) {
        PORTBbits.RB4 = 0;
    } else {
        PORTBbits.RB4 = 1;
    }

// Anode : RB3 RB1 RA4
    if ( row_cnt == 0 ) {
        PORTBbits.RB3 = 1;
        PORTBbits.RB1 = 0;
        PORTAbits.RA4 = 0;
    } else if ( row_cnt == 1 ) {
        PORTBbits.RB3 = 0;
        PORTBbits.RB1 = 1;
        PORTAbits.RA4 = 0;
    } else if ( row_cnt == 2 ) {
        PORTBbits.RB3 = 0;
        PORTBbits.RB1 = 0;
        PORTAbits.RA4 = 1;
    }
}	

/**
 * 333ms timer
 */
void t333ms(void) {
    // 7 seg
    is_7seg_on = true;
    upDigit(false);

    // read button
    bool b = PORTAbits.RA1;    // button
    if ( !b && is_button_prev ) {
        // button change
        if (( status == ST_WAIT )||( status == ST_777 )||( status == ST_END ))  {
            digit[0] = 6;
            digit[1] = 3;
            digit[2] = 0;
            is_digit[0] = true;
            is_digit[1] = true;
            is_digit[2] = true;
            status = ST_PLAY_1;
        } else if ( status == ST_PLAY_1 ) {
            // stop row 0
            is_digit[0] = false;
            if ( is_debug ) {
                digit[0] = 7;
            }
            status = ST_BUTTON_1;
        } else if ( status == ST_PLAY_2 ) {
            // stop row 1
            is_digit[1] = false;
            if ( is_debug ) {
                digit[1] = 7;
            }
            status = ST_BUTTON_2;
        } else if ( status == ST_PLAY_3 ) {
            // stop row 2
            is_digit[2] = false;
            if ( is_debug ) {
                digit[2] = 7;
            }
            status = ST_BUTTON_3;
        }
    }
    is_button_prev = b;

    // --- 1sec timer ---
    t1sec_cnt++;
    if ( t1sec_cnt >= 3 ) {
        t1sec_cnt = 0; // reset
        t1sec();
    }
}

/**
 * 1 sec timer
 */
void t1sec(void) {
    is_1sec_status = !is_1sec_status;
    // LED
    PORTAbits.RA2 = is_1sec_status;  // LED
    // 7seg
    if ((( status == ST_BUTTON_3 )||( status == ST_END ))&& !is_1sec_status ) {
        // blink
        is_7seg_on = false;
    } else if ( status == ST_777 ) {
        // rotate 777
        digit[0] = 7;
        digit[1] = 7;
        digit[2] = 7;
        if ( mode_777 == 0 ) {
            mode_777 = 1;
        } else if ( mode_777 == 1 ) {
            mode_777 = 2;
            digit[0] = 10;
        } else if ( mode_777 == 2 ) {
            mode_777 = 3;
            digit[0] = 10;
            digit[1] = 10;
        } else if ( mode_777 == 3 ) {
            mode_777 = 4;
            digit[0] = 10;
            digit[1] = 10;
            digit[2] = 10;
        } else if ( mode_777 == 4 ) {
            mode_777 = 0;
            is_7seg_on = false;
        }
    }
}

/**
 * count up digit
 */
void upDigit( bool flag ) {
    int i;
    for( i=0; i<ROW_NUM; i++ ) {
        if ( flag || is_digit[i] ) {
            // digit 0-9
            digit[i] ++;
            if ( digit[i] > 9 ) {
                digit[i] = 0;
            }
        }
    }
}