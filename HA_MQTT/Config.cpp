#include "Config.h"
#include "Debug.h"
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
    LOG();
    LOG("=== CONFIGURATION MODE ===");

    LOG("MQTT Server:");
    while (!Serial.available());
    String server = readLine();
    server.trim();

    LOG("MQTT User:");
    while (!Serial.available());
    String user = readLine();
    user.trim();

    LOG("MQTT Password:");
    while (!Serial.available());
    String password = readLine();
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

    LOG();
    LOG("Configuration entered:");

    LOG("MQTT Server: ");
    LOG(config.mqttServer);

    LOG("MQTT User: ");
    LOG(config.mqttUser);

    LOG("MQTT Password: ********");

    LOG();
    LOG("Configuration saved.");
    LOG("Reset Arduino.");

    while (true);
}