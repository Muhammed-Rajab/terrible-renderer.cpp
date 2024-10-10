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
#include <termios.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include "include/hsl.h"
#include "include/map.h"
#include "include/character_map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#include "include/renderer.h"

struct Camera
{
    float x;
    float y;
    float dx;
    float dy;
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

bool kbhit()
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int bytesWaiting = 0;
    // Check for bytes waiting in the stdin stream
    if (read(STDIN_FILENO, &bytesWaiting, sizeof(bytesWaiting)) == -1)
    {
        bytesWaiting = 0; // If there's an error, set to 0
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return bytesWaiting > 0;
}

char getch()
{
    char c;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    read(STDIN_FILENO, &c, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}

void keyListener(Camera &cam, Renderer &r)
{
    while (true)
    {
        if (kbhit())
        {
            char currentKey = getch(); // Get the currently pressed key

            // Check for key release logic here if necessary
            switch (currentKey)
            {
            case 'D':
            case 'd':
                cam.x += cam.dx;
                break;
            case 'A':
            case 'a':
                cam.x -= cam.dx;
                break;
            case 'W':
            case 'w':
                cam.y -= cam.dy;
                break;
            case 'S':
            case 's':
                cam.y += cam.dy;
                break;

            case 'J':
            case 'j':
                cam.dx -= 0.1;
                break;
            case 'L':
            case 'l':
                cam.dx += 0.1;
                break;
            case 'I':
            case 'i':
                cam.dy -= 0.1;
                break;
            case 'K':
            case 'k':
                cam.dy += 0.1;
                break;
            }
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Reduce CPU usage
        std::this_thread::yield();
    }
}

//*-----------------------------------------
//* TILEMAP
//*-----------------------------------------
class Tileset
{

public:
    char *filename = nullptr;
    unsigned char *data = nullptr;

    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    int TILE_SIZE;

    Tileset(char *filename, int tile_size)
    {
        this->filename = filename;
        this->TILE_SIZE = tile_size;
        this->data = stbi_load(filename, &this->tilesetWidth, &this->tilesetHeight, &this->tilesetChannels, 4);
        if (this->data == nullptr)
            throw std::string("can't load image");

        std::cout << "Tileset Width: " << this->tilesetWidth << "\n";
        std::cout << "Tileset Height: " << this->tilesetHeight << "\n";
        std::cout << "Tileset Channels: " << this->tilesetChannels << "\n";
    }

    void renderTile(int n, int x, int y, Renderer &r)
    {
        int linearWidth = this->TILE_SIZE * n;

        // ((Tw * n) // w) * h
        int j0 = (linearWidth / this->tilesetWidth) * this->TILE_SIZE;

        // ( Tw * n ) % w
        int i0 = (linearWidth % this->tilesetWidth) * this->tilesetChannels;

        int xTemp = x;
        int yTemp = y;
        for (int j = j0; j < j0 + this->TILE_SIZE; ++j)
        {
            int x0 = xTemp;
            for (int i = i0; i < i0 + this->tilesetChannels * this->TILE_SIZE; i += this->tilesetChannels)
            {
                int index = j * this->tilesetWidth * this->tilesetChannels + i;

                uint8_t red = this->data[index];
                uint8_t green = this->data[index + 1];
                uint8_t blue = this->data[index + 2];
                uint8_t alpha = this->data[index + 3];

                r.putPixel(x0, yTemp, {red, green, blue, alpha});
                ++x0;
            }
            ++yTemp;
        }
    }
};

class Tilemap
{
public:
    Tilemap(int tilesAcross, int tilesDown, int) {}
};

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Renderer r{170, 128};

    r.clearScreen();
    r.resetCursor();
    // float DELAY_uS = (1 / 60.0f) * 1000000;
    // int DELAY = 16;
    int DELAY = 16;

    // ! READY THE SPRITES
    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    unsigned char *tileset = stbi_load("./assets/test/tileset.png", &tilesetWidth, &tilesetHeight, &tilesetChannels, 4);
    if (tileset == nullptr)
        throw(std::string("Failed to load tileset"));
    int tileWidth = 16;
    int tileHeight = 16;

    Camera cam{0, 0, 0.4, 0.4};

    std::thread listener(keyListener, std::ref(cam), std::ref(r));

    int **bgLayer = Tilemaps::OneD2TwoD(Tilemaps::backgroundLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::backgroundLayer) / sizeof(int));
    int **objLayer = Tilemaps::OneD2TwoD(Tilemaps::objectLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::objectLayer) / sizeof(int));

    Tileset ts{"./assets/test/tileset.png", 16};

    float prevCamX = cam.x;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAWING CODE GOES HERE --------------------------------------->
        int camTileX = static_cast<int>(cam.x);
        int camTileY = static_cast<int>(cam.y);

        float camOffsetX = cam.x - camTileX;
        float camOffsetY = cam.y - camTileY;

        int tilesAcross = (r.width / tileWidth) + 2;
        int tilesDown = (r.height / tileHeight) + 2;

        for (int y = 0; y < tilesDown; ++y)
        {
            for (int x = 0; x < tilesAcross; ++x)
            {
                int tileX = (x * tileWidth) - static_cast<int>(camOffsetX * tileWidth);
                int tileY = (y * tileHeight) - static_cast<int>(camOffsetY * tileHeight);

                int mapX = camTileX + x;
                int mapY = camTileY + y;

                if (mapX >= 0 && mapX < Tilemaps::WIDTH && mapY >= 0 && mapY < Tilemaps::HEIGHT)
                {
                    int backgroundTile = bgLayer[mapY][mapX];
                    int objectTile = objLayer[mapY][mapX];

                    ts.renderTile(backgroundTile - 1, tileX, tileY, r);
                    ts.renderTile(objectTile - 1, tileX, tileY, r);
                }
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        // cam.x += 0.01;

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
    }

    // ! FREE TILES
    Tilemaps::deleteTwoDArray(bgLayer, Tilemaps::HEIGHT);
    Tilemaps::deleteTwoDArray(objLayer, Tilemaps::HEIGHT);

    listener.join();

    return EXIT_SUCCESS;
}

// int main()
// {
//     // ! SEEDING
//     std::srand(static_cast<unsigned int>(std::time(nullptr)));

//     // Renderer r{170, 128};
//     Renderer r{64, 64};

//     r.clearScreen();
//     r.resetCursor();

//     int DELAY = 1600;

//     int frameCount = 0;

//     while (true)
//     {
//         ++frameCount;

//         r.resetBuffer(Pixel{0, 0, 0});

//         // * DRAWING CODE GOES HERE --------------------------------------->
//         float radius = 64;
//         for (int y = 0; y < r.height; ++y)
//         {
//             for (int x = 0; x < r.width; ++x)
//             {
//                 if (x * x + y * y <= radius * radius)
//                 {
//                     r.putPixel(x, y, {255, 0, 0, 255});
//                 }
//             }
//         }

//         //*---------------------------------------------------------------->
//         r.swapBuffers();
//         r.resetCursor();
//         r.render();

//         // cam.x += 0.01;

//         std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
//     }

//     return EXIT_SUCCESS;
// }