#include <SPI.h>
#include <Ethernet.h>
#include "Network.h"
#include <PubSubClient.h>
#include <ArduinoUniqueID.h>
#include <math.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "Config.h"
#include "Node.h"
#include "Pins.h"
#include "Sensors.h"
#include "Outputs.h"
#include "MQTT.h"

void setup()
{
  Serial.begin(115200);

  if (!loadConfig())
  {
    Serial.println("No valid config found.");
    configureNode();
  }

  Serial.println();
  Serial.println("Press C within 5 seconds for configuration, disable line endings!");

  unsigned long startTime = millis();

  while (millis() - startTime < 5000)
  {
      if (Serial.available())
      {
          char c = Serial.read();

          if (c == 'c' || c == 'C')
          {
              configureNode();
          }
      }
  }

  buildNodeId();
  buildMac();

  initNetwork();
  initMQTT();

  initOutputs();
  initDigitalInputs();

}

void loop()
{
  Ethernet.maintain();
  updateInputs();

  mqttLoop();
}