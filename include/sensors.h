//
// Created by bjo on 11/27/22.
//

#ifndef DINOCTOPUS_PIO_SENSORS_H
#define DINOCTOPUS_PIO_SENSORS_H


#include <Arduino.h>

enum SENSOR_TYPE { UNUSED, ANALOG, DIGITAL };

class Sensor {
public:
    Sensor(bool enabled, int id, int currentValue, SENSOR_TYPE type) : enabled(enabled), id(id),
                                                                       current_value(currentValue), type(type) {}
    Sensor() : enabled(false), id(-1), current_value(-1), type(SENSOR_TYPE::UNUSED) {};

    int correctedValue() {
        int val = map(current_value,minimum,maximum,0,1024);
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
    int id;
    int current_value;
    int previous_value = -1;
    int minimum = 0;
    int maximum = 1023;
    int pin = PIN_A0;
    int controller = -1;
    int channel = 16;
    SENSOR_TYPE type;
};


class SensorStatus {
public:
    int n_sensors=16;
    Sensor sensors[16];
    SensorStatus() {

    }

    void setup_sensor(int id, Sensor *sensor) {
        if ((id<0) | (id>15) | (sensor == nullptr)) return ;
        sensors[id] = *sensor;
    }
};

#endif //DINOCTOPUS_PIO_SENSORS_H
