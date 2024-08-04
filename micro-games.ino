#include "defines.h"
#include "hardware.h"
#include "launcher.h"

int gameRunning = -1;

void setup()
{
    Serial.begin(115200);

    // Serial.println("Starting hardware... BRO");
    setupHardware();

    // Serial.println("Hardware initialized and i hate you");

    setupLauncher(&tft);
}

void loop()
{
    get_keys();

    Serial.println("Looping...");
    if (gameRunning == -1)
    {
        gameRunning = loopLauncher(&tft, keys);
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
    delay(1000 / FPS);
}