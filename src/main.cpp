#include <Arduino.h>
#include <U8x8lib.h>

#include <MIDI.h>
using namespace midi;

//#define _TASK_SLEEP_ON_IDLE_RUN  // Enable 1 ms SLEEP_IDLE powerdowns between runs if no callback methods were invoked during the pass
//#define _TASK_STATUS_REQUEST     // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#define _TASK_MICRO_RES // microseconds resolution

#include <TaskScheduler.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <menu.h>
#include "menu.h"
#include "sensors.h"
#include "menu.hpp"


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

// someone needs a struct
int analog0 = 0;
int oldanalog0 = 0;
int channel0=16;
int controller0=79;


SensorStatus sensorStatus = SensorStatus();


void midiCB() {
    int temp;
    for (auto &sensor: sensorStatus.sensors)  {
        if (sensor.enabled) {
            temp = analogRead(sensor.pin);
            if (abs(sensor.previous_value - temp) > 3) {
                sensor.previous_value = temp;
                sensorStatus.sensors[0].current_value = temp;
                if (sensor.controller > 0) {
                    MIDI.sendControlChange(sensor.controller,
                                           sensor.correctedValue() >> 3,
                                           sensor.channel);
                }
            }
        }
    }

}
/*
void updateDisplayCB() {
    if (last_message.type==0) return;
    if (current_position >= 13) { memcpy(buffer2, buffer, sizeof(buffer));current_position = -1; }
    if ((current_position == -1) & newMessage) {
        newMessage = false;
        current_position = 0;
        snprintf_P(buffer, sizeof(buffer), PSTR("C%02x T%02x D%02x%02x"), last_message.channel, last_message.type, last_message.data1, last_message.data2);
    };
    if (current_position >=0) {
        while(buffer[current_position] == buffer2[current_position]) {
            current_position++;
            if ((current_position-1)>13) return;
        }
        if (buffer[current_position] != buffer2[current_position]) {
            u8x8.drawGlyph(current_position, 8, buffer[current_position]);
            current_position++;
        }

    }
}*/

Task midiIn(10L, -1, &midiCB, &scheduler, true); // 100us
Task updateDisplay(10 * TASK_MILLISECOND, -1, &menuCB, &scheduler, true);

/*void handleMessage(const midi::Message<128u> &message) {

}*/


void setup()
{
    analogReference(DEFAULT);
    Sensor sensor = Sensor(true, 0, -1, SENSOR_TYPE::ANALOG);
    sensor.controller = 79;
    sensorStatus.setup_sensor(0, &sensor);
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setBusClock(400000);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0,0,"DINoctopus v0.01");
    MIDI.begin(MIDI_CHANNEL_OMNI);

    menuInit(sensorStatus);
}


void loop()
{
   snprintf_P(buffer, sizeof(buffer), PSTR("%d"), sensorStatus.sensors[0].current_value);
   u8x8.drawString(0,1,buffer);
   scheduler.execute();
}
