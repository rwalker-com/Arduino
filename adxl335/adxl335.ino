//www.elegoo.com
//2016.06.13

#include "adxl335.h"

static ADXL335 accel;

static void setup()
{
   accel.init(A0, A1, A2);

   Serial.begin(115200);
}

static void _loop()
{
  int x, y, z;

  accel.read(x, y, z);

  Serial.print("x:  "); Serial.print(x); Serial.print(" ");
  Serial.print("y:  "); Serial.print(y); Serial.print(" ");
  Serial.print("z:  "); Serial.print(z); Serial.print(" ");

  Serial.println();
}

#include "HzLoop.h"

// loop/update frequency, note that delay() is in milliseconds, is gran.u.lar.
static const long hz = 20;

HzLoop(hz, _loop);
