#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
 
int main(void) {
 

//initialization to check if program mode is on

if(PINA & _BV(PA7)){
	while(1){
		_delay_ms(500);
		//DELAY LOOP FOR PROGRAMMING
		//All external devices are disabled
		
	}
}

 
while(1){
 
	PORTA |= _BV(PA7) | _BV(PA6);
	_delay_ms(50);
 
	PORTA &= ~_BV(PA7) & ~_BV(PA6) ;
	 _delay_ms(50);
}
 
 return 0;
}




void init_io(){

//LED indicator
DDRA |= _BV(PA7) | _BV(PA6);


}
