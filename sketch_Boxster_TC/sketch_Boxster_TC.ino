// 
// Written to run on an attiny85
// 
//  
// 
static const int tc_off_signal = PB1;
static const int tc_off_indicator = A1;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(tc_off_signal, OUTPUT);
  pinMode(tc_off_indicator, INPUT_PULLUP);
}

// the loop function runs over and over again forever
void loop() {
  // only check once every second
  delay(1000);
  // if indicator is low, tc is already off
  //  using analog because the ABS computer offers a not-quite-ground signal
  if (analogRead(tc_off_indicator) < 128) {
     return;
  }
  // else push the button for a half second
  digitalWrite(tc_off_signal, HIGH);
  delay(500);
  digitalWrite(tc_off_signal, LOW);

}
