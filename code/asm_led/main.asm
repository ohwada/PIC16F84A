; **************************************************
;   PIC16F84A LED Blink
;   2015-04-26 K.OHWADA
;
;   CLOCK   4MHz ( 1 us / cycle )
;   PORTA.RA2   LED
; **************************************************

; ==========
; CPU Definition
; ==========
    list            p=16F84A            ; list directive to define processor
    #include    <p16F84A.inc>   ; processor specific variable definitions

; ==========
; CPU Configuraton
;   Code Protection disable
;   Power-up Timer eable
;   Watchdog Timer disable
;   HS Oscillator
; ==========
    __CONFIG  _CP_OFF & _PWRTE_ON & _WDT_OFF & _HS_OSC

; ==========
; VARIABLE DEFINITIONS
; SRAM (General Purpose Registers) 0Ch - 4Fh
; ==========
d1cnt   EQU     0x0C	
d2cnt   EQU     0x0D
d3cnt   EQU     0x0E

; ==========
; Program
; ==========
; processor reset vector
    ORG      0x000 
    goto    Main 
; interrupt vector location
;   ORG      0x004

Main:
    bsf    STATUS,RP0  ; bunk 1
    clrf    TRISA           ; PortA all output
    clrf    TRISB           ; PortB all output
    bcf    STATUS,RP0   ; bunk 0
    clrf    PORTA       ; PortA all low
    clrf    PORTB       ; PortB all low

Loop:
    bsf        PORTA,2       ; RA2 on
    call        Delay3        ; delay 1 sec
    bcf        PORTA,2       ; RA2 off
    call        Delay3        ; delay 1 sec
    goto       Loop

; Delay 1,000 us = 1 us x 4 cycle x 250 	
Delay1:			
    movlw       0xfa	
    movwf       d1cnt   ; 0xfa(250) -> d1cnt
D1Loop:   
    nop                         ; 1 cycle		
    decfsz      d1cnt,F ; 1 cycle : d1cnt --
    goto	 D1Loop  ; 2 cycle	
    return			

; Delay 250 ms = 1 ms x 250 
Delay2:			
    movlw       0xfa		
    movwf       d2cnt   ; 0xfa(250) -> d2cnt		
D2Loop: 
    call           Delay1 ; 1 ms	
    decfsz       d2cnt,F ; d2cnt --
    goto         D2Loop	
    return				

; Delay 1,000 ms = 250 ms x 4		
Delay3:			
    movlw       0x04		
    movwf	  d3cnt    ; 0x04(4) -> d3cnt		
D3Loop:  
    call          Delay2    ; 250 ms		
    decfsz	  d3cnt,F ; d3cnt --
    goto	  D3Loop			
    return				

    END
; ==========
; Program END
; ==========