/*****************************************************************************
*
*
* File              USI_TWI_Master.h compiled with gcc
* Date              Friday, 10/31/08		Boo!
* Updated by        jkl
*
*
* Supported devices : All device with USI module can be used.
*                     The example is written for the ATtiny2313
*
* AppNote           : AVR310 - Using the USI module as a TWI Master
*
* This is modified to just do I2C communication on ATtiny2313 running at 
*	1MHz. Fast mode is probably possible, but would need a faster clock
*	and has not been tested.
*
*	12/15/08	Added declaration of USI_TWI_Start_Memory_Read	-jkl
****************************************************************************/
    
//********** Defines **********//

// Defines controlling timing limits - SCL <= 100KHz.

#define SYS_CLK   1000.0  // [kHz]	Default for ATtiny2313

// For use with _delay_us()
#define T2_TWI    5 		// >4,7us
#define T4_TWI    4 		// >4,0us

// Defines error code generating
//#define PARAM_VERIFICATION
//#define NOISE_TESTING
#define SIGNAL_VERIFY		// This should probably be on always.

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

// Note these have been renumbered from the Atmel Apps Note. Most likely errors are now
//		lowest numbers so they're easily recognized as LED flashes.
#define USI_TWI_NO_DATA             0x08  // Transmission buffer is empty
#define USI_TWI_DATA_OUT_OF_BOUND   0x09  // Transmission buffer is outside SRAM space
#define USI_TWI_UE_START_CON        0x07  // Unexpected Start Condition
#define USI_TWI_UE_STOP_CON         0x06  // Unexpected Stop Condition
#define USI_TWI_UE_DATA_COL         0x05  // Unexpected Data Collision (arbitration)
#define USI_TWI_NO_ACK_ON_DATA      0x02  // The slave did not acknowledge  all data
#define USI_TWI_NO_ACK_ON_ADDRESS   0x01  // The slave did not acknowledge  the address
#define USI_TWI_MISSING_START_CON   0x03  // Generated Start Condition not detected on bus
#define USI_TWI_MISSING_STOP_CON    0x04  // Generated Stop Condition not detected on bus
#define USI_TWI_BAD_MEM_READ	    0x0A  // Error during external memory read

// Device dependant defines - These for ATtiny2313.

    #define DDR_USI             DDRA
    #define PORT_USI            PORTA
    #define PIN_USI             PINA
    #define PORT_USI_SDA        PORTA6
    #define PORT_USI_SCL        PORTA4
    #define PIN_USI_SDA         PINA6
    #define PIN_USI_SCL         PINA4

// General defines
#define TRUE  1
#define FALSE 0

//********** Prototypes **********//

void              USI_TWI_Master_Initialise( void );
unsigned char USI_TWI_Start_Random_Read( unsigned char * , unsigned char );
unsigned char USI_TWI_Start_Read_Write( unsigned char * , unsigned char );
unsigned char USI_TWI_Get_State_Info( void );
