#define F_CPU 800000UL


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
 
	PORTA |= _BV(PA7);
	_delay_ms(100);
 
	PORTA &= ~_BV(PA7) ;
	 _delay_ms(100);
}
 
 return 0;
}




void init_io(){

//LED indicator
DDRA = DDRA | _BV(PA7);


}
