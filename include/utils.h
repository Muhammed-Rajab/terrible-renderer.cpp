#ifndef UTILS_4_TERMINAL_RENDERER_H
#define UTILS_4_TERMINAL_RENDERER_H

#include <cstdlib>
#include "hsl.h"
#include "renderer.h"
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

int randomInteger(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

// Pixel HSLtoPixel(int hue, float s, float l)
// {
//     RGBColor col = HSLToRGB(hue, s, l);
//     return {(std::uint8_t)col.r, (std::uint8_t)col.g, (std::uint8_t)col.b};
// }

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

#endif