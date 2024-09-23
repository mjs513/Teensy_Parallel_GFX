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
#include <Wire.h>
/* FT6x36 definitions */
#define FT6X36_I2C_ADDRESS 0x38
#define FT6X36_DEFAULT_THRESHOLD 40

#define FT6X36_REGISTERS_1 7  // Reads up through P1_YL - not sure if we need weight or Misc for 1 point
#define FT6X36_REGISTERS_2 15 // reads through P2_YL...

#define FT6X36_TOUCH1_XH 0x03
#define FT6X36_GEST_ID 0x01
#define FT6X36_DEVICE_MODE 0x00
// number of touched points [3:0] , valid 1..5
#define FT6X36_TD_STATUS 0x02 // n. of touch points [R]
/*
bit[7:0] the value will be x4 times the register value (280)
*/
#define FT6X36_ID_G_THGROUP 0x80 // valid touching detect threshold [R/W]
/*
bit[7:0]  (60)
*/
#define FT6X36_ID_G_THPEAK 0x81 // valid touching peak detect threshold [R/W]
/*
bit[7:0]  (16)
*/
#define FT6X36_ID_G_THCAL 0x82 // the threshold when calculating the focus of touching[R/W]
/*
bit[7:0] value it's x32 times the register val (20)
*/
#define FT6X36_ID_G_THDIFF 0x85 // the threshold where the coordinatis differs from orig[R/W]
#define FT6X36_ID_G_CLTR 0x86   // power control mode[R/W]
#define FT6X36_REGISTER_G_MODE 0xA4 // 00 - Interrupt polling mode, 01 - Interrupt trigger mode

class FT6x36_t4 {
  public:
    // Constructor:
    //   CTP_INT - Touch Interrupt pin - if not 0xff, the touched will only return true when
    //             the pin has signalled a touch has happended.
    //   max_touch - can be 1 or 2. limits how many bytes that are read in.
    FT6x36_t4(uint8_t CTP_INT = 0xff, uint8_t max_touch = 2); // default to not do interrupt unless user specifies pin.
    bool begin(TwoWire *pwire = &Wire, uint8_t wire_addr = FT6X36_I2C_ADDRESS);
    void setThreshold(uint8_t val);
    bool touchPoint(uint16_t &x, uint16_t &y, uint8_t n = 0);
    bool readData();
    bool touched();
    uint8_t gesture();
    bool writeRegister(uint8_t reg, uint8_t val);
    uint8_t readRegister(uint8_t reg);
    void showAllRegisters();

  private:
  	// Spec says > 60 per second
  	enum {TOUCH_TIMEOUT_MS = 17};
    static void isr(void);
    TwoWire *_pwire;
    uint8_t _wire_addr;
    const uint8_t coordRegStart[2] = {{0x03}, {0x09}};
    uint8_t _regs[FT6X36_REGISTERS_2];
    bool _valid_data = false;
    uint8_t _max_touch = 2;
    uint32_t _time_last_read;
    static uint8_t _ctpInt;
    static volatile bool _touched;
};

#endif