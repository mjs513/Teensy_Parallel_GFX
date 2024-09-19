/************************************************************************
This is a library to handling capacitive multitouch sensors using FT5206.
Originally written to work with ER-TFTM070-5 (LCD module) from EastRising.

Written by Helge Langehaug, February 2014

BSD license, all text above must be included in any redistribution
-------------------------------------------------------------------------------
Modified by Max MC Costa from .s.u.m.o.t.o.y. to correctly return coordinates,
fixed initialization, added gesture, touch state and faster operations.
works with Teensy 3,3.1,LC and Arduino's (dunno DUE)
version:1.0
*************************************************************************/

#ifndef _FT6X36_H_
#define _FT6X36_H_
#include "Arduino.h"

/* FT6x36 definitions */
#define FT6X36_I2C_ADDRESS 			0x38
#define FT6X36_REGISTERS 			16     // there are more registers, but this is enought to get all 5 touch coordinates.                    
#define FT6X36_TOUCH1_XH 			0x03
#define FT6X36_GEST_ID 				0x01
#define FT6X36_DEVICE_MODE 			0x00
//number of touched points [3:0] , valid 1..5
#define FT6X36_TD_STATUS 			0x02//n. of touch points [R]
/*
bit[7:0] the value will be x4 times the register value (280)
*/
#define FT6X36_ID_G_THGROUP 		0x80//valid touching detect threshold [R/W]
/*
bit[7:0]  (60)
*/
#define FT6X36_ID_G_THPEAK 			0x81//valid touching peak detect threshold [R/W]
/*
bit[7:0]  (16)
*/
#define FT6X36_ID_G_THCAL 			0x82//the threshold when calculating the focus of touching[R/W]
/*
bit[7:0] value it's x32 times the register val (20)
*/
#define FT6X36_ID_G_THDIFF	 		0x85//the threshold where the coordinatis differs from orig[R/W]
#define FT6X36_ID_G_CLTR	 		0x86//power control mode[R/W]

/*
TOUCH - XH
bit[7:6] event flag -> 00:put down
					-> 01:put up
					-> 10:contact
					-> 11: (reserved)
bit[5:4] (reserved)
bit[3:0] touch MSB X pos (pix)
TOUCH - XL
bit[7:0] touch LSB X pos (pix)
TOUCH - YH
bit[7:4] touch ID of Touch Point
bit[3:0] touch MSB Y pos (pix)
TOUCH - YL
bit[7:0] touch LSB Y pos (pix)
*/


class FT6x36_t4 {
 public:
 	// Constructor: CTP_INT - optional pin interrupt pin number.
 	//   not 0xff - the touched will only return true when the pin has signalled a touch
 	//   has happended.
	FT6x36_t4(uint8_t CTP_INT=0xff); // default to not do interrupt unless user specifies pin.
	bool 				begin(TwoWire *pwire = &Wire, uint8_t wire_addr = FT6X36_I2C_ADDRESS);
	bool 				touchPoint(uint16_t &x, uint16_t &y, uint8_t n = 0);
	bool 				readData();
	bool 				touched();
	uint8_t 			gesture();
	bool 				writeRegister(uint8_t reg,uint8_t val);
 private:
	static void 		 isr(void);
	TwoWire 			*_pwire;
	uint8_t 			_wire_addr;
	uint8_t 			_ctpInt;
	const uint8_t coordRegStart[2] = {{0x03},{0x09}};
	uint8_t 			_regs[FT6X36_REGISTERS];
	bool           		_valid_data = false;
	static volatile bool _touched;
};

#endif