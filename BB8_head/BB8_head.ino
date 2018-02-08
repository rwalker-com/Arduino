#include <Servo.h>
#include "MechaQMC5883.h"
#include "TrapezoidalMotion.h"


// Assumption: magnet's North/South poles are aligned with the sensor's
// X axis, with North pole being *negative* X
static MechaQMC5883 magnetometer;
// our reader, on I2C pins

// zero the servo
static const int zeroPin = 6; // aka D6

// wiring implies
static const int headPin = 9; // aka D9
static Servo head;
static TrapezoidalMotion headAngle;

static const double headMax = 60.0;
static const double headMin = -60.0;

// update frequency, up to 500
static const long hz = 20;

// head servo maxV, from spec sheet, is 0.13secs/60 degrees
//   seconds/60 degrees
//     ** divide by 60 gets us
//   seconds/degree
//     ** invert gets us
//   degrees/second
//     ** divide by hz gets us
//   degress/cycle
//
static const double maxV = (1/(0.13/60))/hz;
// head acceleration, higher value, jerkier response, test with full system
static const double maxA = maxV/50;

class PiConn {
private:
   char   buf_[32];
   int    idx_;
   int    age_;
   double angle_;

public:
   void init(double angle, int age) {
      idx_ = 0;
      age_ = age;
      Serial1.begin(9600);
   }

   int read(double& angle, unsigned long deadline) {
      int input;

      age_++;

      while ((input = Serial1.read()) > 0 && millis() < deadline) {
         if ('\r' == input || '\n' == input) {
            buf_[idx_] = '\0';
            idx_ = 0;

            if (*buf_ == 'a') {
               angle_ = atof(buf_+1);
               age_ = 0;
               break;
            }

         } else {
            buf_[idx_++] = input;
            idx_ %= sizeof(buf_);
         }
      }
      angle = angle_;
      return age_;
   }
};

static PiConn piconn;

void setup() {
   Wire.begin(); // annoyed that I have to reach around my I2C objects

   Serial.begin(115200);

   magnetometer.init(); // default settings are continuous reads, max range
   piconn.init(0, hz/2); // initialize with zero and "old"

   head.attach(headPin);
   head.write(90);

   headAngle.init(0.0, maxV, maxA);

   pinMode(zeroPin, INPUT_PULLUP);

}

void loop() {
   unsigned long start = millis();
   unsigned long deadline = start + 1000/hz;

   int16_t x, y, z; //triple axis data
   magnetometer.read(&x, &y, &z);
   //   Serial.print(x/100); Serial.print(" ");
   //   Serial.print(y/100); Serial.print(" ");
   //   Serial.print(z/100); Serial.print(" ");


   // arctan and convert radians to degrees
   double magnet = atan2(y, x) * 180/M_PI;
   Serial.print(magnet); Serial.print(" ");

   double piangle = 0;

   // see if the Pi has some angle data for me
   int piage = piconn.read(piangle, deadline);

   Serial.print(piangle); Serial.print(" ");

   double target =
      (LOW == digitalRead(zeroPin)) ? 0 :
      (piage < hz/2) ? piangle : magnet;

   // clamp servo input
   if (target > headMax) target = headMax;
   if (target < headMin) target = headMin;

   double angle = headAngle.next(target);

   head.write(90+angle);

   Serial.print(angle); Serial.print(" ");
   Serial.println();

   unsigned long now = millis();
   if (now < deadline) {
      delay(deadline - now);
   }
}
