#include "display.h"
#include "input.h"

#define TFTW 128 // screen width
#define TFTH 160 // screen height
#define TFTW2 64 // half screen width
#define TFTH2 80 // half screen height
// game constant
#define SPEED 1
#define GRAVITY 9.8
#define JUMP_FORCE 1.85
#define SKIP_TICKS 22.0 // 1000 / 50fps
#define MAX_FRAMESKIP 22
// bird size
#define BIRDW 8  // bird width
#define BIRDH 8  // bird height
#define BIRDW2 4 // half width
#define BIRDH2 4 // half height
// pipe size
#define PIPEW 12     // pipe width
#define GAPHEIGHT 36 // pipe gap height
// floor size
#define FLOORH 20 // floor height (from bottom of the screen)
// grass size
#define GRASSH 4 // grass height (inside floor, starts at floor y)

// // background
// const unsigned int BCKGRDCOL = 0x8F5E;
// // bird
// const unsigned int BIRDCOL = 0xFFF5;
// // pipe
// const unsigned int PIPECOL = 0x67E9;
// // pipe highlight
// const unsigned int PIPEHIGHCOL = 0xFFFF;
// // pipe seam
// const unsigned int PIPESEAMCOL = 0;
// // floor
// const unsigned int FLOORCOL = 0x0B61;
// // grass (col2 is the stripe color)
// const unsigned int GRASSCOL = 0x8F0A;
// const unsigned int GRASSCOL2 = 0x9F6B;

// bird sprite
// bird sprite colors (Cx name for values to keep the array readable)
#define C0 BCKGRDCOL
#define C1 0xC529
#define C2 BIRDCOL
#define C3 ST7735_WHITE
#define C4 ST7735_RED
#define C5 0xFECE
static unsigned int birdcol[] =
    {C0, C0, C1, C1, C1, C1, C1, C0,
     C0, C1, C2, C2, C2, C1, C3, C1,
     C0, C2, C2, C2, C2, C1, C3, C1,
     C1, C1, C1, C2, C2, C3, C1, C1,
     C1, C2, C2, C2, C2, C2, C4, C4,
     C1, C2, C2, C2, C1, C5, C4, C0,
     C0, C1, C2, C1, C5, C5, C5, C0,
     C0, C0, C1, C5, C5, C5, C0, C0};

// bird structure
static struct BIRD
{
    unsigned char x, y, old_y;
    unsigned int col;
    float vel_y;
} bird;
// pipe structure
static struct PIPE
{
    char x, gap_y;
    unsigned int col;
} pipe;

// score
static short score;
// temporary x and y var
static short tmpx, tmpy;

// ---------------
// draw pixel
// ---------------
// faster drawPixel method by inlining calls and using setAddrWindow and pushColor
// using macro to force inlining
// #define drawPixel(a, b, c)     \
//     setAddrWindow(a, b, a, b); \
//     pushColor(c)

// ---------------
// initial setup
// ---------------
void game_init();
void game_start();
void game_loop();
void game_over();

void setupFlappy()
{
    setRotation(0);
}

// ---------------
// main loop
// ---------------
void loopFlappy()
{
    game_start();
    game_loop();
    game_over();
}

// ---------------
// game loop
// ---------------
void game_loop()
{
    // ===============
    // prepare game variables
    // draw floor
    // ===============
    // instead of calculating the distance of the floor from the screen height each time store it in a variable
    unsigned char GAMEH = TFTH - FLOORH;
    // draw the floor once, we will not overwrite on this area in-game
    // black line
    drawFastHLine(0, GAMEH, TFTW, ST7735_BLACK);
    // grass and stripe
    fillRect(0, GAMEH + 1, TFTW2, GRASSH, GRASSCOL);
    fillRect(TFTW2, GAMEH + 1, TFTW2, GRASSH, GRASSCOL2);
    // black line
    drawFastHLine(0, GAMEH + GRASSH, TFTW, ST7735_BLACK);
    // mud
    fillRect(0, GAMEH + GRASSH + 1, TFTW, FLOORH - GRASSH, FLOORCOL);
    // grass x position (for stripe animation)
    char grassx = TFTW;
    // game loop time variables
    double delta, old_time, next_game_tick, current_time;
    next_game_tick = current_time = millis();
    int loops;
    // passed pipe flag to count score
    bool passed_pipe = false;
    // temp var for setAddrWindow
    unsigned char px;

    while (1)
    {
        loops = 0;
        while (millis() > next_game_tick && loops < MAX_FRAMESKIP)
        {
            // ===============
            // input
            // ===============
            // erm
            if (digitalRead(K_A) == LOW)
            {
                // if the bird is not too close to the top of the screen apply jump force
                if (bird.y > BIRDH2 * 0.5)
                    bird.vel_y = -JUMP_FORCE;
                // else zero velocity
                else
                    bird.vel_y = 0;
            }

            // ===============
            // update
            // ===============
            // calculate delta time
            // ---------------
            old_time = current_time;
            current_time = millis();
            delta = (current_time - old_time) / 1000;

            // bird
            // ---------------
            bird.vel_y += GRAVITY * delta;
            bird.y += bird.vel_y;

            // pipe
            // ---------------
            pipe.x -= SPEED;
            // if pipe reached edge of the screen reset its position and gap
            if (pipe.x < -PIPEW)
            {
                pipe.x = TFTW;
                pipe.gap_y = random(10, GAMEH - (10 + GAPHEIGHT));
            }

            // ---------------
            next_game_tick += SKIP_TICKS;
            loops++;
        }

        // ===============
        // draw
        // ===============
        // pipe
        // ---------------
        // we save cycles if we avoid drawing the pipe when outside the screen
        if (pipe.x >= 0 && pipe.x < TFTW)
        {
            // pipe color
            drawFastVLine(pipe.x + 3, 0, pipe.gap_y, PIPECOL);
            drawFastVLine(pipe.x + 3, pipe.gap_y + GAPHEIGHT + 1, GAMEH - (pipe.gap_y + GAPHEIGHT + 1), PIPECOL);
            // highlight
            drawFastVLine(pipe.x, 0, pipe.gap_y, PIPEHIGHCOL);
            drawFastVLine(pipe.x, pipe.gap_y + GAPHEIGHT + 1, GAMEH - (pipe.gap_y + GAPHEIGHT + 1), PIPEHIGHCOL);
            // bottom and top border of pipe
            drawPixel(pipe.x, pipe.gap_y, PIPESEAMCOL);
            drawPixel(pipe.x, pipe.gap_y + GAPHEIGHT, PIPESEAMCOL);
            // pipe seam
            drawPixel(pipe.x, pipe.gap_y - 6, PIPESEAMCOL);
            drawPixel(pipe.x, pipe.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
            drawPixel(pipe.x + 3, pipe.gap_y - 6, PIPESEAMCOL);
            drawPixel(pipe.x + 3, pipe.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
        }
        // erase behind pipe
        if (pipe.x <= TFTW)
            drawFastVLine(pipe.x + PIPEW, 0, GAMEH, BCKGRDCOL);

        // bird
        // ---------------
        tmpx = BIRDW - 1;
        do
        {
            px = bird.x + tmpx + BIRDW;
            // clear bird at previous position stored in old_y
            // we can't just erase the pixels before and after current position
            // because of the non-linear bird movement (it would leave 'dirty' pixels)
            tmpy = BIRDH - 1;
            do
            {
                drawPixel(px, bird.old_y + tmpy, BCKGRDCOL);
            } while (tmpy--);
            // draw bird sprite at new position
            tmpy = BIRDH - 1;
            do
            {
                drawPixel(px, bird.y + tmpy, birdcol[tmpx + (tmpy * BIRDW)]);
            } while (tmpy--);
        } while (tmpx--);
        // save position to erase bird on next draw
        bird.old_y = bird.y;

        // grass stripes
        // ---------------
        grassx -= SPEED;
        if (grassx < 0)
            grassx = TFTW;
        drawFastVLine(grassx % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL);
        drawFastVLine((grassx + 64) % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL2);

        // ===============
        // collision
        // ===============
        // if the bird hit the ground game over
        if (bird.y > GAMEH - BIRDH)
            break;
        // checking for bird collision with pipe
        if (bird.x + BIRDW >= pipe.x - BIRDW2 && bird.x <= pipe.x + PIPEW - BIRDW)
        {
            // bird entered a pipe, check for collision
            if (bird.y < pipe.gap_y || bird.y + BIRDH > pipe.gap_y + GAPHEIGHT)
                break;
            else
                passed_pipe = true;
        }
        // if bird has passed the pipe increase score
        else if (bird.x > pipe.x + PIPEW - BIRDW && passed_pipe)
        {
            passed_pipe = false;
            // erase score with background color
            setTextColor(BCKGRDCOL);
            setCursor(TFTW2, 4);
            setTextSize(2);
            print(score);
            // set text color back to white for new score
            setTextColor(ST7735_WHITE);
            // increase score since we successfully passed a pipe
            score++;
        }

        // update score
        // ---------------
        setCursor(TFTW2, 4);
        setTextSize(2);
        print(score);
    }

    // add a small delay to show how the player lost
    delay(1200);
}

// ---------------
// game start
// ---------------
void game_start()
{
    fillScreen(BCKGRDCOL);
    game_init();
}

// ---------------
// game init
// ---------------
void game_init()
{
    // clear screen
    fillScreen(BCKGRDCOL);
    // reset score
    score = 0;
    // init bird
    bird.x = 20;
    bird.y = bird.old_y = TFTH2 - BIRDH;
    bird.vel_y = -JUMP_FORCE;
    tmpx = tmpy = 0;
    // generate new random seed for the pipe gape
    randomSeed(analogRead(0));
    // init pipe
    pipe.x = TFTW;
    pipe.gap_y = random(20, TFTH - 60);
}

