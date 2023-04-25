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
   int minPWM_;

 public:
   MD10C(/*int mode,*/int dirPin, int pwmPin, int minPWM) {
      dirPin_ = dirPin;
      pwmPin_ = pwmPin;
      minPWM_ = minPWM;
      pinMode(pwmPin_, OUTPUT);
      pinMode(dirPin_, OUTPUT);
   }

   MD10C(/*int mode,*/int dirPin, int pwmPin) {
      MD10C(dirPin, pwmPin, 0);
   }

   void run(double pwm) {

      pwm = constrain(pwm, -255.0, 255.0);

      digitalWrite(dirPin_, pwm >= 0 ? HIGH : LOW);

      pwm = abs(pwm);

      if (pwm < minPWM_) { pwm = 0; }
      analogWrite(pwmPin_, pwm);
   }

};
#endif //#ifndef _MD10C_H
