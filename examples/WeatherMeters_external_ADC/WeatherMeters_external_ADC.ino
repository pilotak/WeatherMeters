#include <Wire.h>
#include <AllAboutEE_MCP3021.h>  // https://github.com/AllAboutEE/ESP8266-MCP3021-Library/tree/master/Software/ESP8266-Arduino-Library-For-MCP3021
#include "WeatherMeters.h"

using namespace AllAboutEE;

const int anemometer_pin = 2;
const int raingauge_pin = 3;

MCP3021 mcp3021;
WeatherMeters <6> meters;  // filter last 6 directions

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);
    mcp3021.begin(SDA, SCL);
}

void loop() {
    static uint8_t counter = 1;

    if (counter == 8) {
        uint16_t adc_result = mcp3021.read(0);

        Serial.print(F("Wind degrees: "));
        Serial.print(meters.adcToDir(adc_result), 1);
        Serial.print(F(" Wind speed: "));
        Serial.print(meters.getSpeed(), 1);
        Serial.print(F("km/h, Rain: "));
        Serial.print(meters.getRain(), 4);
        Serial.println(F("mm"));

        counter = 0;
    }

    counter++;

    delay(1000);
}
