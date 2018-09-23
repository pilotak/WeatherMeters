#include <Wire.h>
#include <MCP3X21.h>  // https://github.com/pilotak/MCP3X21
#include "WeatherMeters.h"

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

    Wire.begin(SDA, SCL);
    mcp3021.init(&Wire);
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
