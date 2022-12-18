//
// Created by bjo on 12/17/22.
//

#include "analogsensor.hpp"

void AnalogSensor::update() {
    int value = analogRead(pin);
    if (abs(current_value - value) < 3)
        return;
    current_value = value;
    midiOutput.update_value(correctedValue() >> 3);
}

void AnalogSensor::setup() {

}
