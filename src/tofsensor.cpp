//
// Created by bjo on 12/17/22.
//

#include "tofsensor.hpp"
#include <Wire.h>
#include <vl53l4cd_class.h>
VL53L4CD sensor_vl53l4cd_sat(&Wire, A1);


int TofSensor::callback() {
    uint8_t NewDataReady = 0;
    VL53L4CD_Result_t results;
    uint8_t status;

    status = sensor_vl53l4cd_sat.VL53L4CD_CheckForDataReady(&NewDataReady);

    if ((!status) && (NewDataReady != 0)) {
        sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
        sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);
        if (results.range_status==0)
            return results.distance_mm;
    }
    return 0;
}

void TofSensor::setup() {
    Wire.begin();
    sensor_vl53l4cd_sat.begin();
    sensor_vl53l4cd_sat.VL53L4CD_Off();
    sensor_vl53l4cd_sat.InitSensor();
    sensor_vl53l4cd_sat.VL53L4CD_SetRangeTiming(20, 0);
    sensor_vl53l4cd_sat.VL53L4CD_StartRanging();
}

void TofSensor::update() {
    int value = callback();
    if (value == 0)
        return;
    if (abs(current_value - value) < 3)
        return;
    current_value = value;
    midiOutput.update_value(correctedValue() >> 3);
}
