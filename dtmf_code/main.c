#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
//#include "i2c_primary.c"

#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>    // Needed to use interrupts

#include "USI_TWI_Master.c" 
//#include "old/i2c.c"

char cmd=0;
unsigned char msg_buff[10];
unsigned char slave_add,temp,myData=0;
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

	//i2c test
	
	msg_buff[0] = (0x08 << 1) | 0x01;
	msg_buff[1] = 0x88;

	while(1){	
	//I2C tests
	USI_TWI_Master_Start();
	 unsigned char const tempUSISR_8bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Prepare register value to: Clear flags, and
                                 (0x0<<USICNT0);                                     // set USI to shift 8 bits i.e. count 16 clock edges.
  unsigned char const tempUSISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Prepare register value to: Clear flags, and
                                 (0xE<<USICNT0); 									// set USI to shift 1 bit i.e. count 2 clock edges.
	

	USIDR = 0x08<<1|0;
	USI_TWI_Master_Transfer(tempUSISR_8bit);
	USI_TWI_Master_Transfer(tempUSISR_1bit);
	USIDR = 65;
	USI_TWI_Master_Transfer(tempUSISR_8bit);
	USI_TWI_Master_Transfer(tempUSISR_1bit);
	USI_TWI_Master_Stop();
	


/*
	if(cmd){
		for(cmd;cmd>0;cmd--){
			PORTA |= _BV(PA7);
			_delay_ms(1);
			PORTA &= ~(_BV(PA7));
			_delay_ms(1);
		}
		PORTA &= ~(_BV(PA5));
	}
*/
	

	_delay_ms(1000);


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

USI_TWI_Master_Initialise();

}


ISR(INT0_vect){

	PORTA |= _BV(PA5);
	cmd = PINA & 0x0f;	

}
