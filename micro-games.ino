#include "defines.h"
#include "hardware.h"
#include "launcher.h"

int gameRunning = -1;

void setup()
{
    Serial.begin(115200);

    Serial.println("Starting and plz stop crashing");
    setupHardware();

    Serial.println("Hardware initialized");

    setupLauncher();
}

void loop()
{
    int frameStart = millis();
    get_keys();

    Serial.println("Looping...");
    if (gameRunning == -1)
    {
        gameRunning = loopLauncher();
    }
    else
    {
        gameUpdates[gameRunning](&tft);
    }

    // int start = millis();
    // while (millis() - start < 1000 / FPS)
    // {
    //     get_keys();
    //     gameKeys[gameRunning](keys);
    // }
    int frameEnd = millis();
    // delay(1000 / FPS - (frameEnd - frameStart));
    tft.setCursor(0, 0);
    tft.setTextSize(1.5);
    tft.setTextColor(WHITE, BLACK);
    tft.print(1000 / (frameEnd - frameStart));
    tft.print(" FPS");

}