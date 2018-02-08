class Smoother {
private:
   double smoothness_;
   double last_;

public:
   void init(double smoothness, long initial)
   {
      init(smoothness, (double)initial);
   }
   void init(double smoothness, float initial)
   {
      init(smoothness, (double)initial);
   }
   void init(double smoothness, double initial)
   {
      smoothness_ = smoothness;
      last_  = initial;
   }

   void force(long val)
   {
      force((double)val);
   }
   void force(float val)
   {
      force((double)val);
   }
   void force(double val)
   {
      last_ = val;
   }
   long next(long value)
   {
      return next((double)value);
   }
   float next(float value)
   {
      return next((double)value);
   }
   double next(double value)
   {
      last_ = (value * (1 - smoothness_)) + (last_ * smoothness_);
      return last_;
   }

};
