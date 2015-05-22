; ====================
;   PIC16F84A LED Blink
;   2015-04-26 K.OHWADA
;
;   CLOCK   4MHz ( 1 us / cycle )
;   PORTA.RA2   LED
; ====================

; ==========
; CPU Definition
; ==========
    LIST        P=PIC16F84A
    INCLUDE P16F84A.INC

; ==========
; CPU Configuraton
;   HS Oscillator
;   Watchdog Timer disable
;   Power-up Timer eable
;   Code Protection disable
; ==========
    __CONFIG _HS_OSC & _WDT_OFF & _PWRTE_ON & _CP_OFF

; ==========
; SRAM (General Purpose Registers) 0Ch - 4Fh
; ==========
D1CNT   EQU     0Ch	
D2CNT   EQU     0Dh
D3CNT   EQU     0Eh

; ==========
; Program
; ==========
; Reset
    ORG      0 
    GOTO    MAIN 
; Interrupt
;   ORG      4
;   GOTO    INTERRUPT

MAIN:
    BSF      STATUS,RP0  ; bunk 1
    CLRF    TRISA           ; PortA all output
    CLRF    TRISB           ; PortB all output
    BCF      STATUS,RP0   ; bunk 0
    CLRF PORTA ; PortA all low
    CLRF PORTB ; PortB all low

LOOP:
    BSF          PORTA,2       ; RA2 on
    CALL        DELAY3        ; delay 1 sec
    BCF          PORTA,2       ; RA2 off
    CALL        DELAY3        ; delay 1 sec
    GOTO       LOOP

; Delay 1,000 us = 1 us x 4 cycle x 250 	
DELAY1:			
    MOVLW       0FAh	
    MOVWF       D1CNT   ; 0FAh(250) -> D1CNT
D1LOOP:   
    NOP                         ; 1 cycle		
    DECFSZ      D1CNT,F ; 1 cycle : D1CNT --
    GOTO	 D1LOOP  ; 2 cycle	
    RETURN			

; Delay 250 ms = 1 ms x 250 
DELAY2:			
    MOVLW       0FAh		
    MOVWF       D2CNT   ; 0FAh(250) -> D2CNT		
D2LOOP: 
    CALL           DELAY1 ; 1 ms	
    DECFSZ       D2CNT,F ; D2CNT --
    GOTO         D2LOOP	
    RETURN				

; Delay 1,000 ms = 250 ms x 4		
DELAY3:			
    MOVLW       003h		
    MOVWF	  D3CNT    ; 004h(4) -> D3CNT		
D3LOOP:  
    CALL          DELAY2    ; 250 ms		
    DECFSZ	  D3CNT,F ; D3CNT --
    GOTO	  D3LOOP			
    RETURN				

    END
; ==========
; Program END
; ==========