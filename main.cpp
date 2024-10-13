#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <thread>

#include "include/renderer.h"

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Renderer r{96, 96};

    r.clearScreen();
    r.resetCursor();

    // IN MILLISECONDS
    int DELAY = 0;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAW TEST STUFF HERE

        // * DRAWING CODE GOES HERE --------------------------------------->
        r.circleBorder(r.width / 2, r.height / 2, 40, {255, 0, 0, 255});
        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
    }

    return EXIT_SUCCESS;
}