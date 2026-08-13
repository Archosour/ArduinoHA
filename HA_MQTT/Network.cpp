#include "Network.h"
#include "Pins.h"
#include "Debug.h"

#include <Ethernet.h>

#include "Node.h"

EthernetClient ethClient;

void initNetwork()
{
    Ethernet.init(ETH_CS_PIN);

    if (Ethernet.begin(mac) == 0)
    {
        LOG("DHCP failed");
    }

    LOGP("IP: ");
    LOG(Ethernet.localIP());
}

void networkLoop()
{
    Ethernet.maintain();
}