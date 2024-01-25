#include<avr/io.h>

char tosend[5]="test";

//Attiny-84 pinout
//PA4 -SCL
//PA6 - SDA

void i2c_ack()
{
  DDRA&=~(1<<PA6);  //Change direction of SDA to receive acknowledge bit
  USISR|=(1<<USICNT3)|(1<<USICNT2)|(1<<USICNT1); //Set counter to 1110 to force it to overflow when ACK bit is received
  i2c_transfer();   //Byte transmission
}

void i2c_transfer()
{
  do
  {
  USICR|=(1<<USITC);   //Clock signal from SCL
  while((PINA&(1<<PA4))); //Waiting for SCL to go high
  _delay_us(5);
  USICR|=(1<<USITC);  //Toggle SCL to go low
  _delay_us(5);
  }while(!(USISR&(1<<USIOIF)));  //Repeat clock generation at SCL until the counter overflows and a byte is transferred
  _delay_us(5);
  USISR|=(1<<USIOIF);      //Clear overflow flag
}
void i2c_actual_data()
{
  DDRA|=(1<<PA6);      
  short int i=0;
  while(tosend[i]!='\0')  //Actual data string "test"
  {
    USIDR=tosend[i];     //Placing byte in Data register
    i2c_transfer();      //Transfer the data placed in register
    i++;
  }
}

//Attiny-84 pinout
//PA4 -SCL
//PA6 - SDA


void i2c_start()
{
  PORTA&=~(1<<PA6);  //Pulling SDA line low
  _delay_us(5);
  PORTA&=~(1<<PA4);  //Pulling SLC line low
  _delay_us(5);
  while(USISIF==1); //detection of start condition
}

void initialize()
{
  USICR=(1<<USIWM1)|(1<<USICS1)|(1<<USICLK);  //TWI mode
  DDRA=(1<<PA4)|(1<<PA6); //SDA & SCL direction as output
  PORTA=(1<<PA4)|(1<<PA6); //SDA & SCL default state
  i2c_start();
  USIDR=0; //address of slave and select write operation
  i2c_transfer();
  i2c_ack();
}

void i2c_stop()
{
  PORTA|=(1<<PA6);       //Pulling SDA high 
  _delay_us(5);
  PORTA|=(1<<PA4);       //Pulling SCL low
  _delay_us(5);
}

/*
int main()
{
 initialize();      //initialize i2c
 char usi_data;
 usi_data=USIDR;
 if(usi_data&0x01)
 i2c_actual_data();  //transmit data
 i2c_stop();         //i2c stop
 return 0;
}
*/
