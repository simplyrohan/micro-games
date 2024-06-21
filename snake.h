#include "display.h"
#include "input.h"

int snakeX[100];
int snakeY[100];
int snakeLength = 3;

int snakeDirection = 0;
// int snakeSpeed = 1;

int appleX = random(0, 160 / 8 - 1);
int appleY = random(0, 128 / 8 - 1);

bool inputed = false;

void setupSnake()
{
    fillScreen(ST7735_BLACK);
    setRotation(1);
    setTextSize(1);
    setTextColor(ST7735_WHITE);
    setCursor(0, 0);
    // println("Snake");
    memset(snakeX, 0, sizeof(snakeX));
    memset(snakeY, 0, sizeof(snakeY));

    snakeX[0] = 2;
    snakeY[0] = 2;
    snakeX[1] = 3;
    snakeY[1] = 2;
    snakeX[2] = 4;
    snakeY[2] = 2;
    snakeLength = 3;
}

void handleSnakeKeys()
{
    if (digitalRead(K_RIGHT) == LOW && snakeDirection != 1)
    {
        snakeDirection = 0;
        inputed = true;
    }
    else if (digitalRead(K_LEFT) == LOW && snakeDirection != 0)
    {
        snakeDirection = 1;
        inputed = true;
    }
    else if ((digitalRead(K_DOWN) == LOW) && snakeDirection != 3)
    {
        snakeDirection = 2;
        inputed = true;
    }
    else if ((digitalRead(K_UP) == LOW) && snakeDirection != 2)
    {
        snakeDirection = 3;
        inputed = true;
    }
}

void generateApple()
{
    appleX = random(0, 160 / 8 - 1);
    appleY = random(0, 128 / 8 - 1);
    for (int i = 0; i < snakeLength; i++)
    {
        if (snakeX[i] == appleX && snakeY[i] == appleY)
        {
            generateApple();
            break;
        }
    }
}

void loopSnake()
{
    handleSnakeKeys();
    // Logic
    fillRect(snakeX[0] * 8, snakeY[0] * 8, 7, 7, ST7735_BLACK);

    for (int i = 0; i < snakeLength - 1; i++)
    {
        snakeX[i] = snakeX[i + 1];
        snakeY[i] = snakeY[i + 1];
    }
    snakeX[snakeLength - 1] = snakeX[snakeLength - 2] + (snakeDirection == 0 ? 1 : snakeDirection == 1 ? -1
                                                                                                       : 0);
    snakeY[snakeLength - 1] = snakeY[snakeLength - 2] + (snakeDirection == 2 ? 1 : snakeDirection == 3 ? -1
                                                                                                       : 0);
    if (snakeX[snakeLength - 1] < 0)
        snakeX[snakeLength - 1] = 160 / 8 - 1;
    if (snakeX[snakeLength - 1] >= 160 / 8)
        snakeX[snakeLength - 1] = 0;
    if (snakeY[snakeLength - 1] < 0)
        snakeY[snakeLength - 1] = 128 / 8 - 1;
    if (snakeY[snakeLength - 1] >= 128 / 8)
        snakeY[snakeLength - 1] = 0;

    if (snakeX[snakeLength - 1] == appleX && snakeY[snakeLength - 1] == appleY)
    {
        snakeLength++;
        snakeX[snakeLength - 1] = snakeX[snakeLength - 2] + (snakeDirection == 0 ? 1 : snakeDirection == 1 ? -1
                                                                                                           : 0);
        snakeY[snakeLength - 1] = snakeY[snakeLength - 2] + (snakeDirection == 2 ? 1 : snakeDirection == 3 ? -1
                                                                                                           : 0);

        if (snakeX[snakeLength - 1] < 0)
            snakeX[snakeLength - 1] = 160 / 8 - 1;
        if (snakeX[snakeLength - 1] >= 160 / 8)
            snakeX[snakeLength - 1] = 0;
        if (snakeY[snakeLength - 1] < 0)
            snakeY[snakeLength - 1] = 128 / 8 - 1;
        if (snakeY[snakeLength - 1] >= 128 / 8)
            snakeY[snakeLength - 1] = 0;
        // appleX = random(0, 128 / 8 - 1);
        // appleY = random(0, 160 / 8 - 1);
        generateApple();
    }

    for (int i = 0; i < snakeLength - 2; i++)
    {
        if (snakeX[snakeLength - 1] == snakeX[i] && snakeY[snakeLength - 1] == snakeY[i])
        {
            fillScreen(ST7735_BLACK);
            setCursor(0, 0);
            println("Game Over");
            print("Your score: ");
            println(snakeLength);
            while (digitalRead(A0) == HIGH && digitalRead(A2) == HIGH && digitalRead(A3) == HIGH && digitalRead(A1) == HIGH)
            {
                delay(100);
            }
            setupSnake();
        }
    }

    // Draw
    for (int i = 0; i < snakeLength; i++)
    {
        fillRect(snakeX[i] * 8, snakeY[i] * 8, 7, 7, ST7735_GREEN);
    }
    fillRect(appleX * 8, appleY * 8, 7, 7, ST7735_RED);
    // delay(1000);
    // int start = millis();
    inputed = false;
    // while (millis() - start < 202)
    // {
        // if (!inputed)
        // {
            // handleSnakeKeys();
        // }
    // }
    for (int i = 0; i < 202; i++)
    {
        if (!inputed)
        {
            handleSnakeKeys();
        }
        delay(1);
    }
}