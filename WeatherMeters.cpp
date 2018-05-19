#include "Arduino.h"
#include "WeatherMeters.h"

#if ADC_RESOLUTION == 4096
const static uint16_t windvane_table[16][2] = {
    {1125, 264},
    {675,  335},   // 71
    {900,  372},   // 37
    {1575, 506},   // 134
    {1350, 739},   // 233
    {2025, 979},   // 240
    {1800, 1149},  // 170
    {225,  1624},  // 475
    {450,  1845},  // 221
    {2475, 2398},  // 553
    {2250, 2521},  // 123
    {3375, 2811},  // 290
    {0,    3143},  // 332
    {2925, 3310},  // 167
    {3150, 3549},  // 239
    {2700, 3781}   // 232
};

#elif ADC_RESOLUTION == 1024
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

WeatherMeters::WeatherMeters(int windvane_pin):
    _windvane_pin(windvane_pin),
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
    pinMode(windvane_pin, INPUT_ANALOG);
}

void WeatherMeters::secondCount() {
    _second_counter++;

    readWindVane();

    if (_second_counter == SAMPLES) {
        _second_counter = 0;
        update();
    }
}

float WeatherMeters::getWindSpeed() {
    float res = (static_cast<float>(_anemometer_sum) / SAMPLES) * WIND_SPEED_RES;

    if (!_callback) {
        res /= _anemometer_samples_passed;
        _anemometer_samples_passed = 0;
        _anemometer_sum = 0;
    }

    return res;
}

float WeatherMeters::getWindVane() {
    uint32_t multiplier = ((_windvane_result / SAMPLES) + 112) / 225;  // round up
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
