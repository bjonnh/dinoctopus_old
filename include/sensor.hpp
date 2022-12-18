//
// Created by bjo on 11/27/22.
//

#ifndef DINOCTOPUS_PIO_SENSORS_H
#define DINOCTOPUS_PIO_SENSORS_H

#include <Arduino.h>
#include "midioutput.hpp"

enum SENSOR_TYPE { UNUSED=0, ANALOG=1, DIGITAL=2, TOF=3 };

class Sensor {
protected:
    MidiOutput midiOutput;
    MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr;
public:
    explicit Sensor(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr,
                    bool enabled=false,
                    int currentValue=0,
                    SENSOR_TYPE type=SENSOR_TYPE::ANALOG,
                    byte controller=1,
                    byte channel=16) : midiOutput(midiInterface, controller, channel), midiInterface(midiInterface),
                    enabled(enabled), current_value(currentValue), type(type) {}

    int correctedValue() const;


    void setMinimum() {
        minimum=current_value;
    }

    void setMaximum() {
        maximum=current_value;
    }

    bool enabled;
    int current_value;
    int minimum = 0;
    int maximum = 1023;
    int pin = PIN_A0;
    SENSOR_TYPE type;

    void set_channel(byte channel);
    void set_controller(byte controller);

    virtual void update() = 0;

    byte get_channel() const;
    byte get_controller() const;
};

#endif //DINOCTOPUS_PIO_SENSORS_H
