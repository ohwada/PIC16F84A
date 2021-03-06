/**
 * PIC16F84A
 * blink LED with timer interrupt
 * 2015-05-03 K.OHWADA
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <pic16f84a.h>  /* PIC16F84A */ 
#include <stdbool.h>    /* bool */

/******************************************************************************/
/* CPU Configuraton                                          */
/******************************************************************************/
/**
 * Oscillator Selection bits: HS oscillator
 * Watchdog Timer: WDT disabled
 * Power-up Timer Enable bit: Power-up Timer is eabled
 * Code Protection bit: Code protection disabled
 */
#pragma config FOSC = HS, WDTE = OFF, PWRTE = ON, CP = OFF 

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
#define OPTION_REG_VALUE  0x07  // 1/256

/* Timer 0 */
#define TMR0_VALUE 0    // 1/256

// _XTAL_FREQ / ( 4 * 256 * 256 ) = 61.035 Hz ( 16.38 ms )
#define TMR0_INTERVAL 61

/* Gloval Variable */
unsigned int led_max = TMR0_INTERVAL; // max of LED counter
unsigned int led_counter = 0; // LED counter
bool is_led_on = 0; // LED status on / off
bool is_timer_on = 0; // Timer status on / off

/**
 * main
 */
int main(void) {
    /* Initialize I/O and Peripherals for application */
    TRISA = 0x00;   // PortA all output
    TRISB = 0x00;   // PortB all output
    PORTA = 0x00;   // PortA all low
    PORTB = 0x00;   // PortB all low

    // OPTION_REG
    OPTION_REG = OPTION_REG_VALUE ;
    // Timer 0
    TMR0 = TMR0_VALUE ; 
    // TMR0 Overflow Interrupt Enable bit = Enables
    INTCONbits.TMR0IE =1; 
    // Global Interrupt Enable bit = Enables
    INTCONbits.GIE = 1;

    // endless loop
    while(1) {
        // dummy
    }
}

/**
 * interrupt
 */
static void interrupt isr(void) {
/* timer 16.38 ms */
    // TMR0 Overflow Interrupt Flag bit : overflowed ?
    if (INTCONbits.TMR0IF == 1) {
        INTCONbits.TMR0IF = 0; // clear flag
        TMR0 = TMR0_VALUE;
        // Timer pin 30.5 Hz
        PORTBbits.RB3 = is_timer_on;  // Timer
        is_timer_on = !is_timer_on;  // bit invert
        // LED pin 1sec
        led_counter++;
        if ( led_counter >= led_max ) {
            led_counter = 0; // reset
            PORTBbits.RB4 = is_led_on;  // LED
            is_led_on = !is_led_on;  // bit invert
        }
    }
}
