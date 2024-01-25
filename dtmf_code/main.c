#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>

#include "i2c.c"

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

 
while(1){
 
	initialize();      //initialize i2c
	char usi_data;
	usi_data=USIDR;
	if(usi_data&0x01)
	i2c_actual_data();  //transmit data
	i2c_stop();         //i2c stop


	PORTA |= _BV(PA7);
	_delay_ms(100);
 
	PORTA &= ~_BV(PA7);
	_delay_ms(500);
}
 
 return 0;
}




void init_io(){

//LED indicator
DDRA |= _BV(PA7) | _BV(PA6);


}
