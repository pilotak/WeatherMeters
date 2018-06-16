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

#include "Arduino.h"
#include "WeatherMeters.h"

#if ADC_RESOLUTION == 4096 || defined(STM32_MCU_SERIES) || defined(ARDUINO_ARCH_ESP32)
const static uint16_t windvane_table[16][2] = {
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

#elif ADC_RESOLUTION == 1024 || defined(__AVR__)
const static uint16_t windvane_table[16][2] = {
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
    {0,    786},
    {2925, 828},
    {3150, 887},
    {2700, 945}
};
#endif

WeatherMeters::WeatherMeters(int windvane_pin, uint32_t samples):
    _windvane_pin(windvane_pin),
    _samples(samples),
    _callback(NULL),
    _rain_ticks(0),
    _rain_sum(0),
    _anemometer_ticks(0),
    _anemometer_sum(0),
    _anemometer_samples_passed(0),
    _windvane_degree_sum(0),
    _windvane_result(0),
    _windwane_samples_passed(0),
    _second_counter(0) {
#if defined(STM32_MCU_SERIES)
    pinMode(windvane_pin, INPUT_ANALOG);
#endif
}

void WeatherMeters::secondCount() {
    _second_counter++;

    readWindVane();

    if (_second_counter == _samples) {
        _second_counter = 0;
        update();
    }
}

float WeatherMeters::getWindSpeed() {
    float res = (static_cast<float>(_anemometer_sum) / _samples) * WIND_SPEED_RES;

    if (!_callback) {
        res /= _anemometer_samples_passed;
        _anemometer_samples_passed = 0;
        _anemometer_sum = 0;
    }

    return res;
}

float WeatherMeters::getWindVane() {
    uint32_t multiplier = ((_windvane_result / _samples) + 112) / 225;  // round up
    float res = static_cast<float>(multiplier) * 22.5;  // get 22.5° resolution

    if (!_callback) {
        res /= _windwane_samples_passed;
        _windwane_samples_passed = 0;
        _windvane_result = 0;
    }

    return res;
}


float WeatherMeters::getRainGauge() {
    float res = static_cast<float>(_rain_sum) * RAIN_GAUGE_RES;

    if (!_callback) {
        _rain_sum = 0;
    }

    return res;
}

void WeatherMeters::intAnemometer() {
    _anemometer_ticks++;
}

void WeatherMeters::intRaingauge() {
    _rain_ticks++;
}

void WeatherMeters::update() {
    if (_callback) {
        _windvane_result = _windvane_degree_sum;
        _rain_sum = _rain_ticks;
        _anemometer_sum = _anemometer_ticks;

        _callback();

    } else {
        _windvane_result += _windvane_degree_sum;
        _rain_sum += _rain_ticks;
        _anemometer_sum += _anemometer_ticks;

        _windwane_samples_passed++;
        _anemometer_samples_passed++;
    }

    _rain_ticks = 0;
    _anemometer_ticks = 0;
    _windvane_degree_sum = 0;
}

void WeatherMeters::readWindVane() {
    uint16_t dir = 0;
    uint16_t value = analogRead(_windvane_pin);

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
        _serial->print("adc:");
        _serial->print(value);
        _serial->print(", dir: ");
        _serial->println(static_cast<float>(dir) / 10, 1);
    }

    _windvane_degree_sum += dir;
}

void WeatherMeters::attach(void (*callback)(void)) {
    _callback = callback;
}

void WeatherMeters::init(HardwareSerial * serial) {
    _serial = serial;
}
