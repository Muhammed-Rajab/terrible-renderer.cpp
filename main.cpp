#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <cmath>

#include "include/renderer.h"

// *-------------------------------------->
// * MATH
// *-------------------------------------->

float clamp(float min, float max, float val)
{
    // return std::max(min, std::min(max, val));
    if (val <= min)
    {
        return min;
    }
    else if (val >= max)
    {
        return max;
    }
    else
    {
        return val;
    }
}

struct Vec3
{
    float x = 0;
    float y = 0;
    float z = 0;

    void display()
    {
        std::cout << "x: " << this->x << " y: " << this->y << " z: " << this->z << "\n";
    }

    Vec3 add(Vec3 b)
    {
        return {
            this->x + b.x,
            this->y + b.y,
            this->z + b.z,
        };
    }

    Vec3 sub(Vec3 b)
    {
        return {
            this->x - b.x,
            this->y - b.y,
            this->z - b.z,
        };
    }

    Vec3 scale(float scalar)
    {
        return {
            this->x * scalar,
            this->y * scalar,
            this->z * scalar,
        };
    }

    float magnitude()
    {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    void normalize()
    {
        float mag = this->magnitude();
        this->x /= mag;
        this->y /= mag;
        this->z /= mag;
    }

    float dot(Vec3 b)
    {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }

    Vec3 copy()
    {
        return *this;
    }

    Pixel toPixel(int alpha)
    {
        return {
            clamp(0, 255, this->x),
            clamp(0, 255, this->y),
            clamp(0, 255, this->z),
            clamp(0, 255, alpha),
        };
    }
};

Vec3 getUV(float x, float y, const Renderer &r)
{
    return {x / (float)(r.width), y / (float)(r.height), 0};
}

Pixel normalColorToPixel(float r, float g, float b, float a)
{
    return {
        clamp(0.0f, 255.0f, r * 255.0f),
        clamp(0.0f, 255.0f, g * 255.0f),
        clamp(0.0f, 255.0f, b * 255.0f),
        clamp(0.0f, 255.0f, a * 255.0f),
    };
}

struct NormalColor
{
    float r;
    float g;
    float b;
    float a;
};

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Renderer r{96, 96};
    Renderer r{200, 96};

    r.clearScreen();
    r.resetCursor();

    // IN MILLISECONDS
    int DELAY = 0;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAW TEST STUFF HERE

        // * DRAWING CODE GOES HERE --------------------------------------->
        for (int y = 0; y < r.height; ++y)
        {
            for (int x = 0; x < r.width; ++x)
            {
                NormalColor fragColor;
                // ! FLIP THE Y-AXIS
                Vec3 uv = getUV(x, r.height - y - 1, r);
                uv = uv.sub({0.5f, 0.5f, 0.0f}).scale(2.0f);
                uv.x *= r.width / r.height;

                float d = uv.magnitude();

                fragColor = {d, 0.0f, 0.0f, 1.0f};
                //*--------------------------------------->
                r.putPixel(x, y, normalColorToPixel(fragColor.r, fragColor.g, fragColor.b, fragColor.a));
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
    }

    return EXIT_SUCCESS;
}