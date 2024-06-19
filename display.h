// display.h
// Display setup and calls
// Map your display to each function and all games will work

// This setup is for the 160x128 1.8" Color TFT display
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#define TFT_CLK 13  // SCL/CLK
#define TFT_MOSI 11 // MOSI/SDA
#define TFT_CS 10   // Chip Select
#define TFT_DC 8    // DC/RS
#define TFT_RST -1      // RESET (-1 for hardware RESET pin)

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define COLOR_BLACK ST7735_BLACK
#define COLOR_WHITE ST7735_WHITE
#define COLOR_RED ST7735_RED
#define COLOR_GREEN ST7735_GREEN
#define COLOR_BLUE ST7735_BLUE
#define COLOR_YELLOW ST7735_YELLOW
#define COLOR_CYAN ST7735_CYAN
#define COLOR_MAGENTA ST7735_MAGENTA

void setupScreen()
{
    display.initR(INITR_BLACKTAB);
    display.fillScreen(ST7735_BLACK);
}

void clearScreen()
{
    display.fillScreen(ST7735_BLACK);
}

void drawPixel(int x, int y, uint16_t color)
{
    display.drawPixel(x, y, color);
}

void drawLine(int x0, int y0, int x1, int y1, uint16_t color)
{
    display.drawLine(x0, y0, x1, y1, color);
}

void drawRect(int x, int y, int w, int h, uint16_t color)
{
    display.drawRect(x, y, w, h, color);
}

void fillRect(int x, int y, int w, int h, uint16_t color)
{
    display.fillRect(x, y, w, h, color);
}

void drawCircle(int x, int y, int r, uint16_t color)
{
    display.drawCircle(x, y, r, color);
}

void fillCircle(int x, int y, int r, uint16_t color)
{
    display.fillCircle(x, y, r, color);
}

void drawChar(int x, int y, char c, uint16_t color, uint8_t size)
{
    display.drawChar(x, y, c, color, ST7735_BLACK, size);
}

void drawString(int x, int y, char *c, uint16_t color, uint8_t size = 1)
{
    display.setCursor(x, y);
    display.setTextColor(color);
    display.setTextSize(size);
    display.print(c);
}


void drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, uint16_t color)
{
    display.drawBitmap(x, y, bitmap, w, h, color);
}

void drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, uint16_t color, uint16_t bg)
{
    display.drawBitmap(x, y, bitmap, w, h, color, bg);
}

void setRotation(uint8_t rotation)
{
    display.setRotation(rotation);
}

void setCursor(int x, int y)
{
    display.setCursor(x, y);
}

void setTextColor(uint16_t color)
{
    display.setTextColor(color);
}

void setTextColor(uint16_t color, uint16_t bg)
{
    display.setTextColor(color, bg);
}

void setTextSize(uint8_t size)
{
    display.setTextSize(size);
}

void setTextWrap(boolean wrap)
{
    display.setTextWrap(wrap);
}

void setFont(const GFXfont *f)
{
    display.setFont(f);
}

void setAddrWindow(int x0, int y0, int x1, int y1)
{
    display.setAddrWindow(x0, y0, x1, y1);
}

void pushColor(uint16_t color)
{
    display.pushColor(color);
}

void fillScreen(uint16_t color)
{
    display.fillScreen(color);
}

void drawFastVLine(int x, int y, int h, uint16_t color)
{
    display.drawFastVLine(x, y, h, color);
}

void drawFastHLine(int x, int y, int w, uint16_t color)
{
    display.drawFastHLine(x, y, w, color);
}

void drawRoundRect(int x, int y, int w, int h, int r, uint16_t color)
{
    display.drawRoundRect(x, y, w, h, r, color);
}

void fillRoundRect(int x, int y, int w, int h, int r, uint16_t color)
{
    display.fillRoundRect(x, y, w, h, r, color);
}

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
    display.drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
    display.fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

// void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
// {
//     display.drawBitmap(x, y, bitmap, w, h, color);
// }

// void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
// {
//     display.drawBitmap(x, y, bitmap, w, h, color, bg);
// }

void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
    display.drawXBitmap(x, y, bitmap, w, h, color);
}

void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h)
{
    display.drawGrayscaleBitmap(x, y, bitmap, w, h);
}

void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t *bitmap, const uint8_t *mask, int16_t w, int16_t h)
{
    display.drawGrayscaleBitmap(x, y, bitmap, mask, w, h);
}

void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    display.drawRGBBitmap(x, y, bitmap, w, h);
}

void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
    display.drawChar(x, y, c, color, bg, size);
}

int16_t height()
{
    return display.height();
}

int16_t width()
{
    return display.width();
}

void invertDisplay(boolean i)
{
    display.invertDisplay(i);
}
