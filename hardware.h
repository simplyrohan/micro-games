#ifndef HARDWARE_H
#define HARDWARE_H

#include "Adafruit_GFX.h"
#include "Adafruit_ST7789.h"
#include "defines.h"

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

bool keys[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setupHardware()
{
    pinMode(SHIFT_CLOCK, OUTPUT);
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    pinMode(SHIFT_INP, INPUT);

    // spi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

    pinMode(TFT_CS, OUTPUT);

    tft.init(135, 240);
    tft.setSPISpeed(80000000);
    tft.setRotation(1);

    tft.fillScreen(BLACK);
}

inline void registerWrite(int whichPin, int whichState)
{
    // the bits you want to send
    byte bitsToSend = 0;
    // turn off the output so the pins don't light up
    // while you're shifting bits:
    digitalWrite(SHIFT_LATCH, LOW);
    // turn on the next highest bit in bitsToSend:
    bitWrite(bitsToSend, whichPin, whichState);
    // shift the bits out:
    shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, bitsToSend);
    // turn on the output so the LEDs can light up:
    digitalWrite(SHIFT_LATCH, HIGH);
}

void get_keys()
{
    for (int i = 0; i < 7; i++)
    {
        if (i > 2)
        {
            registerWrite(i + 1, 1);
            delay(1);
            keys[i + 1] = digitalRead(SHIFT_INP);
        }
        else
        {
            registerWrite(i, 1);
            delay(1);
            keys[i] = digitalRead(SHIFT_INP);
        }
    }
}

#endif