//
//  HzLoop() defines your Arduino loop() function to
//    run at a specified frequency.
//
//  Parameters:
//   __hz__: frequency. Note: delay granularity is
//            milliseconds, so __hz__ will be used to
//            divide 1000
//  __loop__: your "real" loop, contains program logic
//

#define HzLoop(__hz__, __loop__)                      \
   static void loop(void)                             \
   {                                                  \
      unsigned long start = millis();                 \
      unsigned long deadline = start + 1000/(__hz__); \
                                                      \
      (__loop__)();                                   \
                                                      \
      unsigned long now = millis();                   \
      if (now < deadline) {                           \
         delay(deadline - now);                       \
      }                                               \
   }
