
// Q0 -> Start
// Q1 -> RIGHT
// Q2 -> A
// Q4 -> DOWN
// Q5 -> B
// Q6 -> UP
// Q7 -> LEFT
#define KEY_ALT 0
#define KEY_RIGHT 7
#define KEY_A 2
#define KEY_DOWN 4
#define KEY_B 5
#define KEY_UP 6
#define KEY_LEFT 1

#define TFT_CS RX
#define TFT_RST SCL
#define TFT_DC SDA
#define TFT_MOSI MOSI
#define TFT_SCLK SCK
#define TFT_MISO 41 // unused pin (SDA1)

#define SHIFT_CLOCK A3
#define SHIFT_DATA A1
#define SHIFT_LATCH A2

#define SHIFT_INP A0

#define WHITE 0xFFFF
#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFD20
#define PINK 0xF81F

#define FPS 24