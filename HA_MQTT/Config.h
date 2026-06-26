#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define CONFIG_MAGIC 0xA55A1234

struct Config
{
    uint32_t magic;

    char mqttServer[16];

    char mqttUser[32];

    char mqttPassword[32];

    bool dhcpEnabled;

    uint8_t reserved[32];
};

extern Config config;

bool loadConfig();
void saveConfig();
void configureNode();

#endif