#include "IRremote.h"

const int IRrecvpin = 11; // IR Receiver to Arduino Pin 11

/*-----( Declare objects )-----*/
static IRrecv irrecv(IRrecvpin);     // create instance of 'irrecv'

static struct {
   unsigned long code;
   const char*   name;
} keymap[] = {
   { 0x00FFA25D, "POWER"   },
   { 0x00FFE21D, "FUNC"    },
   { 0x00FF629D, "VOL+"    },
   { 0x00FF22DD, "FAST"    },
   { 0x00FF02FD, "PAUSE"   },
   { 0x00FFC23D, "FAST"    },
   { 0x00FFE01F, "DOWN"    },
   { 0x00FFA857, "VOL-"    },
   { 0x00FF906F, "UP"      },
   { 0x00FF9867, "EQ"      },
   { 0x00FFB04F, "ST_REPT" },
   { 0x00FF6897, "0"       },
   { 0x00FF30CF, "1"       },
   { 0x00FF18E7, "2"       },
   { 0x00FF7A85, "3"       },
   { 0x00FF10EF, "4"       },
   { 0x00FF38C7, "5"       },
   { 0x00FF5AA5, "6"       },
   { 0x00FF42BD, "7"       },
   { 0x00FF4AB5, "8"       },
   { 0x00FF52AD, "9"       },
   { 0xFFFFFFFF, "REPEAT"  },
};

static const char* code2name(unsigned long code)
{
   int i;
   for (i = 0; i < sizeof(keymap)/sizeof(keymap[0]); i++) {
      if (keymap[i].code == code) {
         return keymap[i].name;
      }
   }
   Serial.print(code, HEX);
   return "unknown code";

}

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
}


void loop()
{
  decode_results results;

  if (irrecv.isIdle() && irrecv.decode(&results)) { // have we received an IR signal?

     Serial.println(code2name(results.value));
     delay(500);
     irrecv.resume(); // receive the next value
  }
}/* --(end main loop )-- */
