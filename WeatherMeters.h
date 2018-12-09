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

#include <MovingAverageAngle.h>

#define RAIN_GAUGE_RES 0.2794  // mm
#define WIND_SPEED_RES 2.4  // km/h

#if WM_ADC_RESOLUTION == 4096 || defined(STM32_MCU_SERIES) || defined(ARDUINO_ARCH_ESP32)
const static uint16_t _windvane_table[16][2] = {
    {1125, 264},
    {675,  335},
    {900,  372},
    {1575, 506},
    {1350, 739},
    {2025, 979},
    {1800, 1149},
    {225,  1624},
    {450,  1845},
    {2475, 2398},
    {2250, 2521},
    {3375, 2811},
    {0,    3143},
    {2925, 3310},
    {3150, 3549},
    {2700, 3781}
};

#elif WM_ADC_RESOLUTION == 1024 || defined(__AVR__) || defined(ESP8266)
const static uint16_t _windvane_table[16][2] = {
    {1125, 66},
    {675,  84},
    {900,  93},
    {1575, 127},
    {1350, 185},
    {2025, 245},
    {1800, 287},
    {225,  406},
    {450,  461},
    {2475, 599},
    {2250, 630},
    {3375, 703},
    {0,    730},
    {2925, 828},
    {3150, 887},
    {2700, 945}
};
#endif

typedef void (*WeaterMetersCallback) ();

template <uint8_t N>
class WeatherMeters {
  public:
    explicit WeatherMeters(int windvane_pin = -1, uint16_t period = 0);
    virtual ~WeatherMeters(void);
    void attach(WeaterMetersCallback callback);
    void attachRain(WeaterMetersCallback callback);
    void debug(HardwareSerial * serial = NULL);
    float adcToDir(uint16_t value);
    float getDir();
    float getSpeed();
    float getRain();
    void intAnemometer();
    void intRaingauge();
    void timer();
    void reset();

  protected:
    HardwareSerial * _serial;

  private:
    MovingAverageAngle <N> * _dirFilter;
    WeaterMetersCallback _callback;
    WeaterMetersCallback _rain_callback;

    const int _windvane_pin;
    const uint16_t _period;
    volatile uint32_t _anemometer_ticks;
    volatile uint32_t _anemometer_sum;
    volatile uint32_t _timer_passed;
    volatile uint32_t _rain_ticks;
    volatile uint32_t _rain_sum;
    volatile float    _dir;
    volatile uint16_t _timer_counter;
};

template <uint8_t N>
WeatherMeters<N>::WeatherMeters(int windvane_pin, uint16_t period):
    _dirFilter(NULL),
    _callback(NULL),
    _rain_callback(NULL),
    _windvane_pin(windvane_pin),
    _period(period),
    _anemometer_ticks(0),
    _anemometer_sum(0),
    _rain_ticks(0),
    _rain_sum(0),
    _dir(NAN),
    _timer_counter(0) {
    if (N > 0) {
        _dirFilter = new MovingAverageAngle <N>;
    }

#if defined(STM32_MCU_SERIES)

    if (windvane_pin > -1) {
        pinMode(windvane_pin, INPUT_ANALOG);
    }

#endif
}

template <uint8_t N>
WeatherMeters<N>::~WeatherMeters(void) {
    if (_dirFilter) {
        delete _dirFilter;
    }
}

template <uint8_t N>
void WeatherMeters<N>::attach(const WeaterMetersCallback callback) {
    _callback = callback;
}

template <uint8_t N>
void WeatherMeters<N>::attachRain(const WeaterMetersCallback callback) {
    _rain_callback = callback;
}

template <uint8_t N>
void WeatherMeters<N>::debug(HardwareSerial * serial) {
    _serial = serial;
}

template <uint8_t N>
float WeatherMeters<N>::adcToDir(uint16_t value) {
    uint16_t dir = 0;

    // Map ADC to degrees
    for (uint8_t i = 0; i < 16; i++) {
        if (value >= _windvane_table[15][1]) {
            // prevent overflow of index "i"
            dir = _windvane_table[15][0];
            break;

        } else if (value <= (_windvane_table[i][1] + ((_windvane_table[i + 1][1] - _windvane_table[i][1]) >> 1))) {
            // value can be up to half the difference to next
            dir = _windvane_table[i][0];
            break;
        }
    }

    if (_serial) {
        _serial->print(F("[WEATHER] Wind vane ADC:"));
        _serial->print(value);
        _serial->print(F(", raw dir: "));
        _serial->print(dir);
        _serial->print(F(", "));
    }

    if (_dirFilter) {
        float filtered_dir = _dirFilter->add(static_cast<float>(dir) / 10);

        filtered_dir = round(filtered_dir / 22.5) * 22.5;  // get 22.5° resolution

        if (filtered_dir >= 360.0) filtered_dir = 0;

        if (_serial) {
            _serial->print(F("filtered dir: "));
        }

        _dir = filtered_dir;

    } else {
        _dir = static_cast<float>(dir) / 10;

        if (_serial) {
            _serial->print(F("unfiltered dir: "));
        }
    }

    if (_serial) {
        _serial->println(_dir, 1);
    }

    return _dir;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getDir() {
    return _dir;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getSpeed() {
    float res = (static_cast<float>(_anemometer_sum) / static_cast<float>(_period)) * WIND_SPEED_RES;

    if (_period == 0) {
        res /= _timer_passed;
        _timer_passed = 0;
        _anemometer_sum = 0;
    }

    return res;
}

template <uint8_t N>
float WeatherMeters<N>::WeatherMeters::getRain() {
    float res = static_cast<float>(_rain_sum) * RAIN_GAUGE_RES;

    if (_period == 0) {
        _rain_sum = 0;
    }

    return res;
}

template <uint8_t N>
void WeatherMeters<N>::intAnemometer() {
    _anemometer_ticks++;
}

template <uint8_t N>
void WeatherMeters<N>::intRaingauge() {
    _rain_ticks++;

    if (_rain_callback) {
        _rain_callback();
    }
}

template <uint8_t N>
void WeatherMeters<N>::timer() {
    _timer_counter++;

    if (_windvane_pin > -1) {
        adcToDir(analogRead(_windvane_pin));
    }

    if (_period > 0) {
        if (_timer_counter == _period) {
            _timer_counter = 0;
            _rain_sum = _rain_ticks;
            _anemometer_sum = _anemometer_ticks;
            _rain_ticks = 0;
            _anemometer_ticks = 0;

            if (_callback) {
                _callback();
            }
        }

    } else {
        _rain_sum += _rain_ticks;
        _anemometer_sum += _anemometer_ticks;

        _timer_passed++;
        _rain_ticks = 0;
        _anemometer_ticks = 0;
    }
}

template <uint8_t N>
void WeatherMeters<N>::reset() {
    _rain_sum = 0;
    _rain_ticks = 0;
    _anemometer_sum = 0;
    _anemometer_ticks = 0;
    _timer_passed = 0;
    _dir = NAN;

    if (_dirFilter) {
        _dirFilter->reset();
    }
}

#endif  // WEATHERMETERS_H
