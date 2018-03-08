#include <Servo.h>
#include <Encoder.h>
#include "TrapezoidalMotion.h"
#include "md10c.h"

// pin setup is for an Arduino Pro Micro, we also consume Serial2
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


// azimuth
static Servo azServo;
static TrapezoidalMotion azMotion;

static const double azServoMax = 60.0;
static const double azServoMin = -60.0;

// inclination motor controller
static MD10C   incMC(incMCDirPin, incMCSpeedPin);
static Encoder incEncoder(incEncoderPin1, incEncoderPin2);

static const double incMoverMax = 45.0;
static const double incMoverMin = -45.0;



// update frequency, up to 500
static const long hz = 20;

// azimuth servo maxV, from spec sheet, is 0.13secs/60 degrees
//   seconds/60 degrees
//     ** divide by 60 gets us
//   seconds/degree
//     ** invert gets us
//   degrees/second
//     ** divide by hz gets us
//   degress/cycle
//
static const double maxV = (1/(0.13/60))/hz;
// azimuthServo acceleration, higher value, jerkier response, test with full system
static const double maxA = maxV/50;

class PiConn {
private:
   char   buf_[32];
   int    idx_;
   int    age_;
   double azimuth_;
   double inclination_;

public:
   void init(double azimuth, double inclination, int age) {
      idx_ = 0;
      age_ = age;
      azimuth_ = azimuth;
      inclination_ = inclination;
      Serial1.begin(9600);
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

static PiConn piconn;



void setup() {

   Serial.begin(115200);

   piconn.init(0, 0, hz/2); // initialize with zero and "old"

   azServo.attach(azServoPin);
   azServo.write(90);

   azMotion.init(0.0, maxV, maxA);

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
   azimuth = clamp(azimuth, azServoMin, azServoMax);
   inclination = clamp(inclination, incMoverMin, incMoverMax);

   azimuth = azMotion.next(azimuth);

   azServo.write(90+azimuth);

   Serial.print(azimuth); Serial.print(" ");
   Serial.print(inclination); Serial.print(" ");

   Serial.println();

   unsigned long now = millis();
   if (now < deadline) {
      delay(deadline - now);
   }
}
