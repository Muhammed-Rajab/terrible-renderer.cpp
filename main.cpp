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
#include <cstdint>

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void renderBuffer(Pixel *buffer, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Pixel p = buffer[y * width + x];
            std::cout << "\033[38;2;" << (unsigned int)p.r << ";" << (unsigned int)p.g << ";" << (unsigned int)p.b << "m" << "██";
        }
        std::cout << "\n";
    }
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

int main()
{
    int screenWidth = 30;
    int screenHeight = 30;

    Pixel *buffer = new Pixel[screenWidth * screenHeight];

    resetBuffer(buffer, screenWidth, screenHeight, Pixel{0, 50, 0});

    for (int i = 30; i >= 0; i -= 1)
    {
        fillBuffer(buffer, screenWidth, i, i, Pixel{0, (uint8_t)((i / 30.f) * 255), 0});
    }

    renderBuffer(buffer, screenWidth, screenHeight);

    delete[] buffer;

    return EXIT_SUCCESS;
}