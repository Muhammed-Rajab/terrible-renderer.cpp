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
#include <vector>
#include "include/hsl.h"
#include "include/map.h"
#include "include/character_map.h"
#include "include/renderer.h"
#include "include/utils.h"
#include "include/tileset.h"

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
        std::this_thread::yield();
    }
}

using Tile = Pixel **;

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Renderer r{170, 128};
    Renderer r{64, 64};

    r.clearScreen();
    r.resetCursor();

    // IN MILLISECONDS
    int DELAY = 0;

    // ! READY THE SPRITES
    // if (tileset == nullptr)
    //     throw(std::string("Failed to load tileset"));

    Camera cam{0, 0, 0.4, 0.4};

    std::thread listener(keyListener, std::ref(cam), std::ref(r));

    int **bgLayer = Tilemaps::OneD2TwoD(Tilemaps::backgroundLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::backgroundLayer) / sizeof(int));
    int **objLayer = Tilemaps::OneD2TwoD(Tilemaps::objectLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::objectLayer) / sizeof(int));

    // int tileWidth = ts.TILE_SIZE;
    // int tileHeight = ts.TILE_SIZE;

    float prevCamX = cam.x;

    // TODO: IMPLEMENT A WAY TO GET SPRITE BY INDEX

    Tileset ts{"./assets/game/tileset.png", 16};

    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    int TILE_SIZE = 16;

    unsigned char *tileset = stbi_load("./assets/game/tileset.png", &tilesetWidth, &tilesetHeight, &tilesetChannels, 4);
    std::cout << "Tileset Width: " << tilesetWidth << "\n";
    std::cout << "Tileset Height: " << tilesetHeight << "\n";
    std::cout << "Tileset Channels: " << tilesetChannels << "\n";

    // ! PIXEL ARRAY OF TEXTURE
    std::vector<std::vector<Pixel>> pixels(tilesetHeight);
    for (int i = 0; i < tilesetHeight; ++i)
    {
        pixels[i].reserve(tilesetWidth);
    }

    for (int y = 0; y < tilesetHeight; ++y)
    {
        for (int x = 0; x < tilesetWidth; ++x)
        {
            int index = y * tilesetWidth * tilesetChannels + x * tilesetChannels;
            Pixel p = {
                tileset[index],
                tileset[index + 1],
                tileset[index + 2],
                tileset[index + 3],
            };
            pixels.at(y).push_back(p);
        }
    }

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAW TEST STUFF HERE
        for (int y = 0; y < 16; ++y)
        {
            for (int x = 16; x < 32; ++x)
            {
                r.putPixel(x, y, pixels.at(y).at(x));
            }
        }

        // * DRAWING CODE GOES HERE --------------------------------------->
        // int camTileX = static_cast<int>(cam.x);
        // int camTileY = static_cast<int>(cam.y);

        // float camOffsetX = cam.x - camTileX;
        // float camOffsetY = cam.y - camTileY;

        // int tilesAcross = (r.width / tileWidth) + 2;
        // int tilesDown = (r.height / tileHeight) + 2;

        // for (int y = 0; y < tilesDown; ++y)
        // {
        //     for (int x = 0; x < tilesAcross; ++x)
        //     {
        //         int tileX = (x * tileWidth) - static_cast<int>(camOffsetX * tileWidth);
        //         int tileY = (y * tileHeight) - static_cast<int>(camOffsetY * tileHeight);

        //         int mapX = camTileX + x;
        //         int mapY = camTileY + y;

        //         if (mapX >= 0 && mapX < Tilemaps::WIDTH && mapY >= 0 && mapY < Tilemaps::HEIGHT)
        //         {
        //             int backgroundTile = bgLayer[mapY][mapX];
        //             int objectTile = objLayer[mapY][mapX];

        //             // std::cout << "Before render" << "\n";

        //             ts.renderTile(backgroundTile - 1, tileX, tileY, r);
        //             ts.renderTile(objectTile - 1, tileX, tileY, r);

        //             // std::cout << "After render" << "\n";
        //         }
        //     }
        // }

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

// struct Vec3
// {
//     float x;
//     float y;
//     float z;
// };

// Pixel getColor(float x)
// {
//     Vec3 A = {0.5, 0.5, 0.5};
//     Vec3 B = {0.5, 0.5, 0.5};
//     Vec3 C = {1.0, 1.0, 1.0};
//     Vec3 D = {0.00, 0.10, 0.20};

//     Pixel c = {
//         A.x + B.x * std::cos(2 * 3.14159 * (C.x * x + D.x)) * 255,
//         A.y + B.y * std::cos(2 * 3.14159 * (C.y * x + D.y)) * 255,
//         A.z + B.z * std::cos(2 * 3.14159 * (C.z * x + D.z)) * 255,
//         255,
//     };

//     return c;
// }