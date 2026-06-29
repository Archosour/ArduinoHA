#include "Network.h"
#include "Pins.h"

#include <Ethernet.h>

#include "Node.h"

EthernetClient ethClient;

void initNetwork()
{
    Ethernet.init(ETH_CS_PIN);

    if (Ethernet.begin(mac) == 0)
    {
        Serial.println("DHCP failed");
    }

    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
}

void networkLoop()
{
    Ethernet.maintain();
}