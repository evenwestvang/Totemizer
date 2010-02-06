
/*
  A so-naive-you-may-laught-at-it implementation of OSC to send four 32 bit integers over the wire to the address /t

  If the serial stream is coming off a bluetooth serial port you might want to pipe it to UDP with socat:
  socat -vv -d -x -d UDP:localhost:6449 /dev/cu.SparkFun-BT-SerialPort,clocal=1,nonblock=1,cread=1,cs8,ixon=1,ixoff=1  

  TODO: Perhaps to generate arbitrary OSC packets at some point

  Not to self: Package is as follows
      --
      /t,iiii> 2010/01/26 21:53:27.429597  length=28 from=3024 to=3051
       2f 74 00 00 2c 69 69 69 69 00 00 00 00 00 00 01  /t..,iiii.......
       00 00 00 02 00 00 00 03 00 00 00 04              ............
      --
*/

#include <stdint.h>
#include "wiring_serial.h"
#include "naive_osc.h"

uint8_t packet_data[] = { 0x2f, 0x74, 0x00, 0x00, 0x2c, 0x69, 0x69, 0x69, 0x69, 0x00, 0x00, 0x00};

void oscWrite(uint16_t val1, uint16_t val2, uint16_t val3, uint16_t val4) {
  unsigned char *s = &packet_data[0];

  // Dump header
  int i;
  for (i = 0; i < sizeof(packet_data); i++) {
		printByte(*s++);
  }

  // Write values
  oscWriteInt(val1);
  oscWriteInt(val2);
  oscWriteInt(val3);
  oscWriteInt(val4);

}

void oscWriteInt(uint16_t val) {
  printByte(0);
  printByte(0);
  // Print highbyte
  printByte(val >> 8);
  // Print lowbyte
  printByte(val);
}