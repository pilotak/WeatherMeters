/*
MIT License
Copyright (c) 2018 Pavel Slama
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef WEATHERMETERS_H
#define WEATHERMETERS_H

#include "Arduino.h"

#define RAIN_GAUGE_RES 0.2794  // mm
#define WIND_SPEED_RES 2.4  // km/h

enum Weather_mode {
  Weather_manual = 0,
  Weather_auto = 1
};


class WeatherMeters {
 public:
  explicit WeatherMeters(int windvane_pin, uint32_t samples);
  void attach(void (*callback)(void));
  float getWindSpeed();
  float getWindVane();
  float getRainGauge();
  void init(HardwareSerial * serial = NULL);

  void secondCount();
  void intAnemometer();
  void intRaingauge();

 protected:
  const int _windvane_pin;
  HardwareSerial * _serial;

 private:
  void update();
  void readWindVane();
  void countWindVane();

  void (*_callback)(void);

  const uint32_t _samples;

  uint32_t _rain_ticks;
  uint32_t _rain_sum;

  uint32_t _anemometer_ticks;
  uint32_t _anemometer_sum;
  uint32_t _anemometer_samples_passed;

  uint32_t _windvane_degree_sum;
  uint32_t _windvane_result;
  uint32_t _windwane_samples_passed;

  volatile uint16_t _second_counter;
};

#endif  // WEATHERMETERS_H
