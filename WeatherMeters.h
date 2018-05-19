#ifndef WEATHERMETERS_H
#define WEATHERMETERS_H

#include "Arduino.h"

#define RAIN_GAUGE_RES 0.2794  // mm
#define WIND_SPEED_RES 2.4  // km/h
#define SAMPLES 8

enum Weather_mode {
  Weather_manual = 0,
  Weather_auto = 1
};


class WeatherMeters {
 public:
  explicit WeatherMeters(int windvane_pin);
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
