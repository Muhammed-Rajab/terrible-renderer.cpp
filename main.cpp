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

int main()
{
    int screenWidth = 64;
    int screenHeight = 64;
    std::size_t frameCount = 0;

    Pixel *buffer = new Pixel[screenWidth * screenHeight];

    clearScreen();
    resetCursor();

    while (true)
    {
        resetBuffer(buffer, screenWidth, screenHeight, Pixel{0, 50, 0});

        // * DRAWING CODE GOES HERE

        resetCursor();
        renderBuffer(buffer, screenWidth, screenHeight);

        usleep((1 / 60.0f) * 1000);

        ++frameCount;
    }

    delete[] buffer;

    return EXIT_SUCCESS;
}