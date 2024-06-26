// #include "display.h"
// #include <SPI.h>

int TFT_MAX_WIDTH;
int TFT_MAX_HEIGHT;

// Color definitions
#define BLACK 0x0001
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define PURPLE 0x780F
#define YELLOW 0xFFE0
#define ORANGE 0xFD20
#define WHITE 0xFFFF
#define GREY 0x7BEF

// Timer config
#define TIMER_SPEED 1000

#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2

class Event
{

public:
    Event(void)
    {
        eventType = EVENT_NONE;
    };
    void update(void)
    {

        unsigned long now = millis();
        update(now);
    };
    void update(unsigned long now)
    {

        if (now - lastEventTime >= period)
        {
            switch (eventType)
            {
            case EVENT_EVERY:
                (*callback)();
                break;

            case EVENT_OSCILLATE:
                pinState = !pinState;
                digitalWrite(pin, pinState);
                break;
            }
            lastEventTime = now;
            count++;
        }
        if (repeatCount > -1 && count >= repeatCount)
        {
            eventType = EVENT_NONE;
        }
    };
    int8_t eventType;
    unsigned long period;
    int repeatCount;
    uint8_t pin;
    uint8_t pinState;
    void (*callback)(void);
    unsigned long lastEventTime;
    int count;
};

// #include "Timer.h"

#define MAX_NUMBER_OF_EVENTS (10)
#define TIMER_NOT_AN_EVENT (-2)
#define NO_TIMER_AVAILABLE (-1)

class Timer
{
public:
    Timer(void);

    int8_t every(unsigned long period, void (*callback)(void));
    int8_t every(unsigned long period, void (*callback)(void), int repeatCount);
    int8_t after(unsigned long duration, void (*callback)(void));
    int8_t oscillate(uint8_t pin, unsigned long period, uint8_t startingValue);
    int8_t oscillate(uint8_t pin, unsigned long period, uint8_t startingValue, int repeatCount);
    int8_t pulse(uint8_t pin, unsigned long period, uint8_t startingValue);
    int8_t pulseImmediate(uint8_t pin, unsigned long period, uint8_t pulseValue);
    void stop(int8_t id);
    void update(void);
    void update(unsigned long now);

protected:
    Event _events[MAX_NUMBER_OF_EVENTS];
    int8_t findFreeEventIndex(void);
};

Timer::Timer(void)
{
}

int8_t Timer::every(unsigned long period, void (*callback)(), int repeatCount)
{
    int8_t i = findFreeEventIndex();
    if (i == -1)
        return -1;

    _events[i].eventType = EVENT_EVERY;
    _events[i].period = period;
    _events[i].repeatCount = repeatCount;
    _events[i].callback = callback;
    _events[i].lastEventTime = millis();
    _events[i].count = 0;
    return i;
}

int8_t Timer::every(unsigned long period, void (*callback)())
{
    return every(period, callback, -1); // - means forever
}

int8_t Timer::after(unsigned long period, void (*callback)())
{
    return every(period, callback, 1);
}

int8_t Timer::oscillate(uint8_t pin, unsigned long period, uint8_t startingValue, int repeatCount)
{
    int8_t i = findFreeEventIndex();
    if (i == NO_TIMER_AVAILABLE)
        return NO_TIMER_AVAILABLE;

    _events[i].eventType = EVENT_OSCILLATE;
    _events[i].pin = pin;
    _events[i].period = period;
    _events[i].pinState = startingValue;
    digitalWrite(pin, startingValue);
    _events[i].repeatCount = repeatCount * 2; // full cycles not transitions
    _events[i].lastEventTime = millis();
    _events[i].count = 0;
    return i;
}

int8_t Timer::oscillate(uint8_t pin, unsigned long period, uint8_t startingValue)
{
    return oscillate(pin, period, startingValue, -1); // forever
}

int8_t Timer::pulse(uint8_t pin, unsigned long period, uint8_t startingValue)
{
    return oscillate(pin, period, startingValue, 1); // once
}

int8_t Timer::pulseImmediate(uint8_t pin, unsigned long period, uint8_t pulseValue)
{
    int8_t id(oscillate(pin, period, pulseValue, 1));
    // now fix the repeat count
    if (id >= 0 && id < MAX_NUMBER_OF_EVENTS)
    {
        _events[id].repeatCount = 1;
    }
    return id;
}

void Timer::stop(int8_t id)
{
    if (id >= 0 && id < MAX_NUMBER_OF_EVENTS)
    {
        _events[id].eventType = EVENT_NONE;
    }
}

void Timer::update(void)
{
    unsigned long now = millis();
    update(now);
}

void Timer::update(unsigned long now)
{
    for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
    {
        if (_events[i].eventType != EVENT_NONE)
        {
            _events[i].update(now);
        }
    }
}

int8_t Timer::findFreeEventIndex(void)
{
    for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
    {
        if (_events[i].eventType == EVENT_NONE)
        {
            return i;
        }
    }
    return NO_TIMER_AVAILABLE;
}

Timer m_timer;
int8_t m_timerId;

// Controller config
#define BUTTON_RIGHT 3
#define BUTTON_LEFT 5   // 2
#define BUTTON_DOWN 2   // 3
#define BUTTON_ROTATE 4 // 5

// Playfield definition
#define CELLS_WIDE 10
#define CELLS_TALL 20
#define HIDDEN_CELLS 2
#define CELL_SIZE 8
#define PLAYFIELD_X_OFFSET 0
#define TETROMINO_SPAWN_POS_X 3
#define TETROMINO_SPAWN_POS_Y 0
#define TETROMINO_NEXT_POS_X 11
#define TETROMINO_NEXT_POS_Y 2
#define VOID_CELL BLACK

// Event definition
#define EVENT_MOVE_LEFT 0
#define EVENT_MOVE_RIGHT 1
#define EVENT_MOVE_DOWN 2
#define EVENT_ROTATE 3
#define EVENT_SPAWN 4

#define MAX_FORCE_MOVE 2
#define INITIAL_LEVEL_GOAL 5
#define LEVEL_GOAL_UP_FACTOR 2

// Tetromino definitions
#define TETROMINO_I 0
#define TETROMINO_S 1
#define TETROMINO_Z 2
#define TETROMINO_J 3
#define TETROMINO_O 4
#define TETROMINO_L 5
#define TETROMINO_T 6
#define TETROMINO_ROT_0 0
#define TETROMINO_ROT_90 1
#define TETROMINO_ROT_180 2
#define TETROMINO_ROT_270 3

uint16_t TETROMINO_COLOR_MAP[7] = {
    CYAN,   // TETROMINO_I
    GREEN,  // TETROMINO_S
    RED,    // TETROMINO_Z
    BLUE,   // TETROMINO_J
    YELLOW, // TETROMINO_O
    ORANGE, // TETROMINO_L
    PURPLE  // TETROMINO_T
};

uint8_t TETROMINO_MAP[7][4][16] = {
    // TETROMINO_I
    {
        {1, 1, 1, 1,
         0, 0, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 0, 0,
         0, 1, 0, 0,
         0, 1, 0, 0,
         0, 1, 0, 0},

        {1, 1, 1, 1,
         0, 0, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 0, 0,
         0, 1, 0, 0,
         0, 1, 0, 0,
         0, 1, 0, 0}},

    // TETROMINO_S
    {
        {0, 1, 1, 0,
         1, 1, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 0, 0,
         0, 1, 1, 0,
         0, 0, 1, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         1, 1, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 0, 0,
         0, 1, 1, 0,
         0, 0, 1, 0,
         0, 0, 0, 0}},

    // TETROMINO_Z
    {
        {1, 1, 0, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         0, 1, 1, 0,
         0, 1, 0, 0,
         0, 0, 0, 0},

        {1, 1, 0, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         0, 1, 1, 0,
         0, 1, 0, 0,
         0, 0, 0, 0}},

    // TETROMINO_J
    {
        {0, 0, 1, 0,
         0, 0, 1, 0,
         0, 1, 1, 0,
         0, 0, 0, 0},

        {1, 0, 0, 0,
         1, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         0, 1, 0, 0,
         0, 1, 0, 0,
         0, 0, 0, 0},

        {1, 1, 1, 0,
         0, 0, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0}},

    // TETROMINO_O
    {
        {0, 1, 1, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         0, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0}},

    // TETROMINO_L
    {
        {0, 1, 0, 0,
         0, 1, 0, 0,
         0, 1, 1, 0,
         0, 0, 0, 0},

        {1, 1, 1, 0,
         1, 0, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 1, 0,
         0, 0, 1, 0,
         0, 0, 1, 0,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         1, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0}},

    // TETROMINO_T
    {
        {0, 1, 0, 0,
         1, 1, 1, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 1, 0, 0,
         0, 1, 1, 0,
         0, 1, 0, 0,
         0, 0, 0, 0},

        {1, 1, 1, 0,
         0, 1, 0, 0,
         0, 0, 0, 0,
         0, 0, 0, 0},

        {0, 0, 1, 0,
         0, 1, 1, 0,
         0, 0, 1, 0,
         0, 0, 0, 0}}};

// Playfield Object
uint16_t m_currentPlayfield[CELLS_WIDE][CELLS_TALL];
uint8_t m_currentTetrominoPos[2] = {TETROMINO_SPAWN_POS_X, TETROMINO_SPAWN_POS_Y};
uint8_t m_currentTetrominoType = TETROMINO_L;
uint8_t m_currentTetrominoRotation = TETROMINO_ROT_0;
uint8_t m_nextTetrominoType = TETROMINO_L;
uint8_t m_nextTetrominoRotation = TETROMINO_ROT_0;
uint8_t m_retryForceMove = 0;
uint8_t m_linesCleaned = 0;
uint16_t m_levelGoal = INITIAL_LEVEL_GOAL;
uint16_t m_currentLevelLines = 0;
uint8_t m_currentLevel = 1;
bool m_gameover = false;

void nextLevel();
void drawCell(uint8_t x, uint8_t y, uint16_t color);
void fixTetrominoToPlayfield();
void clearLineOnPlayfield(uint8_t line);
void drawTetrominoToTft(uint8_t tetrominoType, uint8_t tetrominoRotation, uint8_t xPos, uint8_t yPos, uint16_t color);
void drawTetromino();
void spawnTetromino();
void clearTetromino();
void drawNextTetromino();
void clearNextTetromino();
bool collisionDetection(uint8_t onEvent);
void moveLeft();
void moveRight();
bool moveDown();
void rotate();
void clearPlayfield();
void drawPlayfield();
void drawScoreLayout();
void initializeTftDisplay();
void manageLevel(uint16_t linesCleaned);
uint8_t checkLinesCleaned();
void updateWorld();
void initializeTimer();
void initilizeButton();
void initilizeGame();
void controller();
void setupTetris();
void loopTetris();

void drawCell(uint8_t x, uint8_t y, uint16_t color)
{
    fillRect(PLAYFIELD_X_OFFSET + x * CELL_SIZE, y * CELL_SIZE,
             CELL_SIZE, CELL_SIZE, color);
    drawRect(PLAYFIELD_X_OFFSET + x * CELL_SIZE, y * CELL_SIZE,
             CELL_SIZE, CELL_SIZE, VOID_CELL);
}

void fixTetrominoToPlayfield()
{
    for (uint8_t y = 0; y < 4; ++y)
    {
        for (uint8_t x = 0; x < 4; ++x)
        {
            if (TETROMINO_MAP[m_currentTetrominoType][m_currentTetrominoRotation][x + (y * 4)] == 1)
            {
                m_currentPlayfield[m_currentTetrominoPos[0] + x][m_currentTetrominoPos[1] + y] = TETROMINO_COLOR_MAP[m_currentTetrominoType];
            }
        }
    }
}

void drawTetrominoToTft(uint8_t tetrominoType, uint8_t tetrominoRotation, uint8_t xPos, uint8_t yPos, uint16_t color)
{
    for (uint8_t y = 0; y < 4; ++y)
    {
        for (uint8_t x = 0; x < 4; ++x)
        {
            if (TETROMINO_MAP[tetrominoType][tetrominoRotation][x + (y * 4)] == 1)
            {
                drawCell(xPos + x, yPos + y, color);
            }
        }
    }
}

void drawTetromino()
{
    drawTetrominoToTft(m_currentTetrominoType, m_currentTetrominoRotation,
                       m_currentTetrominoPos[0], m_currentTetrominoPos[1],
                       TETROMINO_COLOR_MAP[m_currentTetrominoType]);
}

void clearTetromino()
{
    drawTetrominoToTft(m_currentTetrominoType, m_currentTetrominoRotation,
                       m_currentTetrominoPos[0], m_currentTetrominoPos[1],
                       VOID_CELL);
}

void drawNextTetromino()
{
    drawTetrominoToTft(m_nextTetrominoType, m_nextTetrominoRotation,
                       TETROMINO_NEXT_POS_X, TETROMINO_NEXT_POS_Y,
                       TETROMINO_COLOR_MAP[m_nextTetrominoType]);
}

void clearNextTetromino()
{
    drawTetrominoToTft(m_nextTetrominoType, m_nextTetrominoRotation,
                       TETROMINO_NEXT_POS_X, TETROMINO_NEXT_POS_Y,
                       VOID_CELL);
}

bool collisionDetection(uint8_t onEvent)
{
    int8_t futureTetrominoPos[2];
    futureTetrominoPos[0] = m_currentTetrominoPos[0];
    futureTetrominoPos[1] = m_currentTetrominoPos[1];
    uint8_t futureTetrominoRotation = m_currentTetrominoRotation;

    switch (onEvent)
    {
    case EVENT_MOVE_LEFT:
    {
        futureTetrominoPos[0] = futureTetrominoPos[0] - 1;
    }
    break;
    case EVENT_MOVE_RIGHT:
    {
        futureTetrominoPos[0] = futureTetrominoPos[0] + 1;
    }
    break;
    case EVENT_MOVE_DOWN:
    {
        futureTetrominoPos[1] = futureTetrominoPos[1] + 1;
    }
    break;
    case EVENT_ROTATE:
    {
        futureTetrominoRotation = (futureTetrominoRotation + 1) % 4;
    }
    break;
    case EVENT_SPAWN:
    {
        // Do nothing
    }
    break;
    default:
        break;
    }

    for (uint8_t y = 0; y < 4; ++y)
    {
        for (uint8_t x = 0; x < 4; ++x)
        {
            if (TETROMINO_MAP[m_currentTetrominoType][futureTetrominoRotation][x + (y * 4)] == 1)
            {
                // Check Playfield collision
                if (m_currentPlayfield[futureTetrominoPos[0] + x][futureTetrominoPos[1] + y] != VOID_CELL)
                {
                    return true;
                }
                // Check Left/Right border collision
                if (futureTetrominoPos[0] + x < 0 || futureTetrominoPos[0] + x >= CELLS_WIDE)
                {
                    return true;
                }
                // Check bottom border collision
                if (futureTetrominoPos[1] + y >= CELLS_TALL)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void moveLeft()
{
    if (collisionDetection(EVENT_MOVE_LEFT) == false)
    {
        clearTetromino();
        m_currentTetrominoPos[0] = m_currentTetrominoPos[0] - 1;
        drawTetromino();
    }
}

void moveRight()
{
    if (collisionDetection(EVENT_MOVE_RIGHT) == false)
    {
        clearTetromino();
        m_currentTetrominoPos[0] = m_currentTetrominoPos[0] + 1;
        drawTetromino();
    }
}

bool moveDown()
{
    if (collisionDetection(EVENT_MOVE_DOWN) == false)
    {
        clearTetromino();
        m_currentTetrominoPos[1] = m_currentTetrominoPos[1] + 1;
        drawTetromino();
        return true;
    }
    return false;
}

void rotate()
{
    if (collisionDetection(EVENT_ROTATE) == false)
    {
        clearTetromino();
        m_currentTetrominoRotation = (m_currentTetrominoRotation + 1) % 4;
        drawTetromino();
    }
}

void clearPlayfield()
{
    // initialize Playfield with void cells
    for (uint8_t x = 0; x < CELLS_WIDE; ++x)
    {
        for (uint8_t y = 0; y < CELLS_TALL; ++y)
        {
            m_currentPlayfield[x][y] = VOID_CELL;
        }
    }
}

void drawPlayfield()
{
    for (uint8_t x = 0; x < CELLS_WIDE; ++x)
    {
        for (uint8_t y = 0; y < CELLS_TALL; ++y)
        {
            drawCell(x, y, m_currentPlayfield[x][y]);
        }
    }
}

void drawScoreLayout()
{
    fillRect(PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 0,
             TFT_MAX_WIDTH - (PLAYFIELD_X_OFFSET + (CELLS_WIDE - 1) * CELL_SIZE), TFT_MAX_HEIGHT, BLACK);

    drawRect(PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 0,
             TFT_MAX_WIDTH - (PLAYFIELD_X_OFFSET + (CELLS_WIDE - 1) * CELL_SIZE), TFT_MAX_HEIGHT, RED);
    // Next layout
    char next[6] = "Next\0";
    drawString(13 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 5, next, WHITE);

    if (m_gameover)
    {
        char game[6] = "GAME\0";
        drawString(13 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 50, game, RED);

        char over[6] = "OVER\0";
        drawString(13 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 60, over, RED);
    }

    // Line layout
    char lines[7] = "Lines\0";
    drawString(9 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 80, lines, WHITE);
    sprintf(lines, "%5d", m_linesCleaned);
    drawString(9 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 90, lines, WHITE);

    // Level layout
    char level[8] = "Level\0";
    drawString(9 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 105, level, WHITE);
    sprintf(level, "%5d", m_currentLevel);
    drawString(9 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 115, level, WHITE);

    // Goal layout
    char goal[8] = "Goal\0";
    drawString(15 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 130, goal, WHITE);
    sprintf(goal, "%5d", m_levelGoal - m_currentLevelLines);
    drawString(9 + PLAYFIELD_X_OFFSET + CELLS_WIDE * CELL_SIZE, 140, goal, WHITE);
}

void initializeTftDisplay()
{
    // Display Setup
    //   setupScreen();
    // tft.initR(); // initialize a ST7735R chip
    setRotation(2);
    // tft.writecommand(ST7735_DISPON);
}

void manageLevel(uint16_t linesCleaned)
{
    m_currentLevelLines += linesCleaned;

    if (m_currentLevelLines >= m_levelGoal)
    {
        nextLevel();
    }
}
uint8_t checkLinesCleaned()
{
    uint8_t lineCleanedCount = 0;
    for (int8_t y = 0; y < CELLS_TALL; ++y)
    {
        bool clearLine = true;
        for (uint8_t x = 0; x < CELLS_WIDE; ++x)
        {
            if (m_currentPlayfield[x][y] == VOID_CELL)
            {
                clearLine = false;
                break;
            }
        }
        if (clearLine)
        {
            clearLineOnPlayfield(y);
            lineCleanedCount++;
        }
        clearLine = true;
    }
    return lineCleanedCount;
}

void updateWorld()
{
    if (moveDown() == true)
    { // Try to go down
        m_retryForceMove = 0;
    }
    else
    {
        if (++m_retryForceMove > MAX_FORCE_MOVE)
        { // block Tetromino on Playfield
            fixTetrominoToPlayfield();
            // check lines completed
            uint8_t lastLinesCleaned = checkLinesCleaned();
            if (lastLinesCleaned > 0)
            {
                drawPlayfield();
                m_linesCleaned += lastLinesCleaned;
                manageLevel(lastLinesCleaned);
                drawScoreLayout();
            }

            // Prepare spawn new Tetromino
            clearNextTetromino();
            spawnTetromino();
            drawNextTetromino();
            drawTetromino();

            if (m_retryForceMove > MAX_FORCE_MOVE + 2)
            { // TODO review game over manager
                m_gameover = true;
                drawScoreLayout();
            }
        }
    }
}

void initializeTimer()
{
    // Timer Setup
    m_timerId = m_timer.every(TIMER_SPEED, updateWorld);
    m_timer.every(60, controller);
}
void initilizeButton()
{
    // initialize the pushbutton pin as an input:
    pinMode(BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(BUTTON_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_ROTATE, INPUT_PULLUP);
}

void initilizeGame()
{
    // Spawn random Tetromino
    randomSeed(analogRead(0));
    m_nextTetrominoType = random(7);
    m_nextTetrominoRotation = random(4);
    clearPlayfield();
    spawnTetromino();

    drawScoreLayout();
    drawPlayfield();
    drawTetromino();
    drawNextTetromino();
}

long m_lastRotationTime = 0;

void controller()
{
    if (digitalRead(BUTTON_RIGHT) == LOW)
    {
        moveRight();
    }
    if (digitalRead(BUTTON_LEFT) == LOW)
    {
        moveLeft();
    }
    if (digitalRead(BUTTON_DOWN) == LOW)
    {
        moveDown();
    }
    if (digitalRead(BUTTON_ROTATE) == LOW)
    {
        if (millis() - m_lastRotationTime > 200)
        {
            m_lastRotationTime = millis();
            rotate();
        }
    }
}

void spawnTetromino()
{
    m_currentTetrominoPos[0] = TETROMINO_SPAWN_POS_X;
    m_currentTetrominoPos[1] = TETROMINO_SPAWN_POS_Y;
    m_currentTetrominoType = m_nextTetrominoType;
    m_currentTetrominoRotation = m_nextTetrominoRotation;
    m_nextTetrominoType = random(7);
    m_nextTetrominoRotation = random(4);
}

void clearLineOnPlayfield(uint8_t line)
{
    for (int8_t y = line; y >= 1; --y)
    {
        for (uint8_t x = 0; x < CELLS_WIDE; ++x)
        {
            m_currentPlayfield[x][y] = m_currentPlayfield[x][y - 1];
        }
    }
}

void nextLevel()
{
    m_currentLevelLines = m_currentLevelLines - m_levelGoal; // update lines for next level
    m_currentLevel++;
    m_levelGoal = m_levelGoal * LEVEL_GOAL_UP_FACTOR; // next level goal
    // increase timer speed
    m_timer.stop(m_timerId);
    m_timerId = m_timer.every((TIMER_SPEED / m_currentLevel), updateWorld);
}

void setupTetris(void)
{
    //   Serial.begin(9600);
    initializeTftDisplay();
    TFT_MAX_WIDTH = width();
    TFT_MAX_HEIGHT = height();
    initializeTimer();
    initilizeButton();
    initilizeGame();
}

void loopTetris()
{
    m_timer.update();
}
