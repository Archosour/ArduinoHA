#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

#define NUM_ANALOG 4
#define NUM_RELAYS 8
#define NUM_INPUTS 8
#define NUM_PWM 3

extern const uint8_t analogPins[];
extern const uint8_t relayPins[];
extern const uint8_t inputPins[];
extern const uint8_t pwmPins[];

#define ETH_CS_PIN 10
#define SD_CS_PIN  4

#endif