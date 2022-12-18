//
// Created by bjo on 12/17/22.
//

#ifndef CODE_TOFSENSOR_HPP
#define CODE_TOFSENSOR_HPP

#include <Arduino.h>
#include <sensor.hpp>

class TofSensor : public Sensor {
public:
    TofSensor(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr,
              bool enabled=false,
              int currentValue=0,
              byte controller=1,
              byte channel=16) : Sensor(midiInterface, enabled, currentValue, SENSOR_TYPE::TOF, controller, channel) {
        setup();
    }
    static int callback();
    void setup();
    void update();
};


#endif //CODE_TOF_HPP
