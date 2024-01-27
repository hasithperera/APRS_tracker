#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
//#include "i2c_primary.c"
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

/*
	i2c_init();
	i2c_write_byte(0x04);
	i2c_write_byte("a");
	i2c_stop();
*/

/*
	initialize();
	char usi_data;
 	usi_data=USIDR;
 	if(usi_data&0x01)
 	i2c_actual_data();  //transmit data
 	i2c_stop();
*/	
	PORTA |= _BV(PA7);
	_delay_ms(1000);
	PORTA &= ~_BV(PA7);
	_delay_ms(50);
}
 
 return 0;
}




void init_io(){

//LED indicator - PA7
// Cutdown - PA5
DDRA |= _BV(PA7) | _BV(PA5);


}
