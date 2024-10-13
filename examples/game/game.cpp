#include <iostream>
#include <thread>
#include "map.h"
#include "utils.h"
#include "tileset.h"
#include "renderer.h"

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

// FIXME: MOVE FROM LINUX TO WINDOWS, FOR BETTER APIS
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

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Renderer r{170, 128};
    // Renderer r{64, 64};

    r.clearScreen();
    r.resetCursor();

    // IN MILLISECONDS
    int DELAY = 16;

    Camera cam{0, 0, 0.4, 0.4};

    std::thread listener(keyListener, std::ref(cam), std::ref(r));

    int **bgLayer = Tilemaps::OneD2TwoD(Tilemaps::backgroundLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::backgroundLayer) / sizeof(int));
    int **objLayer = Tilemaps::OneD2TwoD(Tilemaps::objectLayer, Tilemaps::WIDTH, Tilemaps::HEIGHT, sizeof(Tilemaps::objectLayer) / sizeof(int));

    // TODO: IMPLEMENT A WAY TO GET SPRITE BY INDEX
    Tileset ts{"./assets/game/tileset.png", 16};

    int tilesAcross = (r.width / ts.TILE_SIZE) + 2;
    int tilesDown = (r.height / ts.TILE_SIZE) + 2;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAW TEST STUFF HERE

        // * DRAWING CODE GOES HERE --------------------------------------->
        int camTileX = static_cast<int>(cam.x);
        int camTileY = static_cast<int>(cam.y);

        float camOffsetX = cam.x - camTileX;
        float camOffsetY = cam.y - camTileY;

        for (int y = 0; y < tilesDown; ++y)
        {
            for (int x = 0; x < tilesAcross; ++x)
            {
                int tileX = (x * ts.TILE_SIZE) - static_cast<int>(std::round(camOffsetX * ts.TILE_SIZE));
                int tileY = (y * ts.TILE_SIZE) - static_cast<int>(std::round(camOffsetY * ts.TILE_SIZE));

                int mapX = camTileX + x;
                int mapY = camTileY + y;

                if (mapX >= 0 && mapX < Tilemaps::WIDTH && mapY >= 0 && mapY < Tilemaps::HEIGHT)
                {
                    int backgroundTile = bgLayer[mapY][mapX];
                    int objectTile = objLayer[mapY][mapX];

                    if (backgroundTile != 0)
                    {
                        ts.renderTile(backgroundTile - 1, tileX, tileY, r);
                    }
                    if (objectTile != 0)
                    {
                        ts.renderTile(objectTile - 1, tileX, tileY, r);
                    }
                }
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
    }

    // ! FREE TILES
    Tilemaps::deleteTwoDArray(bgLayer, Tilemaps::HEIGHT);
    Tilemaps::deleteTwoDArray(objLayer, Tilemaps::HEIGHT);

    listener.join();

    return EXIT_SUCCESS;
}