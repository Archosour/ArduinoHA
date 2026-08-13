#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "Pins.h"

extern int adcValues[NUM_ANALOG];
extern float temperatures[NUM_ANALOG];
extern bool inputStates[NUM_INPUTS];
extern bool inputChanged[NUM_INPUTS];

void initDigitalInputs();
void updateSensors();
void updateInputs();

float adcToTemperature(int adc);

#endif