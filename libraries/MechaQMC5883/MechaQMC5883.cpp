#include "MechaQMC5883.h"

void MechaQMC5883::setAddress(uint8_t addr) {
  addr_ = addr;
}

void MechaQMC5883::WriteReg(byte reg, byte val) {
  Wire.beginTransmission(addr_); //start talking
  Wire.write(reg); // which register
  Wire.write(val); // set the Register
  Wire.endTransmission();
}

void MechaQMC5883::init() {
  WriteReg(0x0B,0x01);
  // Define Set/Reset period
  setMode(MODE_Continuous,ODR_200Hz,RNG_8G,OSR_512);
  // Define
  // OSR = 512
  // Full Scale Range = 8G(Gauss)
  // ODR = 200HZ
  // set continuous measurement mode
}

void MechaQMC5883::setMode(uint16_t mode, uint16_t odr,
                           uint16_t rng, uint16_t osr) {
   WriteReg(REGISTER_Mode, mode|odr|rng|osr);
}


void MechaQMC5883::softReset() {
  WriteReg(0x0A,0x80);
}

void MechaQMC5883::read(int16_t* x, int16_t* y, int16_t* z) {
  Wire.beginTransmission(addr_);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(addr_, (uint8_t)6);
  *x = (uint16_t)Wire.read(); //LSB  x
  *x |= ((uint16_t)Wire.read() << 8); //MSB  x
  *y = (uint16_t)Wire.read(); //LSB  y
  *y |= ((uint16_t)Wire.read() << 8); //MSB y
  *z = (uint16_t)Wire.read(); //LSB z
  *z |= ((uint16_t)Wire.read() << 8); //MSB z
}
