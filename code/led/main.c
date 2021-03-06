/**
 * PIC16F84A
 * LED Blink
 * 2015-04-26 K.OHWADA
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <pic16f84a.h>  /* PIC16F84A */ 

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
#define _XTAL_FREQ 16000000     // 16MHz

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    /* Initialize I/O and Peripherals for application */
    TRISA = 0x00;   // PortA all output
    TRISB = 0x00;   // PortB all output
    PORTA = 0x00;   // PortA all low
    PORTB = 0x00;   // PortB all low

    while(1)
    {
        /* LED blink */
        PORTBbits.RB3 = 1;  // LED on
        __delay_ms(1000);   // 1sec
        PORTBbits.RB3 = 0;  // LED off
        __delay_ms(1000);   // 1sec
    }

}
