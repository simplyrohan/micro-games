// Game manager and launcher
#include "display.h"
#include "input.h"

// #include "tetris.h"
#include "flappy.h"
#include "snake.h"


#define FPS 10 // Game wide FPS

int lastFrame = 0;

struct Game
{
    char *name;
    void (*setup)();
    void (*loop)();
};

Game games[] = {
    // {"Tetris", setupTetris, loopTetris},
    {"Snake", setupSnake, loopSnake},
    {"Flappy", setupFlappy, loopFlappy}
};
int selectionInd = 0;

int game = -1;

bool inputted = false;

void setup()
{
    inputSetup();
    setupScreen();
    setRotation(1);

    for (int i = 0; i < sizeof(games) / sizeof(games[0]); i++)
    {
        drawString(0, i * 8, games[i].name, COLOR_WHITE, COLOR_BLACK);
    }

    drawRect(0, selectionInd * 8, 128, 8, COLOR_WHITE);
}

void loop()
{
    if (game == -1)
    {
        // Launcher
        while (millis() - lastFrame < 1000 / FPS)
        {
            inputted = false;
            if (!inputted)
            {
                if (digitalRead(K_UP) == LOW)
                {
                    drawRect(0, selectionInd * 8, 128, 8, COLOR_BLACK);
                    selectionInd = (selectionInd - 1 + sizeof(games) / sizeof(games[0])) % (sizeof(games) / sizeof(games[0]));
                    drawRect(0, selectionInd * 8, 128, 8, COLOR_WHITE);
                    inputted = true;
                    delay(200);
                }
                if (digitalRead(K_DOWN) == LOW)
                {
                    drawRect(0, selectionInd * 8, 128, 8, COLOR_BLACK);
                    selectionInd = (selectionInd + 1) % (sizeof(games) / sizeof(games[0]));
                    drawRect(0, selectionInd * 8, 128, 8, COLOR_WHITE);
                    inputted = true;
                    delay(200);
                }
                if (digitalRead(K_A) == LOW)
                {
                    game = selectionInd;
                    fillScreen(COLOR_BLACK);
                    games[game].setup();

                    inputted = true;
                }
            }
            delay(10);
        }
        lastFrame = millis();
    }
    else
    {
        // Game
        games[game].loop();
    }
}