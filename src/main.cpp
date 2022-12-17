#include <Arduino.h>
#include <U8x8lib.h>

#include <MIDI.h>
using namespace midi;

//#define _TASK_SLEEP_ON_IDLE_RUN  // Enable 1 ms SLEEP_IDLE powerdowns between runs if no callback methods were invoked during the pass
//#define _TASK_STATUS_REQUEST     // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#define _TASK_MICRO_RES // microseconds resolution

#include <TaskScheduler.h>

#include <Wire.h>

#include "sensors.hpp"
#include "menu.hpp"
#include "backlight_led.hpp"

#include <vl53l4cd_class.h>
VL53L4CD sensor_vl53l4cd_sat(&Wire, A1);

// lcd pin serial
// e 10 clk
// rw 11 SDA/
// rs 9  CS / (register select)

// mega
//Arduino Mega SPI pins: 50 (MISO), 51 (MOSI), 52 (SCK), 53 (SS).

// ENC btn 7
// rot1: 6
// rot2: 5


Scheduler scheduler;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

int counter = 0;
char buffer[32];

SensorStatus sensorStatus = SensorStatus();

uint16_t tofCB()
{
    uint8_t NewDataReady = 0;
    VL53L4CD_Result_t results;
    uint8_t status;

    status = sensor_vl53l4cd_sat.VL53L4CD_CheckForDataReady(&NewDataReady);

    if ((!status) && (NewDataReady != 0)) {
        sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
        sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);
        if (results.range_status==0) {
            return results.distance_mm;
        }
    }
    return 0;
}


void midiCB() {
    int temp;
    for (auto &sensor: sensorStatus.sensors)  {
        if (sensor.enabled) {
            if(sensor.type == SENSOR_TYPE::ANALOG) {
                temp = analogRead(sensor.pin);
                if (abs(sensor.current_value - temp) > 3) {
                    sensor.current_value = temp;
                    if (sensor.controller > 0) {
                        MIDI.sendControlChange(sensor.controller,
                                               sensor.correctedValue() >> 3,
                                               sensor.channel);
                    }
                }
            } else if (sensor.type == SENSOR_TYPE::TOF) {
                temp = tofCB();
                if (temp > 0) { // We should never have 0 except at init, so we use that the "no response" value
                    if (abs(sensor.current_value - temp) > 3) {
                        sensor.current_value = temp;
                        if (sensor.controller > 0) {
//                            snprintf_P(buffer, sizeof(buffer), PSTR("%5u"),temp);
//                            u8x8.drawString(0,1,buffer);
                            MIDI.sendControlChange(sensor.controller,
                                                   sensor.correctedValue() >> 3,
                                                   sensor.channel);
                        }
                    }
                }
            }
        }
    }
}

Task midiIn(10L, -1, &midiCB, &scheduler, true); // 100us
Task updateDisplay(10 * TASK_MILLISECOND, -1, &menuCB, &scheduler, true);
Task updateLed(10 * TASK_MILLISECOND, -1, &setColors, &scheduler, true);

void distancesetup() {
    Wire.begin();
    sensor_vl53l4cd_sat.begin();
    sensor_vl53l4cd_sat.VL53L4CD_Off();
    sensor_vl53l4cd_sat.InitSensor();
    sensor_vl53l4cd_sat.VL53L4CD_SetRangeTiming(20, 0);
    sensor_vl53l4cd_sat.VL53L4CD_StartRanging();
}

void setup()
{
    analogReference(DEFAULT);
    Sensor sensor = Sensor(true, 0, SENSOR_TYPE::ANALOG, 80);
    sensorStatus.setup_sensor(0, &sensor);
    Sensor sensor_tof = Sensor(true, 0, SENSOR_TYPE::TOF, 81);
    sensorStatus.setup_sensor(1, &sensor_tof);
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setBusClock(400000);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0,0,"DINoctopus v0.01");
    MIDI.begin(MIDI_CHANNEL_OMNI);
    initStrip();
    menuInit(sensorStatus);
    distancesetup();
}

void loop()
{
   scheduler.execute();
}
