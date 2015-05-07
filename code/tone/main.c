/**
 * PIC16F84A
 * buzzer tone
 * 2015-05-03 K.OHWADA
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <pic16f84a.h>  /* PIC16F84A */ 

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
/* frequency of XTAL */
#define _XTAL_FREQ 16000000     // 16MHz

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
#define OPTION_REG_VALUE  0x04

// _XTAL_FREQ / ( 4 * 32 ) = 125 Hz
#define PRE_INTERVAL  125000

/* Timer 0 */
#define TMR0_DO4 17
#define TMR0_RE 43
#define TMR0_MI 67
#define TMR0_FA 77
#define TMR0_SO 97
#define TMR0_RA 114
#define TMR0_SI 129
#define TMR0_DO5 137

/**
 * PIC16F84A configuraton
 * Watchdog Timer: WDT disabled
 * Power-up Timer Enable bit: Power-up Timer is disabled
 * Code Protection bit: Code protection disabled
 * Oscillator Selection bits: HS oscillator
 */
#pragma config WDTE = OFF, PWRTE = OFF, CP = OFF, FOSC = HS 

/* Function Declaration */
void tone( int tmr );
void noTone(void);

/* Gloval Variable */
unsigned int tmr0_value = 0;    // TMR0 value
unsigned int led_max = 0; // max of LED counter
unsigned int led_counter = 0; // LED counter
unsigned char is_timer_on = 0; // Timer status on / off
unsigned char is_led_on = 0; // LED status on / off
unsigned char is_buzzer_on = 0; // buzzer status on / off
unsigned char is_tone_enable  = 0; // Timer enable flag

/**
 * main
 */
int main(void) {
    /* Initialize I/O and Peripherals for application */
    TRISA = 0x00;   // PortA all output
    TRISB = 0x00;   // PortB all output
    PORTA = 0x00;   // PortA all low
    PORTB = 0x00;   // PortB all low

    // Option
    OPTION_REG = OPTION_REG_VALUE ;
    // Timer 0
    tmr0_value = TMR0_RA ;
    TMR0 = TMR0_RA ;
    // TMR0 Overflow Interrupt Enable bit = Enables
    INTCONbits.TMR0IE = 1;
    // Global Interrupt Enable bit = Enables
    INTCONbits.GIE = 1;

    while(1) {
        tone( TMR0_DO4 );
        __delay_ms(1000);
        tone( TMR0_RE );
        __delay_ms(1000);
        tone( TMR0_MI );
        __delay_ms(1000);
        tone( TMR0_FA );
        __delay_ms(1000);
        tone( TMR0_SO );
        __delay_ms(1000);
        tone( TMR0_RA );
        __delay_ms(1000);
        tone( TMR0_SI );
        __delay_ms(1000);
        tone( TMR0_DO5 );
        __delay_ms(1000);
        noTone();
        __delay_ms(5000);
    }
}

/**
 * tone
 */
void tone( int tmr ) {
    // Timer 0
    tmr0_value = tmr;
    TMR0 = tmr;
    // Buzzer
    PORTBbits.RB5 = 0;
    is_buzzer_on = 0;
    is_tone_enable = 1;
    // LED
    led_max = PRE_INTERVAL / ( 256 - tmr );
}

/**
 * noTone
 */
void noTone(void) {
    PORTBbits.RB5 = 0;  // Buzzer
    is_buzzer_on = 0;
    is_tone_enable = 0;
}

/**
 * interrupt
 */
static void interrupt isr(void) {
/* 880 Hz @ TMR0_RA */
    // TMR0 Overflow Interrupt Flag bit : overflowed ?
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0; // clear flag
        TMR0 = tmr0_value;
        // Timer pin
        PORTBbits.RB3 = is_timer_on;  // Timer
        is_timer_on = ( is_timer_on == 1 )? 0: 1;
        // Buzzer pin
        if ( is_tone_enable ) {
            PORTBbits.RB5 = is_buzzer_on;  // Buzzer
            is_buzzer_on = ( is_buzzer_on == 1 )? 0: 1;
        }
        // LED pin
        led_counter++;
        // 1 sec
        if ( led_counter >= led_max ) {
            led_counter = 0; // reset
            PORTBbits.RB4 = is_led_on;  // LED
            is_led_on = ( is_led_on == 1 )? 0: 1;            
        }
    }
}
