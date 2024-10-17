#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <cmath>

#include "include/renderer.h"
#include "include/utils.h"

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Renderer r{96, 96};
    // Renderer r{220, 220};
    // Renderer r{128, 128};
    Renderer r{64, 64};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 150;

    std::size_t frameCount = 0;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAWING CODE GOES HERE --------------------------------------->
        for (int i = 0; i < 10; ++i)
        {
            r.circleFill(randomInteger(0, r.width), randomInteger(0, r.height), randomInteger(0, 24), {
                                                                                                          (unsigned char)randomInteger(0, 255),
                                                                                                          (unsigned char)randomInteger(0, 255),
                                                                                                          (unsigned char)randomInteger(0, 255),
                                                                                                      });
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay

        ++frameCount;
    }

    return EXIT_SUCCESS;
}