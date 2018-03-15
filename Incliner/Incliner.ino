// Copyright rwalker@rwalker.com 2018
//

static const int hz = 20;

#define INCLINER_PLOT 1
#include "Incliner.h"

#include "HzLoop.h"

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


void setup(void)
{
   Serial.begin(115200);
}

void loop(void)
{
   HzLoop hzloop(hz);
   Plotter p; // automatic variable, destructor does Serial.println()
   static Incliner incliner(2, 3,   // encoder
                            5, 6);  // motor

   static int count = 0;
   static long target = 0;

   if (count++ > 30) { /* change location once every 600ms */
      count = 0;
      target = random(-19, 19);
   }

   double ftarget = target;
   p.plot(ftarget);

   incliner.write(ftarget);

}
