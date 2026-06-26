#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

extern int adcValues[];
extern float temperatures[];
extern bool inputStates[];
extern bool inputChanged[];

void setupDigitalInputs();
void updateSensors();
void updateInputs();

float adcToTemperature(int adc);

#endif