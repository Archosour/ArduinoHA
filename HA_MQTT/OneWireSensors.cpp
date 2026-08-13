#include "OneWireSensors.h"
#include "Pins.h"
#include "Debug.h"

#include <OneWire.h>

OneWire oneWire(ONEWIRE_PIN);

OneWireSensor oneWireSensors[MAX_ONEWIRE_SENSORS];
int oneWireSensorCount = 0;


// --------------------------------------------------
// Initialize OneWire
// --------------------------------------------------

void initOneWire()
{
    LOG("Initializing OneWire...");

    oneWireSensorCount = 0;

    discoverOneWireSensors();

    LOG("OneWire sensors found: ");
    LOG(oneWireSensorCount);
}


// --------------------------------------------------
// Discover sensors
// --------------------------------------------------

void discoverOneWireSensors()
{
    byte address[8];

    oneWire.reset_search();

    while (oneWire.search(address))
    {
        if (oneWireSensorCount >= MAX_ONEWIRE_SENSORS)
        {
            LOG("Maximum OneWire sensor count reached.");
            break;
        }

        // Verify ROM address
        if (OneWire::crc8(address, 7) != address[7])
        {
            LOG("Invalid OneWire ROM address.");
            continue;
        }

        OneWireSensor &sensor =
            oneWireSensors[oneWireSensorCount];

        memcpy(sensor.address, address, 8);

        sensor.family = address[0];
        sensor.temperature = NAN;
        sensor.valid = false;

        LOGP("Found OneWire device: ");
        LOG(oneWireAddress(oneWireSensorCount));

        if (sensor.family == 0x28)
        {
            LOG("  Type: DS18B20");
        }
        else
        {
            LOG("  Type: unsupported");
        }

        oneWireSensorCount++;
    }

    oneWire.reset_search();
}


// --------------------------------------------------
// Update temperatures
// --------------------------------------------------

void updateOneWireSensors()
{
    for (int i = 0; i < oneWireSensorCount; i++)
    {
        if (!isDS18B20(i))
        {
            continue;
        }

        byte *address = oneWireSensors[i].address;

        // Start temperature conversion
        if (!oneWire.reset())
        {
            oneWireSensors[i].valid = false;
            continue;
        }

        oneWire.select(address);
        oneWire.write(0x44, 1);

        // Wait for conversion.
        // 750 ms is the maximum for 12-bit DS18B20 resolution.
        delay(750);

        // Read scratchpad
        if (!oneWire.reset())
        {
            oneWireSensors[i].valid = false;
            continue;
        }

        oneWire.select(address);
        oneWire.write(0xBE);

        byte data[9];

        for (int j = 0; j < 9; j++)
        {
            data[j] = oneWire.read();
        }

        // Verify scratchpad CRC
        if (OneWire::crc8(data, 8) != data[8])
        {
            LOGP("CRC error reading OneWire sensor: ");
            LOG(oneWireAddress(i));

            oneWireSensors[i].valid = false;
            continue;
        }

        int16_t rawTemperature =
            (data[1] << 8) | data[0];

        oneWireSensors[i].temperature =
            rawTemperature / 16.0;

        LOG("temperature one-wire:");
        LOG(oneWireSensors[i].temperature);

        oneWireSensors[i].valid = true;
    }
}


// --------------------------------------------------
// Get address as string
// --------------------------------------------------

String oneWireAddress(int index)
{
    if (index < 0 || index >= oneWireSensorCount)
    {
        return "";
    }

    String result;

    for (int i = 0; i < 8; i++)
    {
        if (i > 0)
        {
            result += "-";
        }

        if (oneWireSensors[index].address[i] < 0x10)
        {
            result += "0";
        }

        result += String(
            oneWireSensors[index].address[i],
            HEX
        );
    }

    result.toUpperCase();

    return result;
}


// --------------------------------------------------
// Check sensor type
// --------------------------------------------------

bool isDS18B20(int index)
{
    if (index < 0 || index >= oneWireSensorCount)
    {
        return false;
    }

    return oneWireSensors[index].family == 0x28;
}