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

#define RAIN_GAUGE_RES 0.2794  // mm
#define WIND_SPEED_RES 2.4  // km/h

template <uint8_t N>
class WeatherMeters {
 public:
  WeatherMeters(int windvane_pin = -1);
  virtual ~WeatherMeters(void);

  void attach(void (*callback)(void));
  void debug(HardwareSerial * serial = NULL);
  float adcToDir(uint16_t value);
  float getDir();
  float getSpeed();
  float getRain();
  void intAnemometer();
  void intRaingauge();

  /*float getWindSpeed();
  float getWindVane();
  float getRainGauge();*/

  /*void secondCount();
  void intAnemometer();
  void intRaingauge();*/

 protected:
  HardwareSerial * _serial;

 private:
#if N > 0
  MovingAverageAngle <N> _dir_filter;
#endif
  void (*_callback)(void);

  const Mode _mode;
  const int _windvane_pin;
  uint32_t _anemometer_ticks;
  uint32_t _rain_ticks;
  float _dir;
  float _speed;
  float _rain;

  void readWindVane();



  /*void update();
  void readWindVane();
  void countWindVane();



  const uint32_t _samples;


  uint32_t _rain_sum;


  uint32_t _anemometer_sum;
  uint32_t _anemometer_samples_passed;

  uint32_t _windvane_degree_sum;
  uint32_t _windvane_result;
  uint32_t _windwane_samples_passed;

  volatile uint16_t _second_counter;*/
};

template <uint8_t N>
WeatherMeters<N>::WeatherMeters(int windvane_pin):
  _callback(NULL),
  _windvane_pin(windvane_pin),
  _anemometer_ticks(0),
  _rain_ticks(0),
  _dir(NAN),
  _speed(NAN),
  _rain(NAN) {
#if defined(STM32_MCU_SERIES)
  if (windvane_pin > -1) {
    pinMode(windvane_pin, INPUT_ANALOG);
  }
#endif
}

template <uint8_t N>
WeatherMeters<N>::~WeatherMeters(void) {
}

template <uint8_t N>
void WeatherMeters<N>::attach(void (*callback)(void)) {
  _callback = callback;
}

template <uint8_t N>
void WeatherMeters<N>::debug(HardwareSerial * serial) {
  _serial = serial;
}

template <uint8_t N>
void WeatherMeters<N>::readWindVane() {
  adcToDir(analogRead(_windvane_pin));
}

template <uint8_t N>
float WeatherMeters<N>::adcToDir(uint16_t value) {
  uint16_t dir = 0;

  // Map ADC to degrees
  for (uint8_t i = 0; i < 16; i++) {
    if (value >= windvane_table[15][1]) {
      // prevent overflow of index "i"
      dir = windvane_table[15][0];
      break;

    } else if (value <= (windvane_table[i][1] + ((windvane_table[i + 1][1] - windvane_table[i][1]) >> 1))) {
      // value can be up to half the difference to next
      dir = windvane_table[i][0];
      break;
    }
  }

  if (_serial) {
    _serial->print(F("[WEATHER] Wind vane ADC:"));
    _serial->print(value);
    _serial->print(F(", "));
  }

#if N > 0
  float filtered_dir = _dir_filter.add(static_cast<float>(dir) / 10);

  filtered_dir = ceil(filtered_dir / 22.5) * 22.5;

  if (_serial) {
    _serial->print(F("filtered dir: "));
  }

  _average_dir = filtered_dir;

#else
  _average_dir = static_cast<float>(dir) / 10;

  if (_serial) {
    _serial->print(F("unfiltered dir: "));
  }
#endif

  if (_serial) {
    _serial->println(_average_dir, 1);
  }

  return _average_dir;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getDir() {
  return _average_dir;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getSpeed() {
  if (!_callback) {
    _anemometer_ticks = 0;
    _speed = NAN;
  }
  return _speed;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getRain() {
  float result = static_cast<float>(_rain_ticks) * RAIN_GAUGE_RES;

  if (!_callback) {
    _rain_ticks = 0;
  }

  return result;
}

template <uint8_t N>
void WeatherMeters<N>::intAnemometer() {
  _anemometer_ticks++;
}

template <uint8_t N>
void WeatherMeters<N>::intRaingauge() {
  _rain_ticks++;
}

#endif  // WEATHERMETERS_H
