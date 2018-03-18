// Copyright rwalker@rwalker.com 2018
//
#ifndef _INCLINER_H
#define _INCLINER_H

#define ENCODER_USE_INTERRUPTS
#include <Encoder.h>

#include "md10c.h"
#include "TrapezoidalMotion.h"


/////////////////////////////////////////////////////////////////////
// Incliner math

// map a floating point angle to a motor controller/encoder pair
//   that comprise a linear actuator
//
// motor max speed is 1621RPM, screw is 8mm/revolution, we're defining
//  the center of the actuator as "zero"
//
// the screw is 8mm/revolution, runs in a straight line, but the trolley
//   follows an arc, so degrees are not exactly one-to-one
//
// Motor/Encoder details:
//   (from https://www.servocity.com/1621-rpm-hd-premium-planetary-gear-motor)
//
//     Speed (No Load)*	1,621 rpm
//     Gear Ratio	5.182:1
//     Cycles Per Rev (Motor Shaft)	12
//     Cycles Per Rev (Output Shaft)	62.184
//     Countable Events Per Rev (Motor Shaft)	48
//     Countable Events Per Rev (Output Shaft)	248.736 <== number we need
//
// Parameters:
//  events/rev * revs/min * min/sec
//
//   248.736   *  1621    * 1/60     = 6270.0176 events/sec
//
//  mm/rev     * revs/min * min/sec
//
//    8        *  1621    * 1/60     = 216 mm/s, or 8.5 in/s
//
//  current hardware constrains maximum head deflection to 8in, ~19deg each way
//
//   in  *  mm/in  * revs/mm  * events/rev
//
//    8  *  25.4   *   1/8    * 248.736   = 6317.8944 events to max deflection
//

class Incliner {
private:
   Encoder encoder_;
   MD10C motor_;
   TrapezoidalMotion angle_; // angle smoother

   // calculated from motor
   static const double maxV_ = 6270.0176/hz;
   static const double maxA_ = maxV_/5; // good rule of thumb

   static const double maxDeflection_ = 6317.8944;
   static const double minDeflection_ = -6317.8944;

   // measured on the hardware with an inclinometer
   static const double pulsesPerDegree_ = maxDeflection_/19;

   void plot(double x)
   {
#ifndef INCLINER_PLOT
#define INCLINER_PLOT 0
#endif
      if (INCLINER_PLOT) {  Serial.print(x); Serial.print(" "); }
   }

 public:
   Incliner(int pin1, int pin2, // encoder pins
            int dir, int pwm) :  // motor pins
      encoder_(pin1, pin2),
      motor_(dir, pwm),
      angle_(0, maxV_, maxA_) {}

   // update desired angle at hz frequency
   void write(double angle) {

      angle *= pulsesPerDegree_;

      angle = constrain(angle, minDeflection_, maxDeflection_);
      plot(angle);

      angle = angle_.next(angle);
      plot(angle);


      int32_t enc = encoder_.read();
      plot(enc);

      double run = angle - enc; // pwm signal, using error as motor speed
                                // won't work in general, does here because
                                // pulses/turn is pretty high

      plot(run);

      motor_.run(run);
   }

};

#endif _INCLINER_H
