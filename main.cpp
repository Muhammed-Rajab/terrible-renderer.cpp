/*
GOALS:
    1. Draw basic shapes
        - Lines
        - Rectangles
        - Circles
        - Points
        - Triangles
    2. Draw Images
        - Load an image to memory, and draw it to the buffer
*/
#include <iostream>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "include/hsl.h"

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    void display()
    {
        std::cout << "R: " << (int)this->r << " G: " << (int)this->g << " B: " << (int)this->b << "\n";
    }
};

void renderBuffer(Pixel *buffer, int width, int height)
{
    std::ostringstream oss;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Pixel p = buffer[y * width + x];
            oss << "\033[38;2;" << (unsigned int)p.r << ";" << (unsigned int)p.g << ";" << (unsigned int)p.b << "m" << "██";
        }
        oss << "\n";
    }
    std::cout << oss.str();
}

void fillBuffer(Pixel *buffer, int width, int x, int y, Pixel p)
{
    for (int j = 0; j < y; ++j)
    {
        for (int i = 0; i < x; ++i)
        {
            buffer[j * width + i] = p;
        }
    }
}

void resetBuffer(Pixel *buffer, int width, int height, Pixel p)
{
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            buffer[j * width + i] = p;
        }
    }
}

void putPixel(Pixel *buffer, int width, int x, int y, Pixel color)
{
    buffer[y * width + x] = color;
}

void clearScreen()
{
    std::cout << "\033[2J";
}

void resetCursor()
{
    std::cout << "\033[H";
}

//*-----------------------
//* UTILS
//*-----------------------
int randomInteger(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

Pixel HSLtoPixel(int hue, float s, float l)
{
    RGBColor col = HSLToRGB(hue, s, l);
    return {col.r, col.g, col.b};
}

//*-----------------------
//* SHAPES
//*-----------------------

void drawRectangle(Pixel *buffer, int width, int x, int y, float rect_width, float rect_height, Pixel p)
{
    for (int j = y; j < y + rect_height; ++j)
    {
        for (int i = x; i < x + rect_width; ++i)
        {
            putPixel(buffer, width, i, j, p);
        }
    }
}

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int screenWidth = 64;
    int screenHeight = 64;
    std::size_t frameCount = 0;

    Pixel *screen = new Pixel[screenWidth * screenHeight];
    Pixel *buffer = new Pixel[screenWidth * screenHeight];

    clearScreen();
    resetCursor();

    float DELAY_uS = (1 / 60.0f) * 1000000;

    float angle = 0;
    float radius = 30;
    int Ox = screenWidth / 2;
    int Oy = screenHeight / 2;

    while (true)
    {
        ++frameCount;
        resetBuffer(buffer, screenWidth, screenHeight, Pixel{255, 255, 255});

        // * DRAWING CODE GOES HERE --------------------------------------->
        drawRectangle(buffer, screenWidth, Ox, Oy, 1, 1, {0, 0, 255});

        // for (int _ = 0; _ < 10; _++)
        // {
        //     drawRectangle(buffer, screenWidth, randomInteger(0, screenWidth - 10), randomInteger(0, screenHeight - 10), 10, 10, HSLtoPixel(randomInteger(0, 360), 1., 0.5));
        // }

        drawRectangle(buffer, screenWidth, Ox + radius * std::cos(angle), Oy + radius * std::sin(angle), 1, 1, {255, 0, 0});
        angle += 0.1;

        if (angle >= 360)
        {
            angle = 0;
        }

        //*---------------------------------------------------------------->

        std::memcpy(screen, buffer, sizeof(Pixel) * screenWidth * screenHeight);

        resetCursor();
        renderBuffer(buffer, screenWidth, screenHeight);

        usleep(DELAY_uS);

        // break;
    }

    delete[] buffer;
    delete[] screen;

    return EXIT_SUCCESS;
}