#include "MQTT.h"
#include "Network.h"
#include "Node.h"
#include "Outputs.h"
#include "Config.h"
#include "Pins.h"
#include "Sensors.h"
#include "Debug.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "Counters.h"
#include "OneWireSensors.h"

IPAddress mqttServer;
PubSubClient mqtt(ethClient);

unsigned long lastPublish = 0;
const unsigned long publishInterval = 5000;




void callback(char* topic, byte* payload, unsigned int length)
{
    String msg;

    for (unsigned int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }

    String topicStr = topic;

    for (int i = 0; i < NUM_RELAYS; i++)
{
    String relayTopic =
        "home/" + nodeId + "/relay" + String(i + 1) + "/set";

    if (topicStr == relayTopic)
    {
        setRelay(i, msg == "ON");
    }
}

    for (int pwm = 0; pwm < NUM_PWM; pwm++)
    {
      String expectedTopic =
          "home/" +
          nodeId +
          "/pwm" +
          String(pwm + 1) +
          "/set";

      if (topicStr == expectedTopic)
      {
          StaticJsonDocument<128> doc;

          DeserializationError error =
              deserializeJson(doc, msg);

          if (!error)
          {
              if (doc["state"] == "OFF")
              {
                  pwmValues[pwm] = 0;
              }
              else
              {
                  pwmValues[pwm] =
                      doc["brightness"] | 255;
              }

              setPWM(pwm, pwmValues[pwm]);

              String statePayload =
                  "{\"state\":\"" +
                  String(pwmValues[pwm] > 0 ? "ON" : "OFF") +
                  "\",\"brightness\":" +
                  String(pwmValues[pwm]) +
                  "}";

              mqtt.publish(
                
                  stateTopic("pwm", pwm + 1).c_str(),
                  statePayload.c_str(),
                  true
              );

              LOGP("PWM ");
              LOGP(pwm + 1);
              LOGP(" = ");
              LOG(pwmValues[pwm]);
          }
        
      }
    }
  
}



void publishInputChanges()
{
    for(int i = 0; i < NUM_INPUTS; i++)
    {
        if(inputChanged[i])
        {
          String topic = "home/" + nodeId + "/input" + String(i + 1);

            mqtt.publish(
                topic.c_str(),
                inputStates[i] ? "ON" : "OFF",
                true
            );
        }
    }
}

void publishCounters()
{
    for (int i = 0; i < NUM_COUNTERS; i++)
    {
        mqtt.publish(
            ("home/" +
            nodeId +
            "/counter" +
            String(i + 1) +
            "/count").c_str(),
            String(pulseCount[i]).c_str(),
            true
        );

        mqtt.publish(
            ("home/" +
            nodeId +
            "/counter" +
            String(i + 1) +
            "/rate").c_str(),
            String(pulseRate[i], 2).c_str(),
            true
        );

        mqtt.publish(
            ("home/" +
            nodeId +
            "/counter" +
            String(i + 1) +
            "/rpm").c_str(),
            String(rpmValues[i], 1).c_str(),
            true
        );
    }
}

void publishSensors()
{
  updateSensors();

  for (int i = 0; i < NUM_ANALOG; i++)
  {
    mqtt.publish(
      ("home/" + nodeId +
       "/analog" + String(i + 1)).c_str(),
      String(adcValues[i]).c_str(),
      true
    );

    mqtt.publish(
      ("home/" + nodeId +
       "/temp" + String(i + 1)).c_str(),
      String(temperatures[i], 1).c_str(),
      true
    );

    LOGP("A");
    LOGP(i);

    LOGP(" ADC=");
    LOGP(adcValues[i]);

    LOGP(" Temp=");
    LOG(temperatures[i]);
  }
}

String oneWireTopic(int index)
{
    return "home/" +
           nodeId +
           "/onewire/" +
           oneWireAddress(index) +
           "/temperature";
}

void publishOneWireSensors()
{
    updateOneWireSensors();

    for (int i = 0; i < oneWireSensorCount; i++)
    {
        if (!isDS18B20(i))
        {
            continue;
        }

        if (!oneWireSensors[i].valid)
        {
            continue;
        }

        mqtt.publish(
            oneWireTopic(i).c_str(),
            String(
                oneWireSensors[i].temperature,
                2
            ).c_str(),
            true
        );

        LOGP("OneWire ");
        LOGP(oneWireAddress(i));
        LOGP(" = ");
        LOG(oneWireSensors[i].temperature);
    }
}


void publishDiscovery()
{
  for (int i = 0; i < 4; i++)
  {
    // Raw ADC

    String adcTopic =
      "homeassistant/sensor/" +
      nodeId +
      "_analog" +
      String(i + 1) +
      "/config";

    String adcPayload =
      "{"
      "\"name\":\"Analog " + String(i + 1) + "\","
      "\"unique_id\":\"" + nodeId + "_analog" + String(i + 1) + "\","
      "\"state_topic\":\"home/" + nodeId + "/analog" + String(i + 1) + "\","
      "\"state_class\":\"measurement\","
      "\"device\":{"
        "\"identifiers\":[\"" + nodeId + "\"],"
        "\"name\":\"" + nodeId + "\","
        "\"manufacturer\":\"DIY\","
        "\"model\":\"Arduino Mega W5500\""
      "}"
      "}";

    mqtt.publish(
      adcTopic.c_str(),
      adcPayload.c_str(),
      true
    );

    // Temperature

    String tempTopic =
      "homeassistant/sensor/" +
      nodeId +
      "_temp" +
      String(i + 1) +
      "/config";

    String tempPayload =
      "{"
      "\"name\":\"Temperature " + String(i + 1) + "\","
      "\"unique_id\":\"" + nodeId + "_temp" + String(i + 1) + "\","
      "\"state_topic\":\"home/" + nodeId + "/temp" + String(i + 1) + "\","
      "\"device_class\":\"temperature\","
      "\"unit_of_measurement\":\"°C\","
      "\"state_class\":\"measurement\","
      "\"device\":{"
        "\"identifiers\":[\"" + nodeId + "\"],"
        "\"name\":\"" + nodeId + "\","
        "\"manufacturer\":\"DIY\","
        "\"model\":\"Arduino Mega W5500\""
      "}"
      "}";

    mqtt.publish(
      tempTopic.c_str(),
      tempPayload.c_str(),
      true
    );
  }

  for (int i = 0; i < NUM_RELAYS; i++)
  {
    String switchTopic =
        "homeassistant/switch/" +
        nodeId +
        "_relay" +
        String(i + 1) +
        "/config";

    String switchPayload =
        "{"
        "\"name\":\"Relay " + String(i + 1) + "\","
        "\"unique_id\":\"" + nodeId + "_relay" + String(i + 1) + "\","
        "\"state_topic\":\"home/" + nodeId + "/relay" + String(i + 1) + "/state\","
        "\"command_topic\":\"home/" + nodeId + "/relay" + String(i + 1) + "/set\","
        "\"payload_on\":\"ON\","
        "\"payload_off\":\"OFF\","
        "\"device\":{"
            "\"identifiers\":[\"" + nodeId + "\"],"
            "\"name\":\"" + nodeId + "\","
            "\"manufacturer\":\"DIY\","
            "\"model\":\"Arduino Mega W5500\""
        "}"
        "}";

    mqtt.publish(
        switchTopic.c_str(),
        switchPayload.c_str(),
        true
    );
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    String topic =
        "homeassistant/binary_sensor/" +
        nodeId +
        "_input" +
        String(i + 1) +
        "/config";

    String payload =
        "{"
        "\"name\":\"Input " + String(i + 1) + "\","
        "\"unique_id\":\"" + nodeId + "_input" + String(i + 1) + "\","
        "\"state_topic\":\"home/" + nodeId + "/input" + String(i + 1) + "\","
        "\"payload_on\":\"ON\","
        "\"payload_off\":\"OFF\","
        "\"device\":{"
            "\"identifiers\":[\"" + nodeId + "\"],"
            "\"name\":\"" + nodeId + "\","
            "\"manufacturer\":\"DIY\","
            "\"model\":\"Arduino Mega W5500\""
        "}"
        "}";

    mqtt.publish(topic.c_str(), payload.c_str(), true);
  }

  for (int i = 0; i < NUM_PWM; i++)
  {
    String topic =
        "homeassistant/light/" +
        nodeId +
        "_pwm" +
        String(i + 1) +
        "/config";

    String payload =
        "{"
        "\"name\":\"PWM " + String(i + 1) + "\","
        "\"unique_id\":\"" + nodeId + "_pwm" + String(i + 1) + "\","
        "\"schema\":\"json\","
        "\"command_topic\":\"home/" + nodeId + "/pwm" + String(i + 1) + "/set\","
        "\"state_topic\":\"home/" + nodeId + "/pwm" + String(i + 1) + "/state\","
        "\"brightness\":true,"
        "\"device\":{"
            "\"identifiers\":[\"" + nodeId + "\"],"
            "\"name\":\"" + nodeId + "\","
            "\"manufacturer\":\"DIY\","
            "\"model\":\"Arduino Mega W5500\""
        "}"
        "}";

    mqtt.publish(
        topic.c_str(),
        payload.c_str(),
        true
    );
  }

  for (int i = 0; i < NUM_COUNTERS; i++)
    {
        String topic =
            "homeassistant/sensor/" +
            nodeId +
            "_counter" +
            String(i + 1) +
            "_count/config";

        String payload =
            "{"
            "\"name\":\"Counter " + String(i + 1) + " Count\","
            "\"unique_id\":\"" + nodeId + "_counter" + String(i + 1) + "_count\","
            "\"state_topic\":\"home/" + nodeId + "/counter" + String(i + 1) + "/count\","
            "\"state_class\":\"total_increasing\","
            "\"device\":{"
                "\"identifiers\":[\"" + nodeId + "\"],"
                "\"name\":\"" + nodeId + "\","
                "\"manufacturer\":\"DIY\","
                "\"model\":\"Arduino Mega W5500\""
            "}"
            "}";

        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    for (int i = 0; i < NUM_COUNTERS; i++)
    {
        String topic =
            "homeassistant/sensor/" +
            nodeId +
            "_counter" +
            String(i + 1) +
            "_rate/config";

        String payload =
            "{"
            "\"name\":\"Counter " + String(i + 1) + " Rate\","
            "\"unique_id\":\"" + nodeId + "_counter" + String(i + 1) + "_rate\","
            "\"state_topic\":\"home/" + nodeId + "/counter" + String(i + 1) + "/rate\","
            "\"unit_of_measurement\":\"Hz\","
            "\"state_class\":\"measurement\","
            "\"device\":{"
                "\"identifiers\":[\"" + nodeId + "\"],"
                "\"name\":\"" + nodeId + "\","
                "\"manufacturer\":\"DIY\","
                "\"model\":\"Arduino Mega W5500\""
            "}"
            "}";

        mqtt.publish(topic.c_str(), payload.c_str(), true);

        String topic2 =
        "homeassistant/sensor/" +
        nodeId +
        "_counter" +
        String(i + 1) +
        "_rpm/config";

    String payload2 =
        "{"
        "\"name\":\"Counter " + String(i + 1) + " RPM\","
        "\"unique_id\":\"" + nodeId + "_counter" + String(i + 1) + "_rpm\","
        "\"state_topic\":\"home/" + nodeId + "/counter" + String(i + 1) + "/rpm\","
        "\"unit_of_measurement\":\"RPM\","
        "\"state_class\":\"measurement\","
        "\"device\":{"
            "\"identifiers\":[\"" + nodeId + "\"],"
            "\"name\":\"" + nodeId + "\","
            "\"manufacturer\":\"DIY\","
            "\"model\":\"Arduino Mega W5500\""
        "}"
        "}";

    mqtt.publish(
        topic2.c_str(),
        payload2.c_str(),
        true
        );
    }

    for (int i = 0; i < oneWireSensorCount; i++)
    {
        if (!isDS18B20(i))
        {
            continue;
        }

        String address = oneWireAddress(i);

        String topic =
            "homeassistant/sensor/" +
            nodeId +
            "_onewire_" +
            address +
            "/config";

        String payload =
            "{"
            "\"name\":\"Temperature " + address + "\","
            "\"unique_id\":\"" +
                nodeId +
                "_onewire_" +
                address +
                "\","
            "\"state_topic\":\"" +
                oneWireTopic(i) +
                "\","
            "\"device_class\":\"temperature\","
            "\"unit_of_measurement\":\"°C\","
            "\"state_class\":\"measurement\","
            "\"device\":{"
                "\"identifiers\":[\"" + nodeId + "\"],"
                "\"name\":\"" + nodeId + "\","
                "\"manufacturer\":\"DIY\","
                "\"model\":\"Arduino Mega W5500\""
            "}"
            "}";

        mqtt.publish(
            topic.c_str(),
            payload.c_str(),
            true
        );
    }

  LOG("Discovery published");
}

void reconnectMQTT()
{
  while (!mqtt.connected())
  {
    LOG("Connecting MQTT...");

    if (
      mqtt.connect(
        nodeId.c_str(),
        config.mqttUser,
        config.mqttPassword,
        statusTopic.c_str(),
        1,
        true,
        "offline"
      )
    )
    {
      LOG("MQTT connected");

      publishDiscovery();

      mqtt.publish(
        statusTopic.c_str(),
        "online",
        true
      );

      publishSensors();

      for (int i = 0; i < NUM_RELAYS; i++)
      {
        mqtt.subscribe(stateTopic("relay", i + 1).c_str());
      }

      for (int i = 0; i < NUM_RELAYS; i++)
      {
      mqtt.publish(
          stateTopic("relay", i + 1).c_str(),
          relayStates[i] ? "ON" : "OFF",
          true
        );
      }

      for (int i = 0; i < NUM_PWM; i++)
      {
        mqtt.subscribe(commandTopic("pwm", i + 1).c_str());
      }

      for (int i = 0; i < NUM_PWM; i++)
      {
        mqtt.publish(stateTopic("pwm", i + 1).c_str(),
          "{\"state\":\"OFF\",\"brightness\":0}",
          true
        );
      }
    }
    else
    {
      LOGP("MQTT failed: ");
      LOG(mqtt.state());

      delay(5000);
    }
  }
}

void initMQTT()
{
    mqttServer.fromString(config.mqttServer);

    mqtt.setServer(mqttServer, 1883);

    mqtt.setBufferSize(1024);

    mqtt.setCallback(callback);
}

void mqttLoop()
{
    if (!mqtt.connected())
    {
        reconnectMQTT();
    }

    mqtt.loop();

    publishInputChanges();

    if (millis() - lastPublish >= publishInterval)
    {
        lastPublish = millis();

        updateCounters(publishInterval);
        
        publishSensors();

        publishCounters();

        publishOneWireSensors();
    }
}