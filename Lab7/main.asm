.include "m2560def.inc"

.macro outi															; write from constant
	ldi _swap, @1
	.if @0 < 0x40													; register has max of 0x3F address
		out @0, _swap
	.else
		sts @0, _swap
	.endif
.endmacro

.macro outr															; write from register
	mov _swap, @1
	.if @0 < 0x40
		out @0, _swap
	.else
		sts @0, _swap
	.endif
.endmacro

.def	_low		=r16											; I/O ports states
.def	_high		=r17
.def	_swap		=r18											; register for outi and outr

.def	_logic		=r0												; 1=HIGH - button1; 2=HIGH - button2
.def	_last1		=r19

.def	_delay1		=r20											; registers for delay
.def	_delay2		=r21

.def	algo1		=r22
.def	_first_led	=r23
.def	_one		=r24
.def	_start		=r25    



.org $0000
    rjmp RESET

.org OC4Aaddr
    rjmp TIMER4_COMPA_vect

RESET:
    ldi		_low, 0x00											; 0 - low, 1 (255) - high
	ldi		_high, 0xFF											; _low and _high are needed for I/O ports initialization
															; PORTC - OUTPUT, LOW (PINS PC0-PC7)
	ldi		_one, 0x01
	ldi		_first_led, 0b01000000
	ldi		_last1, 0b00000001
	out		DDRA, _high
	out		PORTA, _low
																	; PORTK - OUTPUT, LOW (PINS PK0-PK7)
	outr	DDRF, _high
	outr	PORTF, _low
																	; PORTD - OUTPUT, LOW (BUZZER PD0)
	out		DDRD, _high
	out		PORTD, _low

	out		DDRC, _high
																	; PORTA - INPUT, PULLUP (BUTTONS PA4, PA6)
	outr		DDRL, _low
	ldi		_high, (1<<DDA4) | (1<<DDA6)
	outr		PORTL, _high
	ldi		_high, 0xFF	
    
	cli
    outi TCCR4B, (1 << WGM42) | (1 << CS42) | (1 << CS40) ;
          
    outi OCR4AH, 0x27
    outi OCR4AL, 0xAB
    
    outi TIMSK4, (1 << OCIE4A)
	sei
    
main:
    lds R26, PINL          
    sbrs R26, 6
    rjmp button2Pressed
	
    rjmp main
    
button2Pressed:
    tst algo1              
    brne turnOffAlgo1      
    
    ldi algo1, 0x01   
	ldi _start, 0b10000000        
    rjmp main
    
turnOffAlgo1:
    ldi algo1, 0x00                      
    out PORTA, _low
    rjmp main

TIMER4_COMPA_vect:
	
    cp algo1, _one              
    breq processAlg2      
    
    reti

    
processAlg2:
	
	cp _start, _first_led
	breq change_led
	cp _start, _last1
	breq turnOffAlgo1
	out PORTA, _start
	lsr _start
	;lsl _start
	;out PORTA, _start
	out PORTC, _high    
    rjmp done
    
change_led:
	ldi _start, 0b00000010
	out PORTA, _start
	rjmp done

resetAlg2:
               
    out PORTF, _low
    rjmp done
    
activateBuzzer2:
    sbi PORTC, 0 
    ldi R16, 10           
    call delay
    cbi PORTC, 0 
    ldi R16, 5     
	       
    call delay
	ldi		_low, 0x00
    rjmp resetAlg2
    
delay:
    push R16              
delayLoop:
    dec R16               
    brne delayLoop        
    pop R16               
    ret

done:
    ret

.include "m2560def.inc"