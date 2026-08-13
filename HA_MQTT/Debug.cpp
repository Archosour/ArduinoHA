#include "Debug.h"
#include <Arduino.h>

void setupDebug()
{
    Serial.begin(115200);

    delay(10);

    LOG("System booting...");
}

String readLine()
{
    while (!Serial.available())
    {
        delay(10);
    }

    return Serial.readStringUntil('\n');
}