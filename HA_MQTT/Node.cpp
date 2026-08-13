#include "Node.h"
#include "Debug.h"
#include <ArduinoUniqueID.h>

String nodeId;
String statusTopic;
byte mac[6];

void buildNodeId()
{
  nodeId = "MEGA_";

  for (size_t i = 0; i < UniqueIDsize; i++)
  {
    char buf[3];
    sprintf(buf, "%02X", UniqueID[i]);
    nodeId += buf;
  }

  statusTopic = "home/" + nodeId + "/status";

  LOGP("Node ID: ");
  LOG(nodeId);
}


void buildMac()
{
    mac[0] = 0x02;  // Locally administered MAC

    mac[0] = 0x02;
    mac[1] = UniqueID[0];
    mac[2] = UniqueID[2];
    mac[3] = UniqueID[4];
    mac[4] = UniqueID[6];
    mac[5] = UniqueID[8];

    LOGP("MAC: ");

    for (int i = 0; i < 6; i++)
    {
        LOGP(mac[i], HEX);
        LOGP(":");
    }

    LOG();
}

String stateTopic(const String& type, int index)
{
    return "home/" + nodeId + "/" + type + String(index) + "/state";
}

String commandTopic(const String& type, int index)
{
    return "home/" + nodeId + "/" + type + String(index) + "/set";
}