//
// Created by bjo on 11/25/22.
//

#include <Arduino.h>
#include <menu.h>

#include <menuIO/u8g2Out.h>
#include <menuIO/clickEncoderIn.h>

#include <TimerOne.h>

#include <U8g2lib.h>
#include "sensors.h"


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NEOPIXEL_PIN 3

#define fontX 7
#define fontY 12
#define offsetX 0
#define offsetY 0
#define U8_Width 128
#define U8_Height 64


#define BTN_SEL 4	// Select button
#define BTN_UP 5 // Up
#define BTN_DOWN 6 // Down



Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_GRB);



U8G2_ST7567_JLX12864_1_4W_SW_SPI u8g2_lcd(U8G2_R0, 10,11,9,8, 7);
//U8G2_ST7567_128X64_F_SW_SPI u8g2_lcd(U8G2_R0, 10,11,9);


const colorDef<uint8_t> colors[6] MEMMODE={
        {{0,0},{0,1,1}},//bgColor
        {{1,1},{1,0,0}},//fgColor
        {{1,1},{1,0,0}},//valColor
        {{1,1},{1,0,0}},//unitColor
        {{0,1},{0,0,1}},//cursorColor
        {{1,1},{1,0,0}},//titleColor
};

SensorStatus *sensorStatusMenu;

int input = 0;


struct DisplayedInputData {
    int value = 0;
    int controller = 0;
    int channel = 0;
    int pin = 54;
};

DisplayedInputData displayedInputData;



result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e,navNode& nav,prompt& item) {
    return proceed;
}

result setMinimum(eventMask e,navNode& nav, prompt &item) {
    sensorStatusMenu->sensors[input].setMinimum();
    return proceed;
}

result setMaximum(eventMask e,navNode& nav, prompt &item) {
    sensorStatusMenu->sensors[input].setMaximum();
    return proceed;
}


result setChannel(eventMask e,navNode& nav, prompt &item) {
    sensorStatusMenu->sensors[input].channel = displayedInputData.channel;
    return proceed;
}

result setController(eventMask e,navNode& nav, prompt &item) {
    sensorStatusMenu->sensors[input].controller = displayedInputData.controller;
    return proceed;
}

result setPin(eventMask e,navNode& nav, prompt &item) {
    sensorStatusMenu->sensors[input].pin = displayedInputData.pin;
    return proceed;
}

void setCurrentControllerData() {
    displayedInputData.channel = sensorStatusMenu->sensors[input].channel;
    displayedInputData.controller = sensorStatusMenu->sensors[input].controller;
    displayedInputData.pin = sensorStatusMenu->sensors[input].pin;
}

result changeInput(eventMask e,navNode& nav, prompt &item) {
    setCurrentControllerData();
}

SELECT(displayedInputData.pin,setPinMenu,"Pin: ",setPin,exitEvent,noStyle//,doExit,enterEvent,noStyle
,VALUE("A0",54,doNothing,noEvent)
,VALUE("A1",55,doNothing,noEvent)
,VALUE("A2",56,doNothing,noEvent)
,VALUE("A3",57,doNothing,noEvent)
,VALUE("A4",58,doNothing,noEvent)
,VALUE("A5",59,doNothing,noEvent)
,VALUE("A6",60,doNothing,noEvent)
,VALUE("A7",61,doNothing,noEvent)
,VALUE("A8",62,doNothing,noEvent)
,VALUE("A9",63,doNothing,noEvent)
,VALUE("A10",64,doNothing,noEvent)
,VALUE("A11",65,doNothing,noEvent)
,VALUE("A12",66,doNothing,noEvent)
,VALUE("A13",67,doNothing,noEvent)
,VALUE("A14",68,doNothing,noEvent)
,VALUE("A15",69,doNothing,noEvent)
);

int chooseTest=1;
CHOOSE(chooseTest,modeMenu,"Mode",doNothing,noEvent,noStyle
,VALUE("Expression",1,doNothing,noEvent)
,VALUE("On/Off",2,doNothing,noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
    altPrompt(constMEM promptShadow& p):prompt(p) {}
    Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
        return out.printRaw(F("special prompt!"),len);;
    }
};

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"DINoctopus v0.01",doNothing,noEvent,wrapStyle
,FIELD(input,"Input","",0,16,1,0,changeInput,enterEvent,wrapStyle)
,SUBMENU(modeMenu)
,FIELD(displayedInputData.value,"Value","/1023",0,1024,0,0,doNothing,noEvent,wrapStyle)
,FIELD(displayedInputData.channel,"Channel","",0,16,1,0,setChannel, enterEvent,wrapStyle)
,FIELD(displayedInputData.controller,"Controller","",0,127,1,0,setController, enterEvent,wrapStyle)
,OP("Set Min",setMinimum, enterEvent)
,OP("Set Max",setMaximum, enterEvent)
,SUBMENU(setPinMenu)
,EXIT("<Back")
);

#define MAX_DEPTH 2

/*const panel panels[] MEMMODE={{0,0,16,2}};
navNode* nodes[sizeof(panels)/sizeof(panel)];
panelsList pList(panels,nodes,1);
idx_t tops[MAX_DEPTH];
lcdOut outLCD(&lcd,tops,pList);//output device for LCD
menuOut* constMEM outputs[] MEMMODE={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs
*/

MENU_OUTPUTS(out,MAX_DEPTH
,U8G2_OUT(u8g2_lcd,colors,fontX,fontY,offsetX,offsetY,{0,0,U8_Width/fontX,U8_Height/fontY})
,NONE
);

ClickEncoder clickEncoder(BTN_UP, BTN_DOWN,BTN_SEL,4);
ClickEncoderStream encStream(clickEncoder,1);
void timerIsr() {clickEncoder.service();}

result alert(menuOut& o,idleEvent e) {
    if (e==idling) {
        o.setCursor(0,0);
        o.print("alert test");
        o.setCursor(0,1);
        o.print("[select] to continue...");
    }
    return proceed;
}


NAVROOT(nav,mainMenu,MAX_DEPTH, encStream,out);//the navigation root object


result doAlert(eventMask e, prompt &item) {
    nav.idleOn(alert);
    return proceed;
}

result idle(menuOut& o,idleEvent e) {
    switch(e) {
        case idleStart:o.print("suspending menu!");break;
        case idling:o.print("suspended...");break;
        case idleEnd:o.print("resuming menu.");break;
    }
    return proceed;
}

int r0=0,r1=0,r2=0;
int g0=0,g1=0,g2=0;
int b0=0,b1=0,b2=0;

void setColors() {
    r0+=5;
    if (r0>55) { r1+=5; r0=25; }
    if (r1>55) { g0+=5 ; r1=25; }
    if (g0>55) { g1+=5; g0=25; }
    if (g1>55) { b0+=5; g1=25; }
    if (b0>55) { b1+=5; b0=25; }
    if (b1>55) { b1=25; }
    strip.clear();
    strip.setPixelColor(0, strip.Color(g0, r0, b0));
    strip.setPixelColor(1, strip.Color(g1, r1, b1));
    strip.setPixelColor(2, strip.Color(155, 255, 150));
    strip.show();
}

void initStrip() {
    strip.begin();
    strip.setBrightness(255);
    strip.setPixelColor(0, strip.Color(15, 25, 0*25));
    strip.setPixelColor(1, strip.Color(15, 25, 0*25));
    strip.setPixelColor(2, strip.Color(155, 255, 0*255));
    strip.show();
}



void menuInit(SensorStatus &sensorStatus) {
    sensorStatusMenu = &sensorStatus;

    initStrip();

    u8g2_lcd.begin();
    u8g2_lcd.setContrast(180);
    u8g2_lcd.setFont(u8g2_font_6x12_tr);
    u8g2_lcd.setFontMode(0);

    mainMenu[1].enabled=disabledStatus;
    nav.idleTask=idle;
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    for (int i=0;i<100;i++) {
        digitalWrite(13,255);
        delay(1);
        digitalWrite(13,0);
        delay(1);
    }
    setCurrentControllerData();
}


void menuCB() {
    setColors();
    displayedInputData.value = sensorStatusMenu->sensors[input].correctedValue();

    nav.doInput();
    if (nav.changed(0)) {
        u8g2_lcd.firstPage();
        do nav.doOutput(); while(u8g2_lcd.nextPage());
    }
}
