#include <P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
 
Delay1 equ 0xFC
Delay2 equ 0xFC
D2 equ 0x07
 
 ORG 0x0000
; CODE STARTS FROM THE NEXT LINE
START:
 ORG 0x0000
START:
 MOVLW 0x0F ; Load W with 0x0F0
 MOVWF ADCON1 ; Make ADCON1 to be all digital
 MOVLW 0x00 ; Load W with 0x00
 MOVWF TRISC ; Make PORT C as outputs
 MOVWF TRISD

MAIN_LOOP: ; start of While LOOP
 MOVLW 0x00 ; load W with 0 start for LED D1 at PORT C
 MOVWF 0x20 ; store W to location 0x20
 MOVLW 0x08 ; load W with 7 acting as a counter
 MOVWF 0x21 ; store W to location 0x21
 MOVLW 0x00 ; load W with 1 for SUBLW
 MOVWF 0x22 ; store W to location 0x22
 ;MOVLW D2 ; load d2 intial 
 ;MOVWF 0x23 ; store to location 0x23

FOR_LOOP:
 MOVF 0x20,W ; read content of 0x20 into W
 MOVWF PORTC ; output W to PORT C
 MOVF 0x22,W ; read content of 0x23 into W
 SUBLW D2 ;subtracts 1 from literal at 0x22
 MOVWF PORTD ; output W to PORT D
 CALL DELAY_ONE_SEC ; wait one sec
 ;MOVF 0x22 ; read content of 0x22
  
 INCF 0x22,F ; increment location by 1
 INCF 0x20,F
 INCF 0x20,F; increment location 0x20 by 2 because c0 is broken
 DECF 0x21,F; decrement location 0x21 by 1
 BNZ FOR_LOOP ; if (0x21) not equal to 0
 ; go back to FOR_LOOP
 GOTO MAIN_LOOP ; go back to While LOOP

DELAY_ONE_SEC:
 MOVLW Delay1 ; Load constant Delay1 into W
 MOVWF 0x28 ; Load W to memory 0x21
LOOP_1_OUTER:
 NOP ; Do nothing
 MOVLW Delay2 ; Load constant Delay2 into W
 MOVWF 0x29 ; Load W to memory 0x29
LOOP_1_INNER:
 NOP ; Do nothing
 DECF 0x29,F ; Decrement memory location 0x29
 BNZ LOOP_1_INNER ; If value not zero, go back to
 DECF 0x28,F ; Decrement memory location 0x28
 BNZ LOOP_1_OUTER ; If value not zero, go back to
 RETURN
END