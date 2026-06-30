#ifndef COUNTERS_H
#define COUNTERS_H

#include <Arduino.h>
#include "Pins.h"

extern volatile unsigned long pulseCount[NUM_COUNTERS];

extern unsigned long lastPulseCount[NUM_COUNTERS];

extern float pulseRate[NUM_COUNTERS];
extern float rpmValues[NUM_COUNTERS];

void initCounters();

void updateCounters(unsigned long intervalMs);

#endif