#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoUniqueID.h>
#include <math.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "Config.h"

// --------------------------------------------------
// Network
// --------------------------------------------------

byte mac[6];

IPAddress ip;

IPAddress mqttServer;
EthernetClient ethClient;
PubSubClient mqtt(ethClient);

// --------------------------------------------------
// Node ID
// --------------------------------------------------

String nodeId;
String statusTopic = "home/" + nodeId + "/status";

const uint8_t analogPins[4] = {A0, A1, A2, A3};

int adcValues[4];
float temperatures[4];

const uint8_t NUM_RELAYS = 8;
const uint8_t relayPins[NUM_RELAYS] = { 22, 23, 24, 25, 26, 27, 28, 29};
bool relayStates[NUM_RELAYS] =
{
    false, false, false, false,
    false, false, false, false
};

const uint8_t NUM_INPUTS = 8;
const uint8_t inputPins[NUM_INPUTS] = { 30, 31, 32, 33, 34, 35, 36, 37};
bool inputStates[NUM_INPUTS] =
{
    false, false, false, false,
    false, false, false, false
};

const uint8_t NUM_PWM = 3;
const uint8_t pwmPins[NUM_PWM] = { 44, 45, 46 };
uint8_t pwmValues[NUM_PWM] = { 0, 0, 0};

unsigned long lastPublish = 0;
const unsigned long publishInterval = 5000;









// --------------------------------------------------






void buildNodeId()
{
  nodeId = "MEGA_";

  for (size_t i = 0; i < UniqueIDsize; i++)
  {
    char buf[3];
    sprintf(buf, "%02X", UniqueID[i]);
    nodeId += buf;
  }

  Serial.print("Node ID: ");
  Serial.println(nodeId);
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

    Serial.print("MAC: ");

    for (int i = 0; i < 6; i++)
    {
        Serial.print(mac[i], HEX);
        Serial.print(":");
    }

    Serial.println();
}

// --------------------------------------------------

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

  Serial.println("Discovery published");
}

void publishSensors()
{
  for (int i = 0; i < 4; i++)
  {
    adcValues[i] = analogRead(analogPins[i]);

    temperatures[i] = adcToTemperature(adcValues[i]);

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

    Serial.print("A");
    Serial.print(i);

    Serial.print(" ADC=");
    Serial.print(adcValues[i]);

    Serial.print(" Temp=");
    Serial.println(temperatures[i]);
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
    String msg;

    for (unsigned int i = 0; i < length; i++)
    {
        msg += (char)payload[i];
    }

    String topicStr = topic;

    for (int relay = 0; relay < NUM_RELAYS; relay++)
    {
        String expectedTopic =
            "home/" +
            nodeId +
            "/relay" +
            String(relay + 1) +
            "/set";

        if (topicStr == expectedTopic)
        {
            relayStates[relay] =
                (msg == "ON");

            digitalWrite(
                relayPins[relay],
                relayStates[relay]
            );

            mqtt.publish(
                ("home/" +
                 nodeId +
                 "/relay" +
                 String(relay + 1) +
                 "/state").c_str(),
                relayStates[relay]
                    ? "ON"
                    : "OFF",
                true
            );

            Serial.print("Relay ");
            Serial.print(relay + 1);
            Serial.print(" = ");
            Serial.println(msg);
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

              analogWrite(
                  pwmPins[pwm],
                  pwmValues[pwm]
              );

              String statePayload =
                  "{\"state\":\"" +
                  String(pwmValues[pwm] > 0 ? "ON" : "OFF") +
                  "\",\"brightness\":" +
                  String(pwmValues[pwm]) +
                  "}";

              mqtt.publish(
                  ("home/" +
                  nodeId +
                  "/pwm" +
                  String(pwm + 1) +
                  "/state").c_str(),
                  statePayload.c_str(),
                  true
              );

              Serial.print("PWM ");
              Serial.print(pwm + 1);
              Serial.print(" = ");
              Serial.println(pwmValues[pwm]);
          }
        
      }
    }
  
}

// --------------------------------------------------

void reconnectMQTT()
{
  while (!mqtt.connected())
  {
    Serial.println("Connecting MQTT...");

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
      Serial.println("MQTT connected");

      publishDiscovery();

      mqtt.publish(
        statusTopic.c_str(),
        "online",
        true
      );

      publishSensors();

      for (int i = 0; i < NUM_RELAYS; i++)
      {
        mqtt.subscribe(
            ("home/" +
            nodeId +
            "/relay" +
            String(i + 1) +
            "/set").c_str()
        );
      }

      for (int i = 0; i < NUM_RELAYS; i++)
      {
      mqtt.publish(
          ("home/" +
          nodeId +
          "/relay" +
          String(i + 1) +
          "/state").c_str(),
          relayStates[i] ? "ON" : "OFF",
          true
        );
      }

      for (int i = 0; i < NUM_PWM; i++)
      {
        mqtt.subscribe(
          ("home/" +
          nodeId +
          "/pwm" +
          String(i + 1) +
          "/set").c_str()
        );
      }

      for (int i = 0; i < NUM_PWM; i++)
      {
      String stateTopic =
          "home/" +
          nodeId +
          "/pwm" +
          String(i + 1) +
          "/state";

        mqtt.publish(
          stateTopic.c_str(),
          "{\"state\":\"OFF\",\"brightness\":0}",
          true
        );
      }
    }
    else
    {
      Serial.print("MQTT failed: ");
      Serial.println(mqtt.state());

      delay(5000);
    }
  }
}

float adcToTemperature(int adc)
{
  if (adc <= 0) adc = 1;
  if (adc >= 1023) adc = 1022;

  const float SERIES_RESISTOR = 10000.0;
  const float NOMINAL_RESISTANCE = 10000.0;
  const float NOMINAL_TEMPERATURE = 25.0;
  const float B_COEFFICIENT = 3950.0;

  float resistance =
    SERIES_RESISTOR /
    ((1023.0 / adc) - 1.0);

  float steinhart;

  steinhart = resistance / NOMINAL_RESISTANCE;
  steinhart = log(steinhart);
  steinhart /= B_COEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  steinhart = 1.0 / steinhart;
  steinhart -= 273.15;

  return steinhart;
}

void readInputs()
{
    for (int i = 0; i < NUM_INPUTS; i++)
    {
        bool newState = !digitalRead(inputPins[i]); 
        // inverted because pullup

        if (newState != inputStates[i])
        {
            inputStates[i] = newState;

            String topic =
                "home/" + nodeId +
                "/input" + String(i + 1);

            mqtt.publish(
                topic.c_str(),
                newState ? "ON" : "OFF",
                true
            );

            Serial.print("Input ");
            Serial.print(i + 1);
            Serial.print(" = ");
            Serial.println(newState ? "ON" : "OFF");
        }
    }
}


// --------------------------------------------------

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

  Serial.println();
  Serial.println("Configuration:");

  Serial.print("MQTT Server: ");
  Serial.println(config.mqttServer);

  Serial.print("MQTT User: ");
  Serial.println(config.mqttUser);

  Serial.println("MQTT Password: ********");

  mqttServer.fromString(config.mqttServer);

  buildNodeId();
  buildMac();


  for (int i = 0; i < NUM_RELAYS; i++)
  {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

  for (int i = 0; i < NUM_INPUTS; i++)
  {
    pinMode(inputPins[i], INPUT_PULLUP);
  }

for (int i = 0; i < NUM_PWM; i++)
  {
    pinMode(pwmPins[i], OUTPUT);
    //analogWrite(pwmPins[i], 0);
  }

  Serial.println("Before Ethernet.init");

Ethernet.init(10);

Serial.println("Before DHCP");

int dhcpResult = Ethernet.begin(mac);

Serial.println("After DHCP");

Serial.print("DHCP result = ");
Serial.println(dhcpResult);

if (dhcpResult == 0)
{
    Serial.println("DHCP failed");
}

  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());

  mqtt.setServer(mqttServer, 1883);
  mqtt.setBufferSize(1024);

  mqtt.setCallback(callback);
}

// --------------------------------------------------

void loop()
{
  Ethernet.maintain();

  if (!mqtt.connected())
  {
    reconnectMQTT();
  }

  mqtt.loop();

  readInputs();

  if (millis() - lastPublish >= publishInterval)
  {
    lastPublish = millis();

    publishSensors();
  }
}