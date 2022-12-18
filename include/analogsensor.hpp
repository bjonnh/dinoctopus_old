//
// Created by bjo on 12/17/22.
//

#ifndef CODE_ANALOGSENSOR_HPP
#define CODE_ANALOGSENSOR_HPP

#include <Arduino.h>
#include <sensor.hpp>

class AnalogSensor : public Sensor {
public:
    AnalogSensor(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr,
              bool enabled=false,
              int currentValue=0,
              byte controller=1,
              byte channel=16) : Sensor(midiInterface, enabled, currentValue, SENSOR_TYPE::ANALOG, controller, channel) {
        setup();
    }
    void setup();
    void update();
};

#endif //CODE_ANALOGSENSOR_HPP
