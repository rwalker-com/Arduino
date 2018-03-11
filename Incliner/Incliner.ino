#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>
#include "md10c.h"


#define clamp(x, min, max) (((x)<(min))?(min):((x)>(max))?(max):(x))

class Incliner {
   Encoder* encoder_;
   MD10C*   motor_;
   double   pulsesPerDegree_;

public:
   Incliner(Encoder* encoder, MD10C* motor,
            double pulsesPerDegree){
      motor_ = motor;
      encoder_ = encoder;
      pulsesPerDegree_ = pulsesPerDegree;
   }

   void write(double angle) {
      angle *= pulsesPerDegree_;

      int32_t enc = encoder_->read();
      int32_t run = ((int32_t)angle) - enc;

      Serial.print(enc); Serial.print(" ");
      Serial.print(angle); Serial.print(" ");

      run = clamp(run, -255, 255);
      if (abs(run) < 20) run = 0;

      Serial.print(run); Serial.print(" ");

      motor_->run((int)run);
   }

};


void setup(void)
{
   Serial.begin(115200);
}


void loop(void)
{
   static Encoder e(2, 3);
   static MD10C m(5, 6);

   static Incliner incliner(&e, &m, 6313/19);

   static int count = 0;
   static long target = 0;

   if (count++ > 50) { /* change location once a second */
      count = 0;
      target = random(-19, 19);
   }

   double ftarget = target;
   Serial.print(ftarget); Serial.print(" ");

   incliner.write(ftarget);

   Serial.println();
   delay(20);
}
