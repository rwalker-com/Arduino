/*
  Intended to run on Digispark ATTiny85 with 8Mhz clock need board and
  micronucleus uploader.

  Board Manager URL:
   https://raw.githubusercontent.com/digistump/arduino-boards-index/master/package_digistump_index.json

  Electronic Speed Controllers (ESCs) want to see a servo signal at zero when they
  first boot up otherwise they go nuts, won't work.  This program emulates a
  servo controller in that way: i.e. it shows zero for a few seconds then goes
  to full speed, more like an on-off switch when power is applied.

  Take ESC servo line (the powered one, if the ESC has 2).  Connect red to VIN,
  black to GND, and signal to SIGNAL_PIN.  A 100uF capacitor should be placed
  between the reset pin and GND to delay boot of the Arduino while the ESC is
  booting/stabilizing its power supply.

*/


#define SERVO Servo8Bit // can be "Servo" for a normal Arduino board

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x
#define SERVO_H STRINGIFY(SERVO.h)
#include SERVO_H

static const int start = 0;  // esc wants to see throttle at zero while it boots
static const int init_delay = 1000;
static const int target = 135; // full-speed is 180
static const int slowdown = 10; // min 1

#define SERVO_PIN  PCINT2
#define LED_PIN    PCINT1

#define DIAG (SERVO_PIN!=LED_PIN)

SERVO esc;

typedef struct Diag {
    int _pin;

    void init(int pin) {
#if DIAG
        _pin = pin;
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
#endif
    }
    void flash(int Hz) {
#if DIAG
        if ((millis() / (1000/Hz)) & 1) {
            digitalWrite(_pin, HIGH);
        } else {
            digitalWrite(_pin, LOW);
        }
#endif
    }
    void set(int on_off) {
#if DIAG
        if (on_off) {
            digitalWrite(_pin, HIGH);
        } else {
            digitalWrite(_pin, LOW);
        }
#endif
    }
} Diag;

Diag diag;

void setup() {
    esc.attach(SERVO_PIN, 1000, 2000);  // attaches the servo on pin 9 to the servo object

    diag.init(LED_PIN);
}

void loop() {
    static int initialized = 0;

    if (initialized) {

        esc.write(target);
        diag.set(HIGH); // leave on

    } else {

        int now = millis();

        if (now < init_delay) {
            esc.write(start);
            diag.flash(5); // flash 5Hz
        } else {

            int increment = (now - init_delay) / slowdown;

            if (increment < target) {
                esc.write(start + increment);
                diag.flash(10); // flash 10Hz
            } else {
                initialized = 1;
            }
        }
    }
}
