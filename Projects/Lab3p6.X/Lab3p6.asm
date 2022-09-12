#include <P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
 
Delay1 equ 0xFC
Delay2 equ 0xFC
 ORG 0x0000
; CODE STARTS FROM THE NEXT LINE
START:
 ORG 0x0000
START:
 MOVLW 0x0F ; Load W with 0x0F0
 MOVWF ADCON1 ; Make ADCON1 to be all digital
 MOVLW 0x00 ; Load W with 0x00
 MOVWF TRISC ; Make PORT C as outputs
 MOVLW 0x00;
 MOVWF TRISD ; make port D output
 

MAIN_LOOP: ; start of While LOOP
 MOVLW 0x00 ; load W with 2 or 0010
 MOVWF 0x20 ; store W to location 0x20
 MOVLW 0x08 ; load W with 14 pr 1110
 MOVWF 0x21 ; store W to location 0x21
 
 ;D2 and D3 LED here
 MOVLW 0xC3 ; load W 
 MOVWF 0x30 ; store W to location 0x30
 MOVLW 0xE1 ; load W with 14 pr 1110
 MOVWF 0x31 ; store W to location 0x31
 MOVLW 0x47 ; load W with 2 or 0010
 MOVWF 0x32 ; store W to location 0x32
 MOVLW 0x62 ; load W with 14 pr 1110
 MOVWF 0x33 ; store W to location 0x20
 MOVLW 0x86 ; load W with 2 or 0010
 MOVWF 0x34 ; store W to location 0x20
 MOVLW 0x20 ; load W with 14 pr 1110
 MOVWF 0x35
 MOVLW 0x04 ; load W with 2 or 0010
 MOVWF 0x36 ; store W to location 0x20
 MOVLW 0xA5 ; load W with 14 pr 1110
 MOVWF 0x37
 
 MOVLW 0x30
 MOVWF FSR0L ; set as least significant bit
 
FOR_LOOP:
 MOVF 0x20,W	    ; read content of 0x20 into W
 MOVWF PORTC	    ; output W to PORT C
 MOVF INDF0,W	    ; read 
 MOVWF PORTD	    ; output w to port D
 CALL DELAY_ONE_SEC ; wait one sec
 
 INCF FSR0L	    ; increment FSR
 
		    ;D1 LED 
 INCF 0x20,F
 INCF 0x20,F	    ; increment location 0x20 by 2
		    ;Counter for BNZ
 DECF 0x21,F; decrement location 0x21 by 2
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