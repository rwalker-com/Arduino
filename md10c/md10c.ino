#include "md10c.h"

static MD10C motor(4,/* aka D4, direction */
                   3 /* aka D3, pwm */);

static void setup(void)
{
   Serial.begin(115200);
}


static void loop(void)
{
   static int speed = 0;
   static int increment = 1;

   motor.run(speed += increment);

   if (speed >= 255 || speed <= -255) {
      increment = -increment;
   }

   Serial.print(speed); Serial.print(" ");
   Serial.println();

   delay(100);
}
