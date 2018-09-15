#include <RTClock.h>
#include "WeatherMeters.h"

#define anemometer_pin PB12
#define windvane_pin PB1
#define raingauge_pin PB14

#define SAMPLES 8 // = 8 sec output

volatile bool got_data = false;

RTClock rtclock(RTCSEL_LSE);
WeatherMeters meters(windvane_pin, SAMPLES);

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void secondCount() {
    meters.secondCount();
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

    meters.init();
}

void loop() {
    if (got_data) {
        got_data = false;
        Serial.print("Wind degrees: ");
        Serial.print(meters.getWindVane(), 1);
        Serial.print(" Wind speed: ");
        Serial.print(meters.getWindSpeed(), 1);
        Serial.print("km/h, Rain: ");
        Serial.print(meters.getRainGauge(), 4);
        Serial.println("mm");
    }
}