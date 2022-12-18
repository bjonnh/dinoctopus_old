//
// Created by bjo on 11/27/22.
//

#include "sensor.hpp"

int Sensor::correctedValue() const {
    int val = map(current_value, minimum, maximum, 0, 1024);
    if (val<=0) return 0;
    if (val>=1023) return 1023;
    return val;
}

void Sensor::set_channel(byte channel) {
    midiOutput.set_channel(channel);
}

void Sensor::set_controller(byte channel) {
    midiOutput.set_controller(channel);
}

byte Sensor::get_channel() const {
    return midiOutput.channel;
}

byte Sensor::get_controller() const {
    return midiOutput.controller;
}
