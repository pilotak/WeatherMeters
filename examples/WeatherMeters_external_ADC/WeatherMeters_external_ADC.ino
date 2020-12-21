#include <Wire.h>
#include <MCP3X21.h>  // https://github.com/pilotak/MCP3X21
#include "WeatherMeters.h"

#if (defined(ESP8266) || defined(ESP32)) && !defined(ISR_PREFIX)
    #define ISR_PREFIX ICACHE_RAM_ATTR
#else
    #define ISR_PREFIX
#endif

const int anemometer_pin = 2;
const int raingauge_pin = 3;

MCP3021 mcp3021;
WeatherMeters <4> meters;  // filter last 4 directions

void ISR_PREFIX intAnemometer() {
    meters.intAnemometer();
}

void ISR_PREFIX intRaingauge() {
    meters.intRaingauge();
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);

#if defined(ESP8266)
    Wire.begin(SDA, SCL);
    mcp3021.init(&Wire);
#else
    mcp3021.init();
#endif
}

void loop() {
    static uint8_t counter = 1;

    if (counter == 8) {
        uint16_t adc_result = mcp3021.read();

        Serial.print("Wind degrees: ");
        Serial.print(meters.adcToDir(adc_result), 1);
        Serial.print(" Wind speed: ");
        Serial.print(meters.getSpeed(), 1);
        Serial.print("km/h, Rain: ");
        Serial.print(meters.getRain(), 4);
        Serial.println("mm");

        counter = 0;
    }

    counter++;

    delay(1000);
}
