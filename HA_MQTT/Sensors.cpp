#include "Sensors.h"
#include "Pins.h"

int adcValues[NUM_ANALOG];
float temperatures[NUM_ANALOG];
bool inputStates[NUM_INPUTS];
bool inputChanged[NUM_INPUTS];

void initDigitalInputs()
{
  for (int i = 0; i < NUM_INPUTS; i++)
  {
    pinMode(inputPins[i], INPUT_PULLUP);
  }
}

void updateSensors()
{
    for(int i = 0; i < NUM_ANALOG; i++)
    {
        adcValues[i] = analogRead(analogPins[i]);

        temperatures[i] = adcToTemperature(adcValues[i]);
    }
}

float adcToTemperature(int adc)
{
  if (adc <= 0) adc = 1;
  if (adc >= 1023) adc = 1022;

  const float SERIES_RESISTOR = 10000.0;
  const float NOMINAL_RESISTANCE = 10000.0;
  const float NOMINAL_TEMPERATURE = 25.0;
  const float B_COEFFICIENT = 3950.0;

  float resistance =
    SERIES_RESISTOR /
    ((1023.0 / adc) - 1.0);

  float steinhart;

  steinhart = resistance / NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= B_COEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void updateInputs()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        bool newState = !digitalRead(inputPins[i]);

        inputChanged[i] = false;

        if(newState != inputStates[i])
        {
            inputStates[i] = newState;
            inputChanged[i] = true;
        }
    }
}