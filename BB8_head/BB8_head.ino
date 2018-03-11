#include <Servo.h>
#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include "TrapezoidalMotion.h"
#include "md10c.h"

/////////////////////////////////////////////////////////////////////
// pinout

// pin setup is for an Arduino Pro Micro, we also consume Serial1
//  which is on pins 1 and 0

// zero the servo
static const int zeroPin = 6; // aka D6, digitalRead()

// other wiring....
static const int azServoPin = 9; // aka D9, PWM output

// encoder takes 2 interrupt pins
static const int incEncoderPin1 = 3; // aka SCL
static const int incEncoderPin2 = 7;

// motor controller takes 2 digital pins
static const int incMCDirPin   = 4;
static const int incMCSpeedPin = 5;

/////////////////////////////////////////////////////////////////////
// program update frequency, up to 500
static const long hz = 20;

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
// azimuthServo acceleration, higher value=jerkier response, test with full system
static const double azMaxA = azMaxV/50;

static TrapezoidalMotion azMotion(0.0, maxV, maxA);
static Servo azServo;

static const double azMax = 60.0;
static const double azMin = -60.0;

/////////////////////////////////////////////////////////////////////
// inclination


// read inclination from serial, map to a motor controller/encoder pair
//   that comprise a linear actuator

// motor max speed is 1621RPM, screw is 8mm/revolution, we're defining
//  the center of the actuator as "zero"


// the screw is 8mm/revolution, runs in a straight line, but the trolley
//   follows an arc, so degrees are not exactly one-to-one
// the motor ratio is 5.182:1, 12 cycles per motor revolution, so
//   62.184 cycles/revolution of the screw
//
// max velocity:
//  events/rev * revs/min * min/sec
//
//   248.736   *  1621    * 1/60     = 6270.0176 events/sec
//
//
//  mm/rev     * revs/min * min/sec
//
//    8        *  1621    * 1/60     = 216 mm/s, or 8.5 in/s
//
//  maximum head deflection = 8inches or 19deg deflection each way
//
//   in  *  mm/in  * revs/mm  * events/rev
//
//    8  *  25.4   *   1/8    * 248.736   = 6317.8944 count for max deflection
//
//

static const double incEncoderCyclesPerDegree = 6312.8944/19;

// inclination motor controller
static MD10C   incMC(incMCDirPin, incMCSpeedPin);
static Encoder incEncoder(incEncoderPin1, incEncoderPin2);

static const double incMax = 19.0;
static const double incMin = -19.0;

static const int32_t incMaxCycles = 1579;
static const int32_t incMinCycles = 1579;


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

static PiConn piconn(0,0,hz/2);


void setup() {

   Serial.begin(115200);
   Serial1.begin(9600);

   azServo.attach(azServoPin);
   azServo.write(90);


   pinMode(zeroPin, INPUT_PULLUP);

}

#define clamp(x, min, max) (((x)<(min))?(min):((x)>(max))?(max):(x))

void loop() {
   unsigned long start = millis();
   unsigned long deadline = start + 1000/hz;

   double azimuth = 0;
   double inclination = 0;

   // see if the Pi has some azimuth data for me
   /*int age = */ piconn.read(azimuth, inclination, deadline);

   Serial.print(azimuth); Serial.print(" ");
   Serial.print(inclination); Serial.print(" ");

   if (LOW == digitalRead(zeroPin)) {
      azimuth = inclination = 0;
   }

   // clamp azimuth and inclination to hardware limits
   azimuth = clamp(azimuth, azMin, azMax);
   inclination = clamp(inclination, incMin, incMax);

   azimuth = azMotion.next(azimuth);
   azServo.write(90+azimuth);

   {
      int32_t pos = incEncoder.read();


      azTilter.write(inclination);
   }


   Serial.print(azimuth); Serial.print(" ");
   Serial.print(inclination); Serial.print(" ");

   Serial.println();

   unsigned long now = millis();
   if (now < deadline) {
      delay(deadline - now);
   }
}
