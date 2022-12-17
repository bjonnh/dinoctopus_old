//
// Created by bjo on 11/27/22.
//

#ifndef DINOCTOPUS_PIO_SENSORS_H
#define DINOCTOPUS_PIO_SENSORS_H


#include <Arduino.h>

enum SENSOR_TYPE { UNUSED=0, ANALOG=1, DIGITAL=2, TOF=3 };

class Sensor {
public:
    Sensor(bool enabled, int currentValue, SENSOR_TYPE type, int controller) : enabled(enabled), current_value(currentValue),
                                                                                type(type), controller(controller) {}
    Sensor() : enabled(false), current_value(-1), type(SENSOR_TYPE::UNUSED), controller(-1) {};

    int correctedValue() const {
        int val = map(current_value, minimum, maximum, 0, 1024);
        if (val<=0) return 0;
        if (val>=1023) return 1023;
        return val;
    }

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
    int controller;
    int channel = 16;

};


class SensorStatus {
public:
    Sensor sensors[16];
    bool soloStore[16];
    SensorStatus() {

    }

    void setup_sensor(int id, Sensor *sensor) {
        if ((id<0) | (id>15) | (sensor == nullptr)) return ;
        sensors[id] = *sensor;
    }

    // Store the sensors enabled status and only enable current one
    void solo(int id) {
        for (int i=0; i<16; i++) {
            soloStore[i] = sensors[i].enabled;
            sensors[i].enabled = (i == id);
        }
    }

    // Restore the sensors enabled status before the solo mode
    void exitSolo() {
        for (int i=0; i<16; i++)
            sensors[i].enabled = soloStore[i];
    }
};

#endif //DINOCTOPUS_PIO_SENSORS_H
