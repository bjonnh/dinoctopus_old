#include <Arduino.h>
#include <U8x8lib.h>
#include <U8g2lib.h>
#include <TimerOne.h>

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

// lcd pin serial
// e 10 SCL
// rw 8 SDA
// rs 9  CS

// mega
//Arduino Mega SPI pins: 50 (MISO), 51 (MOSI), 52 (SCK), 53 (SS).

// ENC btn 7
// rot1: 6
// rot2: 5

#include <menu.h>
#include <menuIO/u8g2Out.h>
#include <menuIO/encoderIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/clickEncoderIn.h>
//#include <menuIO/keyIn.h>
#include <menuIO/altKeyIn.h>

U8G2_ST7920_128X64_F_SW_SPI u8g2_lcd(U8G2_R0, 10,11,9);


Scheduler scheduler;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

struct MidiMessage {
    uint8_t type;
    uint8_t data1;
    uint8_t data2;
    uint8_t channel;

    MidiMessage(uint8_t type, uint8_t data1, uint8_t data2, uint8_t channel) :
            type(type), data1(data1), data2(data2), channel(channel) {}
};

MidiMessage last_message(0,0,0,0);

void midiConverter(MidiMessage msg) {
   MIDI.send(msg.type, msg.data1, msg.data2, msg.channel);
}

int counter = 0;
char buffer[32];
char buffer2[32];
int current_position = -1;
bool newMessage = false;


void midiCB() {
    while(MIDI.read()) {
        if (MIDI.check()) {
            last_message.type = MIDI.getType();
            last_message.data1 = MIDI.getData1();
            last_message.data2 = MIDI.getData2();
            last_message.channel = MIDI.getChannel();
            newMessage = true;
            midiConverter(MidiMessage(last_message.type, last_message.data1, last_message.data2, last_message.channel));
        }
    }
}

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
}

Task midiIn(10L, -1, &midiCB, &scheduler, true); // 100us
Task updateDisplay(10 * TASK_MILLISECOND, -1, &updateDisplayCB, &scheduler, true);

/*void handleMessage(const midi::Message<128u> &message) {

}*/

const colorDef<uint8_t> colors[6] MEMMODE={
        {{0,0},{0,1,1}},//bgColor
        {{1,1},{1,0,0}},//fgColor
        {{1,1},{1,0,0}},//valColor
        {{1,1},{1,0,0}},//unitColor
        {{0,1},{0,0,1}},//cursorColor
        {{1,1},{1,0,0}},//titleColor
};
//MENU_INPUTS(in,&keyIn);

#define fontX 7
#define fontY 12
#define offsetX 0
#define offsetY 2
#define U8_Width 128
#define U8_Height 64


#define BTN_SEL 7	// Select button
#define BTN_UP 6 // Up
#define BTN_DOWN 5 // Down


result doAlert(eventMask e, prompt &item);

result showEvent(eventMask e,navNode& nav,prompt& item) {
    u8x8.println("show event");
    return proceed;
}

int analog0=55;

result action1(eventMask e,navNode& nav, prompt &item) {
    u8x8.println("action 1");
    return proceed;
}

result action2(eventMask e,navNode& nav, prompt &item) {
    u8x8.println("action 2");
    return quit;
}

int ledCtrl=LOW;

result myLedOn() {
    ledCtrl=HIGH;
    return proceed;
}
result myLedOff() {
    ledCtrl=LOW;
    return proceed;
}

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
,VALUE("On",HIGH,doNothing,noEvent)
,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
,VALUE("Zero",0,doNothing,noEvent)
,VALUE("One",1,doNothing,noEvent)
,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
,VALUE("First",1,doNothing,noEvent)
,VALUE("Second",2,doNothing,noEvent)
,VALUE("Third",3,doNothing,noEvent)
,VALUE("Last",-1,doNothing,noEvent)
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

MENU(subMenu,"Sub-Menu",showEvent,anyEvent,noStyle
,OP("Sub1",showEvent,anyEvent)
,OP("Sub2",showEvent,anyEvent)
,OP("Sub3",showEvent,anyEvent)
,altOP(altPrompt,"",showEvent,anyEvent)
,EXIT("<Back")
);

/*extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);*/

char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
,OP("OpA",action1,anyEvent)
,OP("OpB",action2,enterEvent)
//,SUBMENU(togOp)
,FIELD(analog0,"Pedal","/1024",0,1024,10,1,doNothing,noEvent,wrapStyle)
,SUBMENU(subMenu)
,SUBMENU(setLed)
,OP("LED On",myLedOn,enterEvent)
,OP("LED Off",myLedOff,enterEvent)
,SUBMENU(selMenu)
,SUBMENU(chooseMenu)
,OP("Alert test",doAlert,enterEvent)
,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
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

ClickEncoder clickEncoder(BTN_UP, BTN_DOWN,BTN_SEL,2);
ClickEncoderStream encStream(clickEncoder,1);
void timerIsr() {clickEncoder.service();}

NAVROOT(nav,mainMenu,MAX_DEPTH, encStream,out);//the navigation root object

result alert(menuOut& o,idleEvent e) {
    if (e==idling) {
        o.setCursor(0,0);
        o.print("alert test");
        o.setCursor(0,1);
        o.print("[select] to continue...");
    }
    return proceed;
}

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

void setup()
{
    u8g2_lcd.begin();
    u8g2_lcd.setFont(u8g2_font_6x12_tr);
    u8g2_lcd.setFontMode(0);
    u8g2_lcd.clearBuffer();
    u8g2_lcd.drawHLine(0,58,128);
    u8g2_lcd.drawStr(50, 64, "CYCLE");
    u8g2_lcd.sendBuffer();



    u8x8.begin();
    u8x8.setPowerSave(0);
    u8x8.setBusClock(400000);
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0,0,"DINoctopus v0.01");
    MIDI.begin(MIDI_CHANNEL_OMNI);

    mainMenu[1].enabled=disabledStatus;
    nav.idleTask=idle;//point a function to be used when menu is suspended
    pinMode(BTN_SEL, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
}

void loop()
{
    analog0 = analogRead(PIN_A0);
    nav.doInput();
    if (nav.changed(0)) {//only draw if menu changed for gfx device
        //change checking leaves more time for other tasks
        u8g2_lcd.firstPage();
        do nav.doOutput(); while(u8g2_lcd.nextPage());
  }
    delay(100);//simulate other tasks delay
     //simulate other tasks delay
   // scheduler.execute();
}
