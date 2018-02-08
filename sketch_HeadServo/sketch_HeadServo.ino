#include <Servo.h>

static const int headPin = 2;
static const int yAccelPin = A0;

static Servo head;
static long yAccelMin;
static long yAccelMax;
static long yAccelSensitivity;

#define SERVO_ZERO  800 // in uSec, 1000us PWM is normally 90deg
#define SERVO_RANGE 1400 // in uSec, 2000us PWM is 270deg

#define AN_AVG_READS 10 // 1 means "not averaging"

class Smoother {
private:
   double alpha_;
   double last_;

public:
   void Setup(double alpha, long initial)
   {
      alpha_ = alpha;
      last_  = initial;
   }
   void Force(long val)
   {
      last_ = val;
   }
   long Next(long value)
   {
      last_ = (value * (1 - alpha_)) + (last_ * alpha_);
      return (long)last_;
   }
};

class AnAvg {
private:
   long total_;
   long readings_[AN_AVG_READS];
   int  idx_;
   int  pin_;
   int  scale_;

public:
   int getScale()
   {
      return scale_;
   }

   long Read()
   {
      // subtract the last reading:
      total_ -= readings_[idx_];
      // read from the sensor:
      readings_[idx_] = (long)analogRead(pin_) << scale_;
      // add the reading to the total:
      total_ += readings_[idx_];
      // advance to the next position in the array:
      idx_++;

      // if we're at the end of the array...
      if (idx_ >= sizeof(readings_)/sizeof(readings_[0])) {
         // ...wrap around to the beginning:
         idx_ = 0;
      }

      // calculate the average:
      return total_/(sizeof(readings_)/sizeof(readings_[0]));
   }

   void Setup(int pin, int initDelay, int scale)
   {
      pinMode(pin, INPUT);

      pin_   = pin;
      idx_   = 0;
      scale_ = scale;

      do {
         Read();
         delay(initDelay);
      } while (idx_);
   }
};

static AnAvg    yAccelAvg;
static Smoother headAngle;
static Smoother accelMax;
static Smoother accelMin;

void setup()
{
   head.attach(headPin);
   Serial.begin(115200); // works better at 9600 in TinkerCAD

   yAccelAvg.Setup(yAccelPin, 1, 8);
   yAccelMin = yAccelMax = yAccelAvg.Read();

   headAngle.Setup(0.2, SERVO_RANGE/2);

   accelMax.Setup(0.9999, yAccelMax);
   accelMin.Setup(0.9999, yAccelMin);
}

void loop()
{
   static int nth = 0;
   long angle  = 0;
   long yAccel = yAccelAvg.Read();

   if (yAccel < yAccelMin) {
      yAccelMin = yAccel;
      accelMin.Force(yAccelMin);
   } else if (yAccel > yAccelMax) {
      yAccelMax = yAccel;
      accelMax.Force(yAccelMax);
   } else {
      yAccelMax = accelMax.Next(yAccel+yAccelSensitivity);
      yAccelMin = accelMin.Next(yAccel-yAccelSensitivity);
   }

   yAccelSensitivity = (yAccelMax - yAccelMin)/(10<<yAccelAvg.getScale());

   angle = headAngle.Next((SERVO_RANGE * (yAccel-yAccelMin))/(yAccelMax-yAccelMin));
   if (yAccelSensitivity) {
      // only move the head of centerif we have at least 10 possible spots
      head.writeMicroseconds(SERVO_ZERO+angle);
   } else {
      head.writeMicroseconds(SERVO_ZERO+(SERVO_RANGE/2));
   }

   if (nth%1024 == 0) {
      if (nth%16384 == 0) {
         Serial.println("head\taccel\tmin\tmax\tsens");
         Serial.println("==================================");
      }
      Serial.print(angle, DEC);
      Serial.print("\t");
      Serial.print(yAccel, DEC);
      Serial.print("\t");
      Serial.print(yAccelMin, DEC);
      Serial.print("\t");
      Serial.print(yAccelMax, DEC);
      Serial.print("\t");
      Serial.print(yAccelSensitivity, DEC);
      Serial.println("");
   }

   nth++;
}
