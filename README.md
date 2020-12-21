# Weather Meters
[![build](https://github.com/pilotak/WeatherMeters/workflows/build/badge.svg)](https://github.com/pilotak/WeatherMeters/actions) 
[![Framework Badge Arduino](https://img.shields.io/badge/framework-arduino-00979C.svg)](https://arduino.cc)

Arduino library for processing wind speed, wind wane and rain gauge sensors (WH1080, WH1090, Sparkfun)

It's interupt based library with callbacks and ready to use on any board. The only difference accros boards would be in initializing 1 second timer, ie. on STM32 you could use RTC timer, ESP32 has its own timer object, Timer1 could be used Arduino Uno (ATmega328P) etc please see `examples/timers`

# Example

```cpp
#include "WeatherMeters.h"

const int windvane_pin = A0;
const int anemometer_pin = 2;
const int raingauge_pin = 3;

// filter array = "binary" length only: 2, 4, 8, 16, 32, 64, 128, etc.
WeatherMeters <4> meters(windvane_pin);  // filter last 4 directions

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
```