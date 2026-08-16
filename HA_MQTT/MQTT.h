#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>

void initMQTT();
void mqttLoop();

void publishDiscovery();
void publishOneWireSensors();

String oneWireScanTopic();

#endif