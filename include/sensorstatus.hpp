//
// Created by bjo on 11/27/22.
//

#ifndef DINOCTOPUS_PIO_SENSORSTATUS_H
#define DINOCTOPUS_PIO_SENSORSTATUS_H


#include <Arduino.h>
#include "sensor.hpp"

class SensorStatus {
private:
    bool soloStore[16]{};
    MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr;
public:
    Sensor *sensors[16];

    void setup();

    // Store the sensors enabled status and only enable current one
    void solo(int id);

    // Restore the sensors enabled status before the solo mode
    void exitSolo();

    explicit SensorStatus(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface);

    void update();
};

#endif //DINOCTOPUS_PIO_SENSORS_H
