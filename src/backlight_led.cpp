//
// Created by bjo on 12/16/22.
//

#include "../include/backlight_led.hpp"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int r0=0,r1=0,r2=0;
int g0=0,g1=0,g2=0;
int b0=0,b1=0,b2=0;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, NEOPIXEL_PIN, NEO_GRB);


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
