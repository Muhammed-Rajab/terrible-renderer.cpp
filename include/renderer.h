#ifndef RENDERER_4_TERMINAL_RENDERER_H
#define RENDERER_4_TERMINAL_RENDERER_H

#include <iostream>
#include <cstdint>

// XXX: COMMENT THIS TO GET COLORFUL
#define ASCII_RENDER_NO_COLOR 1
// #define ASCII_RENDER_WITH_COLOR 1

// * REPRESENTS EVERY PIXEL OF OUR SCREEN
struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;

    void display();
};

class Renderer
{
public:
    int width;
    int height;

    Pixel *screen = nullptr;
    Pixel *buffer = nullptr;

    // ! CONSTRUCTION/DESTRUCTION
    Renderer(int width, int height);
    ~Renderer();

    // ! RENDERING
    void render();

    // ! BUFFER OPS
    void swapBuffers();
    void resetBuffer(Pixel p);
    void putPixel(int x, int y, const Pixel &color);

    // ! TERMINAL OPS
    void clearScreen();
    void resetCursor();

    // ! SHAPES
    void line(int x0, int y0, int x1, int y1, Pixel color);
    void rectangle(int x, int y, float rect_width, float rect_height, Pixel color);
    void circleBorder(int centerX, int centerY, int radius, Pixel color);
    void circleFill(int centerX, int centerY, int radius, Pixel color);
};

#endif