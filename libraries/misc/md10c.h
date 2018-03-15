// Copyright rwalker@rwalker.com 2018
#ifndef _MD10C_H
#define _MD10C_H
//
//  only understands direction and speed mode
//
//  legal speeds are in the range -255 (full "reverse") to 255 (full forward)
//

class MD10C {
 private:
   // TODO: PWM-only mode

   int dirPin_;
   int pwmPin_;

 public:
   MD10C(/*int mode,*/int dirPin, int pwmPin) {
      dirPin_ = dirPin;
      pwmPin_ = pwmPin;
      pinMode(pwmPin_, OUTPUT);
      pinMode(dirPin_, OUTPUT);
   }
   void run(int speed) {
      constrain(speed, -255, 255);

      digitalWrite(dirPin_, speed >= 0 ? HIGH : LOW);

      analogWrite(pwmPin_, speed >= 0 ? speed : -speed);
   }

};
#endif //#ifndef _MD10C_H
