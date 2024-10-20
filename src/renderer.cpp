#include <iostream>
#include <cstring>

#include "../include/renderer.h"
#include "../include/character_map.h"

//*---------------------------------------
//* PIXELS
//*---------------------------------------
void Pixel::display()
{
    std::cout << "R: " << (int)this->r << " G: " << (int)this->g << " B: " << (int)this->b << "\n";
}

//*---------------------------------------
//* RENDERER
//*---------------------------------------

// ! CONSTRUCTION/DESTRUCTION
Renderer ::Renderer(int width, int height)
{
    this->width = width;
    this->height = height;

    this->screen = new Pixel[width * height];
    this->buffer = new Pixel[width * height];
}

Renderer::~Renderer()
{
    if (this->screen != nullptr)
    {
        delete[] this->screen;
    }
    if (this->buffer != nullptr)
    {
        delete[] this->buffer;
    }
}

// ! RENDERING
void Renderer::render()
{
    for (int y = 0; y < this->height; ++y)
    {
        int row_offset = y * width;
        for (int x = 0; x < this->width; ++x)
        {

            Pixel &p = this->buffer[row_offset + x];

            // HACK: COLOR CLAMPING
            p.r = std::max(0, std::min(255, (int)p.r));
            p.g = std::max(0, std::min(255, (int)p.g));
            p.b = std::max(0, std::min(255, (int)p.b));
            p.a = std::max(0, std::min(255, (int)p.a));

#if ASCII_RENDER_NO_COLOR
            int brightness = static_cast<int>((0.299f * p.r + 0.587 * p.g + 0.114f * p.b));
            char pixel = CHARACTER_MAP[brightness];
            std::cout << pixel << pixel;
#elif ASCII_RENDER_WITH_COLOR
            int brightness = static_cast<int>((0.299f * p.r + 0.587 * p.g + 0.114f * p.b));
            char pixel = CHARACTER_MAP[brightness];
            std::cout << "\033[38;2;" << (unsigned int)p.r << ";" << (unsigned int)p.g << ";" << (unsigned int)p.b << "m" << pixel << pixel;
#else
            std::cout << "\033[38;2;" << (unsigned int)p.r << ";" << (unsigned int)p.g << ";" << (unsigned int)p.b << "m" << "██";

#endif
        }
        std::cout << "\n";
    }
}

// ! BUFFER OPS
void Renderer::resetBuffer(Pixel p)
{
    for (int j = 0; j < this->height; ++j)
    {
        for (int i = 0; i < this->width; ++i)
        {
            this->buffer[j * this->width + i] = p;
        }
    }
}

void Renderer::putPixel(int x, int y, const Pixel &color)
{
    if (x >= 0 && x < this->width && y >= 0 && y < this->height)
    {
        // * GET PREV VALUE
        // * CHECK ALPHA
        // FIXME: ADD SUPPORT FOR SEMI TRANSPARENT BACKGROUND
        // this->buffer[y * this->width + x] = color;

        // Pixel temp = this->buffer[y * this->width + x];
        if (color.a == 0)
        {
            return;
        }

        if (color.a == 255)
        {
            this->buffer[y * this->width + x] = color;
            return;
        }
    }
}

void Renderer::swapBuffers()
{
    std::memcpy(this->screen, this->buffer, sizeof(Pixel) * this->width * this->height);
}

// ! TERMINAL OPS
void Renderer::clearScreen()
{
    std::cout << "\033[2J";
}

void Renderer::resetCursor()
{
    std::cout << "\033[H";
}

// ! SHAPES
void Renderer::rectangle(int x, int y, float rect_width, float rect_height, Pixel color)
{
    for (int j = y; j < y + rect_height; ++j)
    {
        for (int i = x; i < x + rect_width; ++i)
        {
            this->putPixel(i, j, color);
        }
    }
}

void Renderer::line(int x0, int y0, int x1, int y1, Pixel color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true)
    {
        this->putPixel(x0, y0, color); // Set pixel at (x0, y0)

        if (x0 == x1 && y0 == y1)
            break; // Line has been drawn

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::circleBorder(int centerX, int centerY, int radius, Pixel color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius; // Decision variable

    auto putSymmetricPixels = [&](int x, int y)
    {
        this->putPixel(centerX + x, centerY + y, color); // Octants
        this->putPixel(centerX - x, centerY + y, color);
        this->putPixel(centerX + x, centerY - y, color);
        this->putPixel(centerX - x, centerY - y, color);
        this->putPixel(centerX + y, centerY + x, color);
        this->putPixel(centerX - y, centerY + x, color);
        this->putPixel(centerX + y, centerY - x, color);
        this->putPixel(centerX - y, centerY - x, color);
    };

    while (x <= y)
    {
        putSymmetricPixels(x, y);

        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void Renderer::circleFill(int centerX, int centerY, int radius, Pixel color)
{
    int x = 0;
    int y = radius;
    int d = 1 - radius;

    auto putHorizontalLine = [&](int x, int y)
    {
        for (int i = -x; i <= x; i++)
        {
            this->putPixel(centerX + i, centerY + y, color);
            this->putPixel(centerX + i, centerY - y, color);
        }
    };

    while (x <= y)
    {
        putHorizontalLine(x, y);
        putHorizontalLine(y, x);

        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}