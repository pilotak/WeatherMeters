#include <RTClock.h>
#include "WeatherMeters.h"

const int windvane_pin = PB1;
const int anemometer_pin = PB12;
const int raingauge_pin = PB14;

RTClock rtclock(RTCSEL_LSE);
WeatherMeters <6> meters(windvane_pin);  // filter last 6 directions

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void secondCount() {
    meters.timer();
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);

    rtclock.attachSecondsInterrupt(secondCount);
}

void loop() {
    Serial.print("Wind degrees: ");
    Serial.print(meters.getDir(), 1);
    Serial.print(" Wind speed: ");
    Serial.print(meters.getSpeed(), 1);
    Serial.print("km/h, Rain: ");
    Serial.print(meters.getRain(), 4);
    Serial.println("mm");

    delay(8000);
}
