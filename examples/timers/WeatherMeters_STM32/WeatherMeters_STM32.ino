#include <RTClock.h>
#include "WeatherMeters.h"

const int windvane_pin = PB1;
const int anemometer_pin = PB12;
const int raingauge_pin = PB14;

volatile bool got_data = false;

RTClock rtclock(RTCSEL_LSE);
WeatherMeters <6> meters(windvane_pin, 8);  // filter last 6 directions, refresh data every 8 sec

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void secondCount() {
    meters.timer();
}

void readDone(void) {
    got_data = true;
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);

    meters.attach(readDone);
    rtclock.attachSecondsInterrupt(secondCount);
    meters.reset();  // in case we got already some interrupts
}

void loop() {
    if (got_data) {
        got_data = false;
        Serial.print("Wind degrees: ");
        Serial.print(meters.getDir(), 1);
        Serial.print(" Wind speed: ");
        Serial.print(meters.getSpeed(), 1);
        Serial.print("km/h, Rain: ");
        Serial.print(meters.getRain(), 4);
        Serial.println("mm");
    }

    delay(1); // or do something else
}
