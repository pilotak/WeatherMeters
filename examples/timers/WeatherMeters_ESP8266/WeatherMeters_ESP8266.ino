#include <Ticker.h>
#include "WeatherMeters.h"

const int windvane_pin = A0;
const int anemometer_pin = 12;
const int raingauge_pin = 14;

volatile bool got_data = false;

Ticker ticker;
WeatherMeters <4> meters(windvane_pin, 8);  // filter last 4 directions, refresh data every 8 sec

void ICACHE_RAM_ATTR intAnemometer() {
    meters.intAnemometer();
}

void ICACHE_RAM_ATTR intRaingauge() {
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
    ticker.attach(1.0, secondCount);
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
