#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>
#include "Pins.h"

extern bool relayStates[NUM_RELAYS];

extern uint8_t pwmValues[NUM_PWM];

void initOutputs();

void setRelay(uint8_t relay, bool state);

void setPWM(uint8_t channel, uint8_t value);

#endif