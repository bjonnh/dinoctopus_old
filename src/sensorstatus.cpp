//
// Created by bjo on 12/17/22.
//

#include "sensor.hpp"
#include "sensorstatus.hpp"
#include "tofsensor.hpp"
#include "analogsensor.hpp"

void SensorStatus::exitSolo() {
    for (int i=0; i<16; i++)
        sensors[i]->enabled = soloStore[i];
}

void SensorStatus::solo(int id) {
    for (int i=0; i<16; i++) {
        soloStore[i] = sensors[i]->enabled;
        sensors[i]-> enabled = (i == id);
    }
}

SensorStatus::SensorStatus(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface) : midiInterface(midiInterface) {

}

void SensorStatus::setup() {
    AnalogSensor ana_sens = AnalogSensor(midiInterface, true, 0, 80);
    sensors[0] = &ana_sens;
    TofSensor sensor_tof = TofSensor(midiInterface, true, 0, 81);
    sensors[1] = &sensor_tof;
    for (int i=2;i<16;i++) {
        AnalogSensor sensor_sub = AnalogSensor(midiInterface, false, 0,  81+i);
        sensors[i] = &sensor_sub;
    }
}

void SensorStatus::update() {
    for (auto &sensor: sensors)  {
        if (sensor->enabled)
            sensor->update();
    }
}
