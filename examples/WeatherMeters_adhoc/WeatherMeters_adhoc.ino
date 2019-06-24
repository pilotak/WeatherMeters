#include "WeatherMeters.h"

#if (defined(ESP8266) || defined(ESP32)) && !defined(ISR_PREFIX)
    #define ISR_PREFIX ICACHE_RAM_ATTR
#else
    #define ISR_PREFIX
#endif

const int windvane_pin = A0;
const int anemometer_pin = 2;
const int raingauge_pin = 3;

WeatherMeters <4> meters(windvane_pin);  // filter last 4 directions

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
}

void loop() {
    Serial.print(F("Wind degrees: "));
    Serial.print(meters.getDir(), 1);
    Serial.print(F(" Wind speed: "));
    Serial.print(meters.getSpeed(), 1);
    Serial.print(F("km/h, Rain: "));
    Serial.print(meters.getRain(), 4);
    Serial.println(F("mm"));

    delay(8000);
}
