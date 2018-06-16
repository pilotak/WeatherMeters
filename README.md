# Weather Meters
Arduino library for processing wind speed, wind wane and rain gauge sensors (WH 1080, Sparkfun)

It's interupt based library with callbacks and ready to use on any board. The only difference accros boards would be in initializing 1 second timer, ie. on STM32 you could use RTC timer, ESP32 has its own timer object, Timer1 could be used Arduino Uno (ATmega328P) etc.

# Example on STM32 platform

```cpp
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
```