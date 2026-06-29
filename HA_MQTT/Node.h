#ifndef NODE_H
#define NODE_H

#include <Arduino.h>

extern String nodeId;
extern String statusTopic;
extern byte mac[6];

void buildNodeId();
void buildMac();

String stateTopic(const String& type, int index);
String commandTopic(const String& type, int index);

#endif