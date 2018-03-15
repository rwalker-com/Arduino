// copyright 2018 rwalker@rwalker.com
//
#ifndef _HZLOOP_H
#define _HZLOOP_H
//
//  HzLoop() forces the instantiation context (usually your loop() function)
//   to run for 1000/hz milliseconds
//
//  void loop(void) {
//     HzLoop(20); // run me at 20 hz
//      ... work ...
//  }
//
class HzLoop {
 private:
   unsigned long deadline_;

 public:
   HzLoop(int hz) {
      deadline_ = millis() + 1000/hz;
   }
   ~HzLoop() {
      unsigned long now = millis();
      if (now < deadline_) {
         delay(deadline_ - now);
      }
   }
   unsigned long deadline() { return deadline_; }

};

#endif // #ifndef _HZLOOP_H
