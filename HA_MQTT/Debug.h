#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

void setupDebug();
String readLine();

#define DEBUG_ENABLED 1

#if DEBUG_ENABLED

#define LOG(...) Serial.println(__VA_ARGS__)
#define LOGP(...) Serial.print(__VA_ARGS__)

#else

  #define LOG(...)
  #define LOGP(...)

#endif

#endif