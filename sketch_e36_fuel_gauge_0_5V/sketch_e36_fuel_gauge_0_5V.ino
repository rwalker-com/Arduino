
//
// Written to run on an Arduino Nano
//
//  0-5V needs to be mapped to variable resistance between 50 and 500 ohms
//
//  the vaiable resistor is represented by fuel_gauge's output, sinking more current when "empty"
//     and less current when "full" by means of an output voltage that we adjust dynamically to
//     target a particular voltage (0.444V = empty, 3.33V = full)
//
namespace sender {
   static const int pin  = A1;   // 0-5V signal (0-1023) pin read 0 == empty
   static const int full = 1023; // analog 5V read of sender
   static const int empty = 4;   // analog 0.02V (1023*0.02/5) which is ~1.5 gal left in tank,
                                 //  gives us some margin 'cuz lift pumps are a bitch
}

static const int gauge   = PD3; // output signal that should be between 0.444V and 3.333V
static const int monitor = A0; // connected to fuel_gauge to auto-tune to desired voltage

static const int full = 170 + 30; // 255*(3.333333/5) = 170, 30 is fudge factor
static const int empty = 23 - 21; // 255*(0.444/5) = 23, 18 is fudge factor

void setup() {
  Serial.begin(115200);
  pinMode(sender::pin, INPUT);
  pinMode(monitor, INPUT);
  pinMode(gauge, OUTPUT);
}

int adj_target = empty;
int then = millis();
// the loop function runs over and over again forever
void loop() {

   int level = analogRead(sender::pin);
   int target = map(level, sender::empty, sender::full, empty, full);

   // read what I'm actually putting out
   int actual = map(analogRead(monitor), 0, 1023, 0, 255);
   int error = target - actual;

   adj_target = constrain(adj_target + error, 0, 255);
   analogWrite(gauge, adj_target);


   int now = millis();

   if (now%100 == 0) {

      Serial.print("elapsed = "); Serial.print(now - then, DEC);
      Serial.print(", level = "); Serial.print(level, DEC);
      Serial.print(", target = "); Serial.print(target, DEC);
      Serial.print(", adj_target = "); Serial.print(adj_target, DEC);

      Serial.print(", actual = "); Serial.print(actual, DEC);
      Serial.print(", error = "); Serial.print(error, DEC);
      Serial.println();
   }
   then = now;


}
