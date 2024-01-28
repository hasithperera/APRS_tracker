#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
//#include "i2c_primary.c"

#include <stdint.h>            // has to be added to use uint8_t
#include <avr/interrupt.h>    // Needed to use interrupts

#include "USI_TWI_Master.c" 
//#include "old/i2c.c"

char cmd=0;

char cmds[20];
char cmd_no=0;

char cmd_str[4] ={7,8,9,12};
int cmd_match=1;

int main(void) {

//initialization to check if program mode is on

#define SLAVE_APRS 0x08
#define debug 1


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
	
	while(1){	

//	send_i2c_data(65,SLAVE_APRS);


	if(cmd){
			cmds[cmd_no++] = cmd; //store cmds
			#ifdef debug
				send_i2c_data(cmd+100,SLAVE_APRS); //debug
			#endif
			if(cmd==12 | cmd_no>19){
				
				cmd_match = 1;
				int i;
				for(i=0;i<cmd_no;i++){
					send_i2c_data(cmds[i],SLAVE_APRS); //debug
					
					if(!(cmds[i]==cmd_str[i]))
						cmd_match = 0;
				}
				if(cmd_match){
					cutdown();
				}
				cmd_no = 0;
			}
			cmd = 0;
	}
	
#ifdef debug
		PORTA |= _BV(PA7);
		_delay_ms(50);
 
		PORTA &= ~_BV(PA7);
	 	_delay_ms(50);
#endif

#ifndef debug
	_delay_ms(100);
#endif

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

	cmd = PINA & 0x0f;	
}

//i2c function

void send_i2c_data(char data,char address){

	unsigned char const tempUSISR_8bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Prepare register value to: Clear flags, and
                                 (0x0<<USICNT0);                                     // set USI to shift 8 bits i.e. count 16 clock edges.
  unsigned char const tempUSISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      // Prepare register value to: Clear flags, and
                                 (0xE<<USICNT0); 									// set USI to shift 1 bit i.e. count 2 clock edges.
	
	USI_TWI_Master_Start();
	USIDR = address<<1|0;
	USI_TWI_Master_Transfer(tempUSISR_8bit);
	USI_TWI_Master_Transfer(tempUSISR_1bit);
	USIDR = data;
	USI_TWI_Master_Transfer(tempUSISR_8bit);
	USI_TWI_Master_Transfer(tempUSISR_1bit);
	USI_TWI_Master_Stop();

}


void cutdown(){
	// cutdown sequence
	for(int i=0;i<20;i++){

		PORTA |= _BV(PA5);
		_delay_ms(100);
#ifndef debug
		PORTA &= ~_BV(PA5);
	 	_delay_ms(100);
#endif
	}

	//turn off mosfet
	PORTA &= ~_BV(PA5);
	_delay_ms(100);

}
