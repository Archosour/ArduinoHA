#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <Ethernet.h>

extern EthernetClient ethClient;

void initNetwork();
void networkLoop();

#endif