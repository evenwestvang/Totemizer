/*
   
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h> 
#include <stdlib.h>
#include <avr/sleep.h>
#include "device.h"
#include "naive_osc.h"
#include "wiring_serial.h"
#include "i2cmaster.h"

void setDebugLed (uint8_t state) {
  if (state) {
		DEBUG_PORT |= 1 << DEBUG_PIN; 		// LED on
  } else {
		DEBUG_PORT &= ~( 1 << DEBUG_PIN );	// LED off
  }
}

// Read out the specified analog channel to 10 bits (From Baby Orangutan library)
uint16_t analog10(uint8_t channel) 
{
	// Begin by setting up the MUX so it knows which channel
	// to connect to the ADC:

	// Clear the channel selection (low 5 bits in ADMUX)
	ADMUX &= ~0x1F;
	// Select the specified channel
	ADMUX |= channel;
	// ADC start conversion
	ADCSRA |= (1 << ADSC);
	// We wait for conversion to complete by watching the
	// ADSC bit on the ADCSRA register.
	while (bit_is_set(ADCSRA, ADSC));
	return (ADCL | ADCH << 8);	// read ADC (full 10 bits);
}

int main (void) { 

  uint8_t ret;
  uint16_t dx, dy, dz, brightness;

  // set DEBUG LED pin to output
	DEBUG_DDR |= 1 << DEBUG_PIN;			

  // Helo
  setDebugLed(0);
  _delay_ms(150);
  setDebugLed(1);
  _delay_ms(150);
  setDebugLed(0);
  _delay_ms(150);
  setDebugLed(1);
  _delay_ms(150);
  setDebugLed(0);

  /* ------------------------------ BEGIN HARDWARE CONFIGURATION -------------------------------- */

  // The Orangutans have an external reference voltage on the AREF
  // pin tied to our +5V regulated Vcc.  We want to set our ADC to
  // use this as our reference.  The ADMUX register needs REFS0
  // set to 1, and REFS1 set to zero for this mode.
  ADMUX = (1 << REFS0 );

  // The ADC Control and Status Register A sets up the rest of 
  // what we need.  Three bits ADPS0, 1, and 2, set the prescale
  // for how fast our conversions are done.  In this example
  // we use CPU/128, or mode 7:
  ADCSRA = (6 << ADPS0);	

  // Conversions take 13 ADC cycles to complete the sample and hold.
  // Dividing our CPU by 128 gives us 12019 samples/sec.

  // Finally, we enable the ADC subsystem:
  ADCSRA |= (1 << ADEN);

  // Setup serial comms 
  // TODO: increase bit rate to at least 115200
  _delay_ms(500);
  beginSerial(9600);   
  // printString('\nAT+BTURT=115200,8,0,1,0\n');
  // _delay_ms(500);
  // beginSerial(115200);
  
  // Init I2C interface
  i2c_init();                                

  // Check to see if accelerometer is present
  ret = i2c_start(DEV_LIS3LV02DQ+I2C_WRITE);       
  if (ret) {
    // Fail
    i2c_stop();
    setDebugLed(1);
    return(0); 
  }

  // Setup accelerometer
  i2c_start_wait(DEV_LIS3LV02DQ+I2C_WRITE);
  i2c_write(0x20);                            
  i2c_write(0xC7);

  i2c_stop();

  // Enable interrupts
  sei(); 

  // Display max brightness
  // serialWrite(0x7c);
  // serialWrite(157);
  
    
  /* ----------------------------- END OF HARDWARE CONFIGURATION -------------------------------- */
  


  /* -------------------------------------- MAIN LOOP ------------------------------------------- */

  while (1) {

    brightness = analog10(0);

    // Talk i2c
    i2c_start_wait(DEV_LIS3LV02DQ+I2C_WRITE);
    // Read out consecutive bytes starting with X-axis at 0x28. 0x80 toggles consecutive read
    i2c_write(0x28|0x80);
    i2c_start_wait(DEV_LIS3LV02DQ+I2C_READ);
    dx = i2c_readAck();
    dx |= i2c_readAck() << 8;
    dy = i2c_readAck();
    dy |= i2c_readAck() << 8;
    dz = i2c_readAck();
    dz |= i2c_readNak() << 8;
    oscWrite(brightness, dx, dy, dz);                  // read LSB
    _delay_ms(8);

  }
}
