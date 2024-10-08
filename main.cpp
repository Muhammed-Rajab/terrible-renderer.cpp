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
#include "include/map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

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

class Renderer
{
public:
    int width;
    int height;

    Pixel *screen = nullptr;
    Pixel *buffer = nullptr;

    // ! CONSTRUCTION/DESTRUCTION
    Renderer(int width, int height)
    {
        this->width = width;
        this->height = height;

        this->screen = new Pixel[width * height];
        this->buffer = new Pixel[width * height];
    }

    ~Renderer()
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
    void render()
    {
        std::ostringstream oss;
        for (int y = 0; y < this->height; ++y)
        {
            for (int x = 0; x < this->width; ++x)
            {
                const Pixel &p = this->buffer[y * width + x];
                oss << "\033[38;2;" << (unsigned int)p.r << ";" << (unsigned int)p.g << ";" << (unsigned int)p.b << "m" << "██";
            }
            oss << "\n";
        }
        std::cout << oss.str();
    }

    // ! BUFFER OPS
    void resetBuffer(Pixel p)
    {
        for (int j = 0; j < this->height; ++j)
        {
            for (int i = 0; i < this->width; ++i)
            {
                this->buffer[j * this->width + i] = p;
            }
        }
    }

    inline void putPixel(int x, int y, Pixel color)
    {
        if (x >= 0 && x < this->width && y >= 0 && y < this->height)
            this->buffer[y * this->width + x] = color;
    }

    void swapBuffers()
    {
        std::memcpy(this->screen, this->buffer, sizeof(Pixel) * this->width * this->height);
    }

    // ! TERMINAL OPS
    void clearScreen()
    {
        std::cout << "\033[2J";
    }

    void resetCursor()
    {
        std::cout << "\033[H";
    }

    // ! SHAPES
    void rectangle(int x, int y, float rect_width, float rect_height, Pixel color)
    {
        for (int j = y; j < y + rect_height; ++j)
        {
            for (int i = x; i < x + rect_width; ++i)
            {
                this->putPixel(i, j, color);
            }
        }
    }

    void line(int x0, int y0, int x1, int y1, Pixel color)
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

    void circleBorder(int centerX, int centerY, int radius, Pixel color)
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

    void circleFill(int centerX, int centerY, int radius, Pixel color)
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

    // ! TEST LOOP
    void loop()
    {
        this->clearScreen();
        this->resetCursor();

        float DELAY_uS = (1 / 60.0f) * 1000000;

        while (true)
        {
            this->resetBuffer(Pixel{255, 255, 255});

            // * DRAWING CODE GOES HERE --------------------------------------->

            //*---------------------------------------------------------------->
            this->swapBuffers();

            this->resetCursor();
            this->render();

            usleep(DELAY_uS);
        }
    }
};

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
    return {(std::uint8_t)col.r, (std::uint8_t)col.g, (std::uint8_t)col.b};
}

void drawTile(int n, int x, int y, unsigned char *tileset, int tilesetWidth, int tilesetHeight, int tilesetChannels, int tileWidth, int tileHeight, Renderer &r)
{

    int linearWidth = tileWidth * n;

    // ((Tw * n) // w) * h
    int j0 = (linearWidth / tilesetWidth) * tileHeight;

    // ( Tw * n ) % w
    int i0 = (linearWidth % tilesetWidth) * tilesetChannels;

    // std::cout << "X: " << i0 << " Y: " << j0 << "\n";

    int xTemp = x;
    int yTemp = y;
    for (int j = j0; j < j0 + 16; ++j)
    {
        int x0 = xTemp;
        for (int i = i0; i < i0 + tilesetChannels * 16; i += tilesetChannels)
        {
            int index = j * tilesetWidth * tilesetChannels + i;

            uint8_t red = tileset[index];
            uint8_t green = tileset[index + 1];
            uint8_t blue = tileset[index + 2];

            r.putPixel(x0, yTemp, {red, green, blue});
            ++x0;
        }
        ++yTemp;
    }
}

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Renderer r{128, 128};

    r.clearScreen();
    r.resetCursor();
    float DELAY_uS = (1 / 60.0f) * 1000000;

    // ! READY THE SPRITES
    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    unsigned char *tileset = stbi_load("./assets/test/tileset.png", &tilesetWidth, &tilesetHeight, &tilesetChannels, 4);
    if (tileset == nullptr)
        throw(std::string("Failed to load tileset"));
    int tileWidth = 16;
    int tileHeight = 16;

    int cameraX = 0;
    int cameraY = 0;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAWING CODE GOES HERE --------------------------------------->
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                int mapX = cameraX + x;
                int mapY = cameraY + y;

                if (mapX >= 0 && mapX < Tilemap::WIDTH && mapY >= 0 && mapY < Tilemap::HEIGHT)
                {
                    int tile = Tilemap::map[mapY][mapX];
                    drawTile(tile - 1, x * 16, y * 16, tileset, tilesetWidth, tilesetHeight, tilesetChannels, Tilemap::TILE_SIZE, Tilemap::TILE_SIZE, r);
                }
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        usleep(DELAY_uS);
    }

    return EXIT_SUCCESS;
}