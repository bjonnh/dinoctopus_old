//
// Created by bjo on 11/25/22.
//

#include <Arduino.h>
#include <menu.h>

#include <menuIO/u8g2Out.h>
#include <menuIO/clickEncoderIn.h>

#include <TimerOne.h>

#include <U8g2lib.h>
#include "sensors.hpp"

#define MAX_DEPTH 3

#define fontX 5
#define fontY 9
#define offsetX 0
#define offsetY -1
#define U8_Width 128
#define U8_Height 64
#define FONT u8g2_font_5x8_mr

#define BTN_SEL 4    // Select button
#define BTN_UP 5 // Up
#define BTN_DOWN 6 // Down

U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd(U8G2_R0, 10, 11, 9, 8, 7);

const colorDef<uint8_t> colors[6] MEMMODE = {
        {{0, 0}, {0, 1, 1}},//bgColor
        {{1, 1}, {1, 0, 0}},//fgColor
        {{1, 1}, {1, 0, 0}},//valColor
        {{1, 1}, {1, 0, 0}},//unitColor
        {{0, 1}, {0, 0, 1}},//cursorColor
        {{1, 1}, {1, 0, 0}},//titleColor
};

SensorStatus *sensorStatusMenu;

int input = 0;

struct DisplayedInputData {
    bool enabled = false;
    SENSOR_TYPE type = SENSOR_TYPE::ANALOG;
    int value = 0;
    int controller = 0;
    int channel = 0;
    int pin = 54;
};

DisplayedInputData displayedInputData;

result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e, navNode &nav, prompt &item) {
    return proceed;
}

result setMinimum(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].setMinimum();
    return proceed;
}

result setMaximum(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].setMaximum();
    return proceed;
}

result setType(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].type = displayedInputData.type;
    return proceed;
}

result setEnabled(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].enabled = displayedInputData.enabled;
    return proceed;
}

result setChannel(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].channel = displayedInputData.channel;
    return proceed;
}

result setController(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].controller = displayedInputData.controller;
    return proceed;
}

result setPin(eventMask e, navNode &nav, prompt &item) {
    sensorStatusMenu->sensors[input].pin = displayedInputData.pin;
    return proceed;
}

void setCurrentControllerData() {
    displayedInputData.type = sensorStatusMenu->sensors[input].type;
    displayedInputData.enabled = sensorStatusMenu->sensors[input].enabled;
    displayedInputData.channel = sensorStatusMenu->sensors[input].channel;
    displayedInputData.controller = sensorStatusMenu->sensors[input].controller;
    displayedInputData.pin = sensorStatusMenu->sensors[input].pin;
}

result changeInput(eventMask e, navNode &nav, prompt &item) {
    setCurrentControllerData();
    return proceed;
}

SELECT(displayedInputData.pin, setPinMenu, "Pin: ", setPin, exitEvent, noStyle,
       VALUE("A0", 54, doNothing, noEvent), VALUE("A1", 55, doNothing, noEvent), VALUE("A2", 56, doNothing, noEvent),
       VALUE("A3", 57, doNothing, noEvent), VALUE("A4", 58, doNothing, noEvent), VALUE("A5", 59, doNothing, noEvent),
       VALUE("A6", 60, doNothing, noEvent), VALUE("A7", 61, doNothing, noEvent), VALUE("A8", 62, doNothing, noEvent),
       VALUE("A9", 63, doNothing, noEvent), VALUE("A10", 64, doNothing, noEvent), VALUE("A11", 65, doNothing, noEvent),
       VALUE("A12", 66, doNothing, noEvent), VALUE("A13", 67, doNothing, noEvent), VALUE("A14", 68, doNothing, noEvent),
       VALUE("A15", 69, doNothing, noEvent));


CHOOSE(displayedInputData.type, typeMenu, "Type", setType, exitEvent, noStyle,
       VALUE("Expression", SENSOR_TYPE::ANALOG, doNothing, noEvent),
       VALUE("On/Off", SENSOR_TYPE::DIGITAL, doNothing, noEvent),
       VALUE("TOF", SENSOR_TYPE::TOF, doNothing, noEvent)
);

TOGGLE(displayedInputData.enabled, toggleMenu, "Enabled: ", setEnabled, exitEvent, wrapStyle,
       VALUE("On", true, setEnabled, exitEvent),
       VALUE("Off", false, setEnabled, exitEvent))

MENU(controllersMenu, "Controllers", doNothing, noEvent, wrapStyle,
     FIELD(input, "Input", "", 0, 16, 1, 0, changeInput, enterEvent, wrapStyle),
     SUBMENU(toggleMenu),
     SUBMENU(typeMenu),
     FIELD(displayedInputData.value, "Value", "/1023", 0, 1024, 0, 0, doNothing, noEvent, wrapStyle),
     FIELD(displayedInputData.channel, "Channel", "", 0, 16, 1, 0, setChannel, enterEvent, wrapStyle),
     FIELD(displayedInputData.controller, "Controller", "", 0, 127, 1, 0, setController, enterEvent, wrapStyle),
     OP("Set Min", setMinimum, enterEvent),
     OP("Set Max", setMaximum, enterEvent),
     SUBMENU(setPinMenu),
     EXIT("<Back")
);

MENU(mainMenu, "DINoctopus v0.01", doNothing, noEvent, wrapStyle,
     SUBMENU(controllersMenu),
     EXIT("<Back")
);

MENU_OUTPUTS(out, MAX_DEPTH,
             U8G2_OUT(u8g2_lcd, colors, fontX, fontY, offsetX, offsetY, {0, 0, U8_Width / fontX, U8_Height / fontY}),
             NONE);

ClickEncoder clickEncoder(BTN_UP, BTN_DOWN, BTN_SEL, 4);
ClickEncoderStream encStream(clickEncoder, 1);

void timerIsr() { clickEncoder.service(); }

result alert(menuOut &o, idleEvent e) {
    if (e == idling) {
        o.setCursor(0, 0);
        o.print("alert test");
        o.setCursor(0, 1);
        o.print("[select] to continue...");
    }
    return proceed;
}

NAVROOT(nav, mainMenu, MAX_DEPTH, encStream, out);

result doAlert(eventMask e, prompt &item) {
    nav.idleOn(alert);
    return proceed;
}

result idle(menuOut &o, idleEvent e) {
    switch (e) {
        case idleStart:
            break;
        case idling:
            for (int i=0 ; i<=16;i++) {
                u8g2_lcd.setColorIndex(1);
                u8g2_lcd.drawFrame(8*i, 0, 8, 34);
                if (sensorStatusMenu->sensors[i].enabled) {
                    u8g2_lcd.setColorIndex(0);
                    u8g2_lcd.drawBox(8 * i + 1, 1, 6, 32);
                    u8g2_lcd.setColorIndex(1);
                    int v = sensorStatusMenu->sensors[i].correctedValue() / 32;
                    u8g2_lcd.drawBox(8 * i + 1, 33 - v, 6, v);
                }
            }
            nav.idleChanged = true;
            break;
        case idleEnd:
            break;
    }
    return proceed;
}

void menuInit(SensorStatus &sensorStatus) {
    sensorStatusMenu = &sensorStatus;

    u8g2_lcd.begin();
    u8g2_lcd.setContrast(180);
    u8g2_lcd.setFont(FONT);
    u8g2_lcd.setFontMode(1);

    //mainMenu[1].enabled = false;
    nav.idleTask = idle;
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    for (int i = 0; i < 100; i++) {
        digitalWrite(13, 255);
        delay(1);
        digitalWrite(13, 0);
        delay(1);
    }
    setCurrentControllerData();
}

void menuCB() {
    displayedInputData.value = sensorStatusMenu->sensors[input].correctedValue();

    nav.doInput();
    if (nav.changed(0)) {
        u8g2_lcd.firstPage();
        do nav.doOutput(); while (u8g2_lcd.nextPage());
    }
}
