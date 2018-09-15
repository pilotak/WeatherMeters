#include "WeatherMeters.h"

const int windvane_pin = A0;
const int anemometer_pin = 2;
const int raingauge_pin = 3;

volatile bool got_data = false;

WeatherMeters <0> meters(windvane_pin, 8);  // no filtering saves RAM and computing, refresh data every 8 sec

void intAnemometer() {
    meters.intAnemometer();
}

void intRaingauge() {
    meters.intRaingauge();
}

void readDone(void) {
    got_data = true;
}

void setup() {
    Serial.begin(115200);

    attachInterrupt(digitalPinToInterrupt(anemometer_pin), intAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), intRaingauge, FALLING);

    // TIMER 1 for interrupt frequency 1000 Hz:
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    OCR1A = 15999; // 16MHz = 15999, 8Mhz = 7999
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    sei();

    meters.attach(readDone);
    meters.reset();  // in case we got already some interrupts
}

void loop() {
    if (got_data) {
        got_data = false;
        Serial.print(F("Wind degrees: "));
        Serial.print(meters.getDir(), 1);
        Serial.print(F(" Wind speed: "));
        Serial.print(meters.getSpeed(), 1);
        Serial.print(F("km/h, Rain: "));
        Serial.print(meters.getRain(), 4);
        Serial.println(F("mm"));
    }

    delay(1); // or do something else
}

ISR(TIMER1_COMPA_vect) {
    meters.timer();
}
