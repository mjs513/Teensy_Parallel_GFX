/************************************************************************
This is a library to handling capacitive multitouch sensors using FT6x36.
Originally written to work with some of the BuyDisplay 

Some of this was derived from stuff Written by Helge Langehaug, February 2014

BSD license, all text above must be included in any redistribution
-------------------------------------------------------------------------------
*************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include "FT6x36_t4.h"


static volatile boolean FT6x36_t4::_touched = false;


FT6x36_t4::FT6x36_t4(uint8_t CTP_INT)
{
    _ctpInt = CTP_INT;
}


void FT6x36_t4::isr(void)
{
	_touched = true;
}


//in EXTRLN the entire ISR it's not handled by the library!
bool FT6x36_t4::begin(TwoWire *pwire, uint8_t wire_addr) 
{
	_pwire = pwire;
	_wire_addr = wire_addr;
	_touched = false;

    _pwire->begin();
	_pwire->setClock(400000UL); // Set I2C frequency to 400kHz
	delay(10);

	if (!writeRegister(FT6X36_DEVICE_MODE,0)) return false;
	if (_ctpInt != 0xff){
		pinMode(_ctpInt ,INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(_ctpInt),isr,FALLING);
	}
	return true;
}
 
//in safe mode it will also disconnect interrupt!
bool FT6x36_t4::touched()
{
	// Start off ignore if we have interrupt stuff 
	// lets read in everything...	
	if ((_ctpInt != 0xff) && !_touched) return false;

	if (!readData()) return false; // read failed.

	uint8_t touch_points = _regs[FT6X36_TD_STATUS] & 0xf;

	bool fRet = ((touch_points > 0) && (touch_points <= 2));
	if (!fRet) _touched = false; 
	return fRet;
}

bool FT6x36_t4::writeRegister(uint8_t reg,uint8_t val)
{
    _pwire->beginTransmission(FT6X36_I2C_ADDRESS);
    _pwire->write(reg);
    _pwire->write(val);
    return _pwire->endTransmission(true) == 0;
}
 
 
bool FT6x36_t4::touchPoint(uint16_t &x, uint16_t &y, uint8_t n)
{
	if (!_valid_data) return false;
	uint8_t touches = _regs[FT6X36_TD_STATUS] & 0xF;
	if (touches < 1) return false;
	if (n > (touches -1)) return false;
	x = ((_regs[coordRegStart[n]] & 0x0f) << 8) | _regs[coordRegStart[n] + 1];
	y = ((_regs[coordRegStart[n] + 2] & 0x0f) << 8) | _regs[coordRegStart[n] + 3];
    return true;
}

bool FT6x36_t4::readData() 
{
	// should start at register 0
    _pwire->beginTransmission(FT6X36_I2C_ADDRESS);
    _pwire->write(0);
    _valid_data = _pwire->endTransmission(false) == 0;
    if (!_valid_data) return false;

    _valid_data = _pwire->requestFrom(FT6X36_I2C_ADDRESS, FT6X36_REGISTERS) > 0;
    uint8_t register_number = 0;
    while(_pwire->available()) {
      _regs[register_number++] = _pwire->read();
    }
    return _valid_data;
}
  

uint8_t FT6x36_t4::gesture() 
{
	uint8_t temp = _regs[FT6X36_GEST_ID];
	switch(temp){
		case 0x00: //not rec
			return 0;
		case 0x10: //move up
			return 1;
		case 0x14: //move left
			return 2;
		case 0x18: //move down
			return 3;
		case 0x1C: //move right
			return 4;
		case 0x48: //zoom in
			return 5;
		case 0x49: //zoom out
			return 6;
	}
	return temp;
}

