#ifndef ONEWIRE_SENSORS_H
#define ONEWIRE_SENSORS_H

#include <Arduino.h>

#define MAX_ONEWIRE_SENSORS 16

struct OneWireSensor
{
    byte address[8];
    byte family;
    float temperature;
    bool valid;
};

extern OneWireSensor oneWireSensors[MAX_ONEWIRE_SENSORS];
extern int oneWireSensorCount;

void initOneWire();
void discoverOneWireSensors();
void updateOneWireSensors();
bool rescanOneWire();

String oneWireAddress(int index);
bool isDS18B20(int index);

#endif