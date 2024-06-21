#ifndef INPUT_H
#define INPUT_H

#define K_LEFT 2
#define K_RIGHT 4
#define K_UP 5
#define K_DOWN 3
#define K_A 6

#define PULLUP
// #define NORMAL
void inputSetup()
{
#ifdef PULLUP
    pinMode(K_LEFT, INPUT_PULLUP);
    pinMode(K_RIGHT, INPUT_PULLUP);
    pinMode(K_UP, INPUT_PULLUP);
    pinMode(K_DOWN, INPUT_PULLUP);
    pinMode(K_A, INPUT_PULLUP);
#endif
#ifdef NORMAL
    pinMode(K_LEFT, INPUT);
    pinMode(K_RIGHT, INPUT);
    pinMode(K_UP, INPUT);
    pinMode(K_DOWN, INPUT);
    pinMode(K_A, INPUT);
#endif
}
#endif