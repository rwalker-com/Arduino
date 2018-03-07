#ifndef Mecha_QMC5883
#define Mecha_QMC5883

#include <Arduino.h>
#include <Wire.h>

#define QMC5883_ADDR 0x0D

#define REGISTER_Mode        0x09

#define MODE_Standby    0b00000000
#define MODE_Continuous 0b00000001

#define ODR_10Hz        0b00000000
#define ODR_50Hz        0b00000100
#define ODR_100Hz       0b00001000
#define ODR_200Hz       0b00001100

#define RNG_2G          0b00000000
#define RNG_8G          0b00010000

#define OSR_512         0b00000000
#define OSR_256         0b01000000
#define OSR_128         0b10000000
#define OSR_64          0b11000000


class MechaQMC5883 {
 public:

   // ignore-able if you haven't moved it
   void setAddress(uint8_t addr);

   // init qmc5883, defaults to
   // OSR = 512
   // Full Scale Range = 8G(Gauss)
   // ODR = 200HZ
   // i.e. continuous measurement mode
   void init();

   // custom settings
   void setMode(uint16_t mode,uint16_t odr,uint16_t rng,uint16_t osr);

   void softReset(); //soft RESET

   void read(int16_t* x,int16_t* y,int16_t* z); //reading

 private:

   void WriteReg(uint8_t Reg,uint8_t val);

   uint8_t addr_ = QMC5883_ADDR;

};



#endif
