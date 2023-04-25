// Copyright rwalker@rwalker.com 2018
//
#ifndef _TRAPEZOIDALMOTION_H
#define _TRAPEZOIDALMOTION_H
// class provides a Trapezoidal motion curve to a changing target
//
class TrapezoidalMotion {

public:
   // initializes the object, returns nothing
   //
   //  initial: that which would normally be returned by next(), gimme some
   //            initial state, yeah?
   //  maxV: maximum rate of change of next(), e.g. a servo's maximum
   //           speed divided by the sample frequency (not part this class)
   //  maxA: maximum rate of change of rate of change of next(), per sample,
   //           the lower the rate, the smoother the ease in, ease out
   //
   //  recommendations:
   //    * start with a low maxA, like an order of magnitude below maxV
   //    * sample this class frequently enough that maxV is at least an order
   //            of magnitude smaller than the range you'll be targeting
   //    * the higher the rate at which you sample
   //
   TrapezoidalMotion(double initial, double maxV, double maxA) {
      maxV_  = maxV;
      maxA_  = maxA;
      prev_  = initial;
      prevV_ = 0;
   }

#define SAME_SIGN(x,y) (((x)<=0&&(y)<=0)||((x)>=0&&(y)>=0))

   // returns advice about next value to use given a target
   //
   //  target: where you want to go right now
   //
   double next(double target) {

      double v = target - prev_;
      // raw velocity for this sample


      if (SAME_SIGN(v, prevV_) &&
          abs(v) < ((prevV_*prevV_)/maxA_)/2) {
         // we need to start slowing

         if (v > 0
             // && prevV_ >= 0 // unnecessary, SAME_SIGN()...
             ) {
            if (v > prevV_ - maxA_) {
               // ease out
               v = prevV_ - maxA_;
            }
            // else v is low enough, will get us over the line
         } else if (v < 0
                    // && prevV_ <= 0 // unnecessary, SAME_SIGN()...
                    ) {
            if (v < prevV_ + maxA_) {
               // ease out
               v = prevV_ + maxA_;
            }
            // else v is low enough in magnitude, will get us over the line
         }

      } else {

         if (v > 0) {
            if (v > prevV_ + maxA_) {
               // need to ease in, might be changing direction...
               v = prevV_ + maxA_;
            }
            //else {
            // no biggie
            //}

         }
         else if (v < 0) {
            if (v < prevV_ - maxA_) {
               // need to ease in, might be changing direction...
               v = prevV_ - maxA_;
            }
            //else {
            // no biggie
            //}
         }
      }

      // never produce an error larger than maxV
      if (abs(v) > maxV_) {
         v = (v < 0) ? -maxV_ : maxV_;
      }
      // for the plotter...
      //      Serial.print(v);Serial.print(" ");

      prevV_ = v;
      prev_ += v;

      return prev_;
   }

private:
   double maxV_;
   double maxA_;

   double prev_;
   double prevV_;

};

#endif //#ifndef _TRAPEZOIDALMOTION_H
