#include <Servo.h>

/////////////////////////////////////////////////////////////////////
// program update frequency, up to 500
static const long hz = 20;

#define INCLINER_PLOT 1
#define INCLINER_RUN  1
#include "Incliner.h"

#include "TrapezoidalMotion.h"
#include "HzLoop.h"


/////////////////////////////////////////////////////////////////////
// pinout

// pin setup is for an Arduino Pro Micro, we also consume Serial1
//  which is on pins 1 and 0

// other wiring....
static const int azServoPin = 6; // aka D6, PWM output

// encoder takes 2 interrupt pins
static const int incEncoderPin1 = 2; // aka SCL
static const int incEncoderPin2 = 3;

// motor controller takes 2 digital pins
static const int incMCDirPin   = 4;
static const int incMCSpeedPin = 5;


/////////////////////////////////////////////////////////////////////
// azimuth

// read azimuth from serial, output a Servo signal via a trapezoidal
//    motion smoother


// azimuth servo maxV, from spec sheet, is 0.13secs/60 degrees
//   seconds/60 degrees
//     ** divide by 60 gets us
//   seconds/degree
//     ** invert gets us
//   degrees/second
//     ** divide by hz gets us
//   degress/cycle
//
static const double azMaxV = (1/(0.13/60))/hz;
// azimuthServo acceleration, higher value=jerkier response, test and tune
//   with full system, do not start higher than V/10
static const double azMaxA = azMaxV/50;

static TrapezoidalMotion azMotion(0.0, azMaxV, azMaxA);
static Servo azServo;

static const double azMax = 60.0;
static const double azMin = -60.0;



// this class could be generalized to a line-reading scanf(), maybe templated
class PiConn {
private:
   char   buf_[32];
   int    idx_;
   int    age_;
   double azimuth_;
   double inclination_;

public:
   PiConn(double azimuth, double inclination, int age) {
      idx_ = 0;
      age_ = age;
      azimuth_ = azimuth;
      inclination_ = inclination;
   }

   int read(double& azimuth, double& inclination, unsigned long deadline) {
      int input;

      age_++;

      while ((input = Serial1.read()) > 0 && millis() < deadline) {
         if ('\r' == input || '\n' == input) {
            buf_[idx_] = '\0';
            idx_ = 0;

            Serial.println(buf_);

            if (*buf_ == 'a') {
               azimuth_ = atof(buf_+1);
               age_ = 0;
               break;
            }
            if (*buf_ == 'i') {
               inclination_ = atof(buf_+1);
               age_ = 0;
               break;
            }

         } else {
            buf_[idx_++] = input;
            idx_ %= sizeof(buf_);
         }
      }
      azimuth = azimuth_;
      inclination = inclination_;

      return age_;
   }
};

#define PLOT 1
class Plotter {
public:
   void plot(double x) {
      if (PLOT) { Serial.print(x); Serial.print(" "); }
   }

   ~Plotter() {
      if (PLOT) Serial.println();
   }
};

void setup(void) {

   Serial.begin(115200);
   Serial1.begin(9600);

   azServo.attach(azServoPin);
   azServo.write(90);

}

void loop(void) {
   HzLoop hzloop(hz); // automatic, dtor does delay(xx)
   Plotter p; // automatic variable, destructor does Serial.println()

   static PiConn piconn(0, 0, hz/2);
   static Incliner incliner(incEncoderPin1, incEncoderPin2,
                            incMCDirPin, incMCSpeedPin);

   double azimuth = 0;
   double inclination = 0;

   // see if the Pi has some azimuth and/or data for me
   int age = piconn.read(azimuth, inclination, hzloop.deadline());

   if (age > hz) {
      azimuth = 0;
      inclination = 0;
   }

   p.plot(azimuth);
   p.plot(inclination);

   // clamp azimuth and inclination to hardware limits
   azimuth = constrain(azimuth, azMin, azMax);

   azimuth = azMotion.next(azimuth);

   azServo.write(90+azimuth);

   incliner.write(inclination);
}
