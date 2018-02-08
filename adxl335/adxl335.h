//www.elegoo.com
//2016.06.13

class ADXL335 {
 private:
   int xPin_, yPin_, zPin_;

 public:
   void init(int xPin, int yPin, int zPin)
   {
      pinMode(xPin_ = xPin, INPUT);
      pinMode(yPin_ = yPin, INPUT);
      pinMode(zPin_ = zPin, INPUT);
   }
   double readX() {
      return analogRead(xPin_);
   }
   double readY() {
      return analogRead(yPin_);
   }
   double readZ() {
      return analogRead(zPin_);
   }
   double readX(double &x) {
      return (x = analogRead(xPin_));
   }
   double readY(double &y) {
      return (y = analogRead(yPin_));
   }
   double readZ(double &z) {
      return (z = analogRead(zPin_));
   }
   void read(int &x, int &y, int &z) {
      x = readX();
      y = readY();
      z = readZ();
   }
   void read(double &x, double &y, double &z) {
      x = analogRead(xPin_);
      y = analogRead(yPin_);
      z = analogRead(zPin_);
   }
};
