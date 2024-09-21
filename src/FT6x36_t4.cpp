/************************************************************************
This is a library to handling capacitive multitouch sensors using FT6x36.
Originally written to work with some of the BuyDisplay

Some of this was derived from stuff Written by Helge Langehaug, February 2014

BSD license, all text above must be included in any redistribution
-------------------------------------------------------------------------------
*************************************************************************/

#include "FT6x36_t4.h"
#include <SPI.h>
#include <Wire.h>

volatile boolean FT6x36_t4::_touched = false;

FT6x36_t4::FT6x36_t4(uint8_t CTP_INT, uint8_t max_touch)
    : _ctpInt(CTP_INT), _max_touch(max_touch) {
}

void FT6x36_t4::isr(void) {
    _touched = true;
}

// in EXTRLN the entire ISR it's not handled by the library!
bool FT6x36_t4::begin(TwoWire *pwire, uint8_t wire_addr) {
    _pwire = pwire;
    _wire_addr = wire_addr;
    _touched = false;

    _pwire->begin();
    _pwire->setClock(400000UL); // Set I2C frequency to 400kHz
    delay(10);

    if (!writeRegister(FT6X36_DEVICE_MODE, 0))
        return false;
    setThreshold(FT6X36_DEFAULT_THRESHOLD);

    if (_ctpInt != 0xff) {
        pinMode(_ctpInt, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_ctpInt), isr, FALLING);
    }
    return true;
}

void FT6x36_t4::setThreshold(uint8_t val) {
    writeRegister(FT6X36_ID_G_THGROUP, val);
}

// in safe mode it will also disconnect interrupt!
bool FT6x36_t4::touched() {
    // Start off ignore if we have interrupt stuff
    // lets read in everything...
    if ((_ctpInt != 0xff) && !_touched)
        return false;

    if (!readData())
        return false; // read failed.

    uint8_t touch_points = _regs[FT6X36_TD_STATUS] & 0xf;

    bool fRet = ((touch_points > 0) && (touch_points <= 2));
    if (!fRet)
        _touched = false;
    return fRet;
}

bool FT6x36_t4::writeRegister(uint8_t reg, uint8_t val) {
    _pwire->beginTransmission(FT6X36_I2C_ADDRESS);
    _pwire->write(reg);
    _pwire->write(val);
    return _pwire->endTransmission(true) == 0;
}

uint8_t FT6x36_t4::readRegister(uint8_t reg) {
    _pwire->beginTransmission(FT6X36_I2C_ADDRESS);
    _pwire->write(reg);
    _pwire->endTransmission(false);
    _pwire->requestFrom(FT6X36_I2C_ADDRESS, 1);
    if (!_pwire->available())
        return 0;
    return _pwire->read();
}

typedef struct {
    uint8_t reg;
    uint8_t hl_mask; // size of value with H and L
    const __FlashStringHelper *reg_name;
} id_to_name_t;

static const id_to_name_t reg_name_table[] PROGMEM = {
    {0x00, 0, F("DEV_MODE")},
    {0x01, 0, F("GEST_ID")},
    {0x02, 0, F("TD_STATUS")},
    {0x03, 0, F("P1_XH")},
    {0x04, 0xf, F("P1_XL")},
    {0x05, 0, F("P1_YH")},
    {0x06, 0xf, F("P1_YL")},
    {0x07, 0, F("P1_WEIGHT")},
    {0x08, 0, F("P1_MISC")},
    {0x09, 0, F("P2_XH")},
    {0x0A, 0xf, F("P2_XL")},
    {0x0B, 0, F("P2_YH")},
    {0x0C, 0xf, F("P2_YL")},
    {0x0D, 0, F("P2_WEIGHT")},
    {0x0E, 0, F("P2_MISC")},
    {0x80, 0, F("TH_GROUP")},
    {0x85, 0, F("TH_DIFF")},
    {0x86, 0, F("CTRL")},
    {0x87, 0, F("TIMEENTER MONITOR")},
    {0x88, 0, F("PERIODACTIVE")},
    {0x89, 0, F("PERIODMONITOR")},
    {0x91, 0, F("RADIAN_VALUE")},
    {0x92, 0, F("OFFSET_LEFT_RIGHT")},
    {0x93, 0, F("OFFSET_UP_DOWN")},
    {0x94, 0, F("DISTANCE_LEFT_RIGHT")},
    {0x95, 0, F("DISTANCE_UP_DOWN")},
    {0x96, 0, F("DISTANCE_ZOOM")},
    {0xA1, 0, F("LIB_VER_H")},
    {0xA2, 0xff, F("LIB_VER_L")},
    {0xA3, 0, F("CIPHER")},
    {0xA4, 0, F("G_MODE")},
    {0xA5, 0, F("PWR_MODE")},
    {0xA6, 0, F("FIRMID")},
    {0xA8, 0, F("FOCALTECH_ID")},
    {0xAF, 0, F("RELEASE_CODE_ID")},
    {0xBC, 0, F("STATE")}};

void FT6x36_t4::showAllRegisters() {
    Serial.println("\n*** FT6x36 Registers ***");
    uint8_t prev_reg_value = 0;
    for (uint16_t ii = 0; ii < (sizeof(reg_name_table) / sizeof(reg_name_table[0])); ii++) {
        uint8_t reg_value = readRegister(reg_name_table[ii].reg);
        Serial.printf("%s(%x): %u(%x)", reg_name_table[ii].reg_name,
                      reg_name_table[ii].reg, reg_value, reg_value);
        if (reg_name_table[ii].hl_mask) {
            Serial.printf(" : %u", (uint16_t)(prev_reg_value & reg_name_table[ii].hl_mask) << 8 | reg_value);
        }
        Serial.println();
        prev_reg_value = reg_value;
    }
    Serial.println("----------");
}

bool FT6x36_t4::touchPoint(uint16_t &x, uint16_t &y, uint8_t n) {
    if (!_valid_data)
        return false;
    uint8_t touches = _regs[FT6X36_TD_STATUS] & 0xF;
    if (touches < 1)
        return false;
    if (n >= touches)
        return false;
    if (n >= _max_touch)
        return false;
    x = ((_regs[coordRegStart[n]] & 0x0f) << 8) | _regs[coordRegStart[n] + 1];
    y = ((_regs[coordRegStart[n] + 2] & 0x0f) << 8) | _regs[coordRegStart[n] + 3];
    return true;
}

bool FT6x36_t4::readData() {
    // should start at register 0
    _pwire->beginTransmission(FT6X36_I2C_ADDRESS);
    _pwire->write(0);
    _valid_data = _pwire->endTransmission(false) == 0;
    if (!_valid_data)
        return false;

    _valid_data = _pwire->requestFrom(FT6X36_I2C_ADDRESS, (_max_touch == 1) ? FT6X36_REGISTERS_1 : FT6X36_REGISTERS_2) > 0;
    uint8_t register_number = 0;
    while (_pwire->available()) {
        _regs[register_number++] = _pwire->read();
    }
    return _valid_data;
}

uint8_t FT6x36_t4::gesture() {
    uint8_t temp = _regs[FT6X36_GEST_ID];
    switch (temp) {
    case 0x00: // not rec
        return 0;
    case 0x10: // move up
        return 1;
    case 0x14: // move left
        return 2;
    case 0x18: // move down
        return 3;
    case 0x1C: // move right
        return 4;
    case 0x48: // zoom in
        return 5;
    case 0x49: // zoom out
        return 6;
    }
    return temp;
}
