/**
 * PIC16F84A
 * 7 Seg LED
 * 2015-05-10 K.OHWADA
 */

/* --- pin assign --- */
// RA0  7SEG F
// RA1  
// RA2  
// RA3  
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

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
#define _XTAL_FREQ 4000000     // 4MHz

/**
 * PIC16F84A configuraton
 * Watchdog Timer: WDT disabled
 * Power-up Timer Enable bit: Power-up Timer is disabled
 * Code Protection bit: Code protection disabled
 * Oscillator Selection bits: HS oscillator
 */
#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, FOSC = HS 

/* Function Declaration */
void display( int row );
void upDigit(void);
void upSeg( int cnt ) ;

/* Conmodeant */
// 7 digit
#define ROW_NUM  3
#define SEG_NUM  7
#define BLANK  10

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
    0,0,0,0, 0,0,0,	// blank
    1,0,0,0, 0,0,0,	// A
    0,1,0,0, 0,0,0,	// B
    0,0,1,0, 0,0,0,	// C
    0,0,0,1, 0,0,0,	// D
    0,0,0,0, 1,0,0,	// E
    0,0,0,0, 0,1,0,	// F
    0,0,0,0, 0,0,1	// G		
};

/* Gloval Variable */	
int digit[ ROW_NUM ];

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void) {
    /* Initialize I/O and Peripherals for application */
    TRISA = 0x00;   // PortA all output
    TRISB = 0x00;   // PortB all output
    PORTA = 0x00;   // PortA all low
    PORTB = 0x00;   // PortB all low

    // counter
    int row_cnt = 0;  // row 0-2
    int loop_cnt = 0;
    int digit_cnt = 0;  // 0-9
    int seg_cnt = 0;    // A-G
    int mode = 0;

    // endless loop
    int i;
    while(1) {
        for ( i=0; i<ROW_NUM; i++ ) {
            display( i );
            loop_cnt ++;
        }
        if ( loop_cnt > 250 ) {
            // 0.5 sec
            loop_cnt = 0;
            if ( mode == 1 ) {
                // digit mode
                upDigit();
                digit_cnt ++;
                if ( digit_cnt > 2*10 ) {
                    // mode change
                    digit_cnt = 0;
                    mode = 0;
                    digit[0] = 0;
                    digit[1] = 0;
                    digit[2] = 0;
                }
            } else {
                // segment test mode
                upSeg( seg_cnt );
                seg_cnt ++;
                if ( seg_cnt > ROW_NUM * SEG_NUM ) {
                    // mode change
                    seg_cnt = 0;
                    mode = 1;
                    digit[0] = 2;
                    digit[1] = 1;
                    digit[2] = 0;
                }
            }
        }
        __delay_ms(10);
    }
}

/**
 * display
 */
void display( int row ) {
    // turn off 7digit
    PORTBbits.RB3 = 0;
    PORTBbits.RB1 = 0;
    PORTAbits.RA4 = 0;

    int n = SEG_NUM * digit[ row ];

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
    if ( row == 0 ) {
        PORTBbits.RB3 = 1;
        PORTBbits.RB1 = 0;
        PORTAbits.RA4 = 0;
    } else if ( row == 1 ) {
        PORTBbits.RB3 = 0;
        PORTBbits.RB1 = 1;
        PORTAbits.RA4 = 0;
    } else if ( row == 2 ) {
        PORTBbits.RB3 = 0;
        PORTBbits.RB1 = 0;
        PORTAbits.RA4 = 1;
    }
}	

/**
 * up Digit
 */
void upDigit(void) {
    int i;
    for( i=0; i<ROW_NUM; i++ ) {
        // digit 0-9
        digit[i] ++;
        if ( digit[i] > 9 ) {
            digit[i] = 0;
        }
    }
}

/**
 * up Seg
 */
void upSeg( int cnt ) {
    int n = cnt / SEG_NUM;
    int m = cnt % SEG_NUM; 
    int i;
    for( i=0; i<ROW_NUM; i++ ) {
        if ( i == n ) {
            // show one segment
            digit[i] = BLANK + 1 + m;
        } else {
            // turn off
            digit[i] = BLANK;
        }
    }
}
