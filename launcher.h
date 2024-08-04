#include <Adafruit_GFX.h>
#include "defines.h"
#include "hardware.h"
#include "games/snake.h"
#include "games/pong.h"

int selectionIndex = 0;

// A list of functions
void (*gameSetups[])(Adafruit_GFX *tft) = {
    &setupSnake,
    &setupPong,
};
int (*gameUpdates[])(Adafruit_GFX *tft) = {
    &updateSnake,
    &updatePong,
};
void (*gameKeys[])(bool keys[7]) = {
    &keysSnake,
    &keysPong,
};

String gameNames[] = {
    "Snake",
    "Pong",
};

int numberOfGames = 2;

int selectionCooldown = 0;

void setupLauncher()
{
    tft.fillScreen(BLACK);
    for (int i = 0; i < numberOfGames; i++)
    {
        tft.setCursor(20, i * 22);
        tft.setTextSize(2);
        if (i == selectionIndex)
        {
            tft.fillRect(0, i * 22, 240, 20, WHITE);
            tft.setTextColor(BLACK);
            tft.print(gameNames[i]);
        }
        else
        {
            tft.drawRect(0, i * 22, 240, 20, WHITE);
            tft.setTextColor(WHITE);
            tft.print(gameNames[i]);
        }
    }
}

int loopLauncher()
{
    if (keys[KEY_UP] && selectionCooldown == 0)
    {
        Serial.println("UP");

        selectionCooldown = 10;

        selectionIndex--;
        if (selectionIndex < 0)
        {
            selectionIndex = numberOfGames - 1;
        }
        setupLauncher();
    }
    if (keys[KEY_DOWN] && selectionCooldown == 0)
    {
        Serial.println("DOWN");

        selectionCooldown = 10;

        selectionIndex++;
        if (selectionIndex >= numberOfGames)
        {
            selectionIndex = 0;
        }
        setupLauncher();
    }

    if (keys[KEY_A])
    {
        gameSetups[selectionIndex](&tft);
        return selectionIndex;
    }

    selectionCooldown = std::max(0, selectionCooldown - 1);

    return -1;
}