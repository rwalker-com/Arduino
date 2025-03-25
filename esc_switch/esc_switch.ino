/*
  Intended to run on Digispark ATTiny85 with 8Mhz clock need board and
  micronucleus uploader.

  Board Manager URL: https://raw.githubusercontent.com/ArminJo/DigistumpArduino/master/package_digistump_index.json

  Electronic Speed Controllers (ESCs) want to see a servo signal at zero when they
  first boot up otherwise they go nuts, won't work.  This program emulates a
  servo controller in that way: i.e. it shows zero for a few seconds then goes
  to full speed, more like an on-off switch when power is applied.

  Take ESC servo line (the powered one, if the ESC has 2).  Connect red to VIN, black to GND, and signal to PCINT1.  A 100uF 
  capacitor should be placed between the reset pin and GND to delay boot of the Arduino while the ESC is booting/stabilizing
  it's power supply.

*/

#include "Servo8Bit.h"

static const int start = 0;
static const int init_delay = 1000;
static const int target = 180;
static const int slowdown = 1; // min 1

Servo8Bit esc;

void setup() {
  esc.attach(PCINT1, 1000, 2000);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  static int initialized = 0;

  if (initialized) {
    esc.write(target);
  } else {

    int now = millis();

    if (now < init_delay) {
      esc.write(start);
    } else {
      int increment = (now - init_delay) / slowdown;

      if (increment < target) {
        esc.write(start + increment);
      } else {
        esc.write(target);
        initialized = 1;
      }
    }
  }
}
