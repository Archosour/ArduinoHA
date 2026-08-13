#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define NUM_ANALOG 4
#define NUM_RELAYS 8
#define NUM_INPUTS 8
#define NUM_PWM 3
#define NUM_COUNTERS 6

extern const uint8_t analogPins[];
extern const uint8_t relayPins[];
extern const uint8_t inputPins[];
extern const uint8_t pwmPins[];
extern const uint8_t counterPins[];

#define ETH_CS_PIN 10
#define SD_CS_PIN  4

#define ONEWIRE_PIN 38

#endif