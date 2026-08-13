#include "Outputs.h"

bool relayStates[NUM_RELAYS];

uint8_t pwmValues[NUM_PWM];

void initOutputs()
{
    for(int i = 0; i < NUM_RELAYS; i++)
    {
        pinMode(relayPins[i], OUTPUT);

        relayStates[i] = false;

        digitalWrite(relayPins[i], LOW);
    }

    for(int i = 0; i < NUM_PWM; i++)
    {
        pinMode(pwmPins[i], OUTPUT);

        pwmValues[i] = 0;

        //analogWrite(pwmPins[i], 0);
    }
}

void setRelay(uint8_t relay, bool state)
{
    if(relay >= NUM_RELAYS)
    {
        return;
    }

    relayStates[relay] = state;

    digitalWrite(relayPins[relay], state ? HIGH : LOW);
}

void setPWM(uint8_t channel, uint8_t value)
{
    if(channel >= NUM_PWM)
    {
        return;
    }

    pwmValues[channel] = value;

    analogWrite(pwmPins[channel], value);
}