#include <Arduino.h>
#include <U8x8lib.h>

#include <MIDI.h>

//#define _TASK_SLEEP_ON_IDLE_RUN  // Enable 1 ms SLEEP_IDLE powerdowns between runs if no callback methods were invoked during the pass
//#define _TASK_STATUS_REQUEST     // Compile with support for StatusRequest functionality - triggering tasks on status change events in addition to time only
#define _TASK_MICRO_RES // microseconds resolution

#include <TaskScheduler.h>

#include "sensorstatus.hpp"
#include "menu.hpp"
#include "backlight_led.hpp"

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

SensorStatus sensorStatus = SensorStatus(&MIDI);

void midiCB() {
    sensorStatus.update();
}

Task midiOut(10L, -1, &midiCB, &scheduler, true); // 100us
Task updateDisplay(10 * TASK_MILLISECOND, -1, &menuCB, &scheduler, true);
Task updateLed(100 * TASK_MILLISECOND, -1, &setColors, &scheduler, true);


void setup()
{
    analogReference(DEFAULT);
    sensorStatus.setup();
    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setBusClock(400000);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0,0,"DINoctopus v0.01");
    MIDI.begin(MIDI_CHANNEL_OMNI);
    initStrip();
    menuInit(sensorStatus);
}

void loop()
{
   scheduler.execute();
}
