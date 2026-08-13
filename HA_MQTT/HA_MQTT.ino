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
#include "Debug.h"
#include "Counters.h"
#include "OneWireSensors.h"

void setup()
{
  setupDebug();

  if (!loadConfig())
  {
    LOG("No valid config found.");
    configureNode();
  }

  LOG();
  LOG("Press C within 5 seconds for configuration, disable line endings!");

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

  LOG();
    LOG("Configuration entered:");

    LOG("MQTT Server: ");
    LOG(config.mqttServer);

    LOG("MQTT User: ");
    LOG(config.mqttUser);

    LOG("MQTT Password: ********");

  buildNodeId();
  buildMac();

  initNetwork();
  initMQTT();

  initOutputs();
  initDigitalInputs();
  initCounters();
  initOneWire();
}

void loop()
{
  Ethernet.maintain();
  updateInputs();

  mqttLoop();
}