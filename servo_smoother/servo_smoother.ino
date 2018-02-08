

class ServoSmoother {
   //  initial: initial position of servo in radians
   //  maxSpeed: speed of servo (time in seconds it takes to turn 60 degrees)
   //  hz: the number of times per second you'll be updating the servo,
   //        or 1/loop delay
   void init(double initial, double maxSpeed, int hz) {
      last_ = initial;

      // fix our units ;)
      // maxSpeed is seconds per PI/3 radians (s/pi/3)
      maxV_ = 1/maxSpeed; // maxV_ is PI/3 radians/sec (pi/3/s)
      maxV_ *= 3/PI;      // maxV_ is now radians/sec (1/s or Hz)
      maxV_ /= hz;        // maxV_ is now radians/sample
      hz_    = hz;
   }

   // initialize with
   //  initial: initial position of servo in degrees
   //  maxSpeed: speed of servo (time in seconds it takes to turn 60 degrees)
   //  hz: the number of times per second you'll be updating the servo,
   //        or 1/loop delay
   void init(int initial, int maxSpeed, int hz) {
      init(initial, maxSpeed*(PI/180), hz);
   }

   // next angle to give to servo in radians, given target radians
   double next(double target) {
      double error  = target - last_;
      double errorV = error / hz_; // might be negative

      // never produce an error larger than maxV
      if (abs(errorV) > maxV_) {
         errorV = (errorV < 0) ? -maxV_ : maxV_;
      }

//      double next = errorV*hz + last_; // next() value, unsmoothed,
//                but maxed

      // lastV_ and errorV are the same sign
      if (errorV > 0 && lastV_ > 0) {

         errorV += (errorV - lastV_)/2; // handles acceleration and decel...

      } else if (errorV < 0 && lastV_ < 0) {

         errorV += (errorV - lastV_)/2;

      } else {
         // lastV and errorV have different signs
         //    need to decelerate

      }
      //    need to accelerate
      //    need to decelerate
      //    at max

   }

   // next angle to give to servo in degrees, given target degrees
   int next(int target) {
      return next(target*(PI/180));
   }

private:
   double lastV_; // last speed in radian/sec
   double last_;  // last return value of next()

   double maxV_;  // maximum change of position per sample, absolute


};
