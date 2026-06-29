#include "Config.h"
#include <EEPROM.h>

#define CONFIG_MAGIC 0x435A1234

void saveConfig()
{
    EEPROM.put(0, config);
}

bool loadConfig()
{
    EEPROM.get(0, config);

    return config.magic == CONFIG_MAGIC;
}

Config config;

void configureNode()
{
    Serial.println();
    Serial.println("=== CONFIGURATION MODE ===");

    Serial.println("MQTT Server:");
    while (!Serial.available());
    String server = Serial.readStringUntil('\n');
    server.trim();

    Serial.println("MQTT User:");
    while (!Serial.available());
    String user = Serial.readStringUntil('\n');
    user.trim();

    Serial.println("MQTT Password:");
    while (!Serial.available());
    String password = Serial.readStringUntil('\n');
    password.trim();

    config.magic = CONFIG_MAGIC;

    strncpy(config.mqttServer,
            server.c_str(),
            sizeof(config.mqttServer));

    strncpy(config.mqttUser,
            user.c_str(),
            sizeof(config.mqttUser));

    strncpy(config.mqttPassword,
            password.c_str(),
            sizeof(config.mqttPassword));

    saveConfig();

      Serial.println();
    Serial.println("Configuration entered:");

    Serial.print("MQTT Server: ");
    Serial.println(config.mqttServer);

    Serial.print("MQTT User: ");
    Serial.println(config.mqttUser);

    Serial.println("MQTT Password: ********");

    Serial.println();
    Serial.println("Configuration saved.");
    Serial.println("Reset Arduino.");

    while (true);
}