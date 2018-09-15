#include "WeatherMeters.h"

const int windvane_pin = A0;
const int anemometer_pin = 2;
const int raingauge_pin = 3;

WeatherMeters <6> meters(windvane_pin);  // filter last 6 directions

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
