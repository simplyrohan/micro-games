#include <Adafruit_GFX.h>
#include "../defines.h"
#include <tgx.h>
#include "naruto.h"

using namespace tgx;

#define SLX 120
#define SLY 68

uint16_t fb[SLX * SLY];
uint16_t fb2[SLX * SLY];
uint16_t *zbuf;

Image<RGB565> imfb(fb, SLX, SLY);

const int LOADED_SHADERS = TGX_SHADER_PERSPECTIVE | TGX_SHADER_ZBUFFER | TGX_SHADER_FLAT | TGX_SHADER_GOURAUD | TGX_SHADER_NOTEXTURE | TGX_SHADER_TEXTURE_NEAREST | TGX_SHADER_TEXTURE_WRAP_POW2;

Renderer3D<RGB565, LOADED_SHADERS, uint16_t> renderer;

int loopnumber = 0;
int prev_loopnumber = -1;
int other = 0;
int started = 0;

void setupSnake(Adafruit_GFX *tft)
{
    tft->fillScreen(BLACK);

    zbuf = (uint16_t *)malloc(SLX * SLY * sizeof(uint16_t));
    while (zbuf == nullptr)
    {
        Serial.println("Error: cannot allocate memory for zbuf");
        delay(1000);
    }

    renderer.setViewportSize(SLX, SLY);
    renderer.setOffset(0, 0);
    renderer.setImage(&imfb);
    renderer.setZbuffer(zbuf);
    renderer.setPerspective(45, ((float)tft->width()) / tft->height(), 1.0f, 100.0f);
    renderer.setMaterial(RGBf(0.85f, 0.55f, 0.25f), 0.2f, 0.7f, 0.8f, 64);
    renderer.setCulling(1);
    renderer.setTextureQuality(TGX_SHADER_TEXTURE_NEAREST);
    renderer.setTextureWrappingMode(TGX_SHADER_TEXTURE_WRAP_POW2);
}

fMat4 moveModel(int &loopnumber)
{
    const float end1 = 6000;
    const float end2 = 2000;
    const float end3 = 6000;
    const float end4 = 2000;

    int tot = (int)(end1 + end2 + end3 + end4);
    int m = millis();

    loopnumber = m / tot;
    float t = m % tot;

    const float dilat = 9;
    const float roty = 360 * (t / 4000);
    float tz, ty;

    if (t < end1)
    {
        tz = -25;
        ty = 0;
    }
    else
    {
        t -= end1;
        if (t < end2)
        {
            t /= end2;
            tz = -25 + 18 * t;
            ty = -6.5f * t;
        }
        else
        {
            t -= end2;
            if (t < end3)
            {
                tz = -7;
                ty = -6.5f;
            }
            else
            {
                t -= end3;
                t /= end4;
                tz = -7 - 18 * t;
                ty = -6.5 + 6.5 * t;
            }
        }
    }

    fMat4 M;
    M.setScale({dilat, dilat, dilat});
    M.multRotate(-roty, {0, 1, 0});
    M.multTranslate({0, ty, tz});
    return M;
}

int updateSnake(Adafruit_GFX *tft)
{
    fMat4 M = moveModel(loopnumber);
    renderer.setModelMatrix(M);

    imfb.fillScreen(RGB565_Black);
    renderer.clearZbuffer();

    switch (loopnumber % 4)
    {
    case 0:
        renderer.setShaders(TGX_SHADER_GOURAUD | TGX_SHADER_TEXTURE);
        renderer.drawMesh(&naruto_1, false);
        break;
    case 1:
        renderer.drawWireFrameMesh(&naruto_1, true);
        break;
    case 2:
        renderer.setShaders(TGX_SHADER_FLAT);
        renderer.drawMesh(&naruto_1, false);
        break;
    case 3:
        renderer.setShaders(TGX_SHADER_GOURAUD);
        renderer.drawMesh(&naruto_1, false);
        break;
    }

    tft->startWrite();
    for (int x = 0; x < SLX; x++)
    {
        for (int y = 0; y < SLY; y++)
        {
            if (started == 1)
            {
                int d = fb2[y * SLX + x];
                if (d != 0)
                {
                    tft->writePixel(x * 2, y * 2, 0);
                    tft->writePixel(x * 2 + 1, y * 2 + 1, 0);
                }
            }
            else
            {
                started = 1;
            }

            int c = fb[y * SLX + x];
            if (c != 0)
            {
                tft->writePixel(x * 2, y * 2, c);
                tft->writePixel(x * 2 + 1, y * 2 + 1, c);
            }
        }
    }
    tft->endWrite();

    memcpy(fb2, fb, SLX * SLY * sizeof(uint16_t));
    other = !other;

    if (prev_loopnumber != loopnumber)
    {
        prev_loopnumber = loopnumber;
        tft->fillRect(0, 105, 240, 20, BLACK);
        tft->setTextColor(WHITE);
        tft->setCursor(5, 110);

        switch (loopnumber % 4)
        {
        case 0:
            tft->print("Gouraud shading / texturing");
            break;
        case 1:
            tft->print("Wireframe");
            break;
        case 2:
            tft->print("Flat Shading");
            break;
        case 3:
            tft->print("Gouraud shading");
            break;
        }
    }

    return 0;
}

void keysSnake(bool keys[7])
{
    // Function not implemented
}
