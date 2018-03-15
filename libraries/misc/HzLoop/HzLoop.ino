#include <HzLoop.h>

void setup() { Serial.begin(115200); }

static int hz = 20;
void loop()
{
   HzLoop hl(hz);
   static unsigned long laststart = 0;

   Serial.println(millis() - laststart);
   laststart = millis();

   delay(random(0,1000/hz));
}
