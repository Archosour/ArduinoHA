#include "Counters.h"

volatile unsigned long pulseCount[NUM_COUNTERS];

unsigned long lastPulseCount[NUM_COUNTERS];

volatile unsigned long lastPulseMicros[NUM_COUNTERS];
volatile unsigned long pulsePeriodMicros[NUM_COUNTERS];

float pulseRate[NUM_COUNTERS];
float rpmValues[NUM_COUNTERS];

// --------------------------------------------------
// Interrupt handlers
// --------------------------------------------------

void counter0ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[0] = now - lastPulseMicros[0];

    lastPulseMicros[0] = now;

    pulseCount[0]++;
}

void counter1ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[1] = now - lastPulseMicros[1];

    lastPulseMicros[1] = now;

    pulseCount[1]++;
}

void counter2ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[2] = now - lastPulseMicros[2];

    lastPulseMicros[2] = now;

    pulseCount[2]++;
}

void counter3ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[3] = now - lastPulseMicros[3];

    lastPulseMicros[3] = now;

    pulseCount[3]++;
}

void counter4ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[4] = now - lastPulseMicros[4];

    lastPulseMicros[4] = now;

    pulseCount[4]++;
}

void counter5ISR()
{
    unsigned long now = micros();

    pulsePeriodMicros[5] = now - lastPulseMicros[5];

    lastPulseMicros[5] = now;

    pulseCount[5]++;
}

// --------------------------------------------------

void initCounters()
{
    for (int i = 0; i < NUM_COUNTERS; i++)
    {
      pinMode(counterPins[i], INPUT_PULLUP);

        pulseCount[i] = 0;
        lastPulseCount[i] = 0;
        pulseRate[i] = 0;
    }

    attachInterrupt(
        digitalPinToInterrupt(counterPins[0]),
        counter0ISR,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(counterPins[1]),
        counter1ISR,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(counterPins[2]),
        counter2ISR,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(counterPins[3]),
        counter3ISR,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(counterPins[4]),
        counter4ISR,
        FALLING
    );

    attachInterrupt(
        digitalPinToInterrupt(counterPins[5]),
        counter5ISR,
        FALLING
    );
}

// --------------------------------------------------

void updateCounters(unsigned long intervalMs)
{
    noInterrupts();

    unsigned long counts[NUM_COUNTERS];

    for (int i = 0; i < NUM_COUNTERS; i++)
    {
        counts[i] = pulseCount[i];
    }

    interrupts();

    for (int i = 0; i < NUM_COUNTERS; i++)
    {
        unsigned long delta = counts[i] - lastPulseCount[i];

        if (delta >= 50)
        {
            pulseRate[i] = delta * 1000.0 / intervalMs;
        }
        else
        {
            pulseRate[i] = 1000000.0 / pulsePeriodMicros[i];
        }

        rpmValues[i] = pulseRate[i] * 60.0f;

        lastPulseCount[i] = counts[i];
    }
}