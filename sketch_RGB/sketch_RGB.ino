#define RED 3
#define GREEN 5
#define BLUE 6

void setup() {
  // put your setup code here, to run once:
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  Serial.begin(115200);
}

#define RtoG 0
#define GtoB 1
#define BtoR 2

void loop() {
  // put your main code here, to run repeatedly:

  static int red = 255;
  static int green = 0;
  static int blue = 0;
  
  // move from R to G to B smoothly
 
  analogWrite(RED, red);
  analogWrite(BLUE, blue);
  analogWrite(GREEN, green);

  delay(8);

  if (red > 0 && blue == 0) {
    red--;
    green++;
  } else if (red == 0 && green > 0) {
    green--;
    blue++;
  } else if (green == 0 && blue > 0) {
    blue--;
    red++;
  }
 
}
