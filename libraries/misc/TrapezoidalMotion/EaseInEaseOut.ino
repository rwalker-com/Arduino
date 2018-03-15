#include "TrapezoidalMotion.h"

static const int loopDelay = 20;
#define EQ 0
#define GE 1
#define LE 2

static void motion_next_limit(TrapezoidalMotion& motion,
                              double target,
                              double limit,
                              int check,
                              int num)
{
   double  next;
   boolean targeting;

   do {
      next = motion.next(target);
      Serial.print(target); Serial.print(" ");
      Serial.print(next); Serial.print(" ");
      Serial.println();
      delay(loopDelay);
      switch (check) {
         case EQ:
            targeting = next != limit;
            break;

         case GE:
            targeting = next < limit;
            break;

         case LE:
            targeting = next > limit;
            break;
      }
   } while (targeting && num--);
}

static TrapezoidalMotion motion;

void setup() {
   Serial.begin(115200);
   Serial.println("0 0 0");

   motion.init(0, 16, 1.0/16);

}

void loop() {

   double target = rand()%512;
   motion_next_limit(motion, target, target, EQ, rand()%16);

}
