#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
//#include "i2c_primary.c"
//#include "i2c.c"

#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>    // Needed to use interrupts


char cmd=0;

int main(void) {


//initialization to check if program mode is on

if(PINA & _BV(PA7)){
	while(1){
		//DELAY LOOP FOR PROGRAMMING
		//All external devices are disabled
		
		//slow blink - programming mode	
		PORTA |= _BV(PA7);
		_delay_ms(500);
 
		PORTA &= ~_BV(PA7);
	 	_delay_ms(500);
	}
}

// normal program
	 
	init_io(); 
	sei();

	while(1){

	if(cmd){
		for(cmd;cmd>0;cmd--){
			PORTA |= _BV(PA7);
			_delay_ms(1);
			PORTA &= ~(_BV(PA7));
			_delay_ms(1);
		}
		PORTA &= ~(_BV(PA5));
	}

	_delay_ms(100);
//	if(cmd)
//	}
	} 
 return 0;
}




void init_io(){

//LED indicator - removed (low voltage detected)

// input pins from DTMF
// PA0,PA1,PA2,PA3
// PB2 - tone precent pin.

//PA5 - Cutdown
//PA7 - Drouge 

DDRA |= _BV(PA7) | _BV(PA5);

// input 

// INT0 interrupt pin

//MCUCR |= (1<< ISC01) | (1<<ISC00); // rising edge
MCUCR |= (1<< ISC01); // falling edge
GIMSK |= (1<<INT0);
SREG |= (1<<7);


}


ISR(INT0_vect){

	PORTA |= _BV(PA5);
	cmd = PINA & 0x0f;	

}
