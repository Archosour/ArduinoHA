#include "Counters.h"

volatile unsigned long pulseCount[NUM_COUNTERS];

unsigned long lastPulseCount[NUM_COUNTERS];

float pulseRate[NUM_COUNTERS];
float rpmValues[NUM_COUNTERS];

// --------------------------------------------------
// Interrupt handlers
// --------------------------------------------------

void counter0ISR()
{
    pulseCount[0]++;
}

void counter1ISR()
{
    pulseCount[1]++;
}

void counter2ISR()
{
    pulseCount[2]++;
}

void counter3ISR()
{
    pulseCount[3]++;
}

void counter4ISR()
{
    pulseCount[4]++;
}

void counter5ISR()
{
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

        pulseRate[i] = delta * 1000.0f / intervalMs;

        rpmValues[i] = pulseRate[i] * 60.0f;

        lastPulseCount[i] = counts[i];
    }
}