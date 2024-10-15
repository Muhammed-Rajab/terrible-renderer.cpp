#include <iostream>
#include <fstream>
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

struct NormalColor
{
    float r;
    float g;
    float b;
    float a;
};

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

    Vec3 sub(float val)
    {
        return {
            this->x - val,
            this->y - val,
            this->z - val,
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

    Vec3 scale(Vec3 b)
    {
        return {
            this->x * b.x,
            this->y * b.y,
            this->z * b.z,
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

    Vec3 abs()
    {
        return {
            std::fabs(this->x),
            std::fabs(this->y),
            std::fabs(this->z),
        };
    }

    float dot(Vec3 b)
    {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }

    Vec3 cos()
    {
        return {
            std::cos(this->x),
            std::cos(this->y),
            std::cos(this->z),
        };
    }

    Vec3 min(float val)
    {
        return {
            std::min(this->x, val),
            std::min(this->y, val),
            std::min(this->z, val),
        };
    }

    Vec3 max(float val)
    {
        return {
            std::max(this->x, val),
            std::max(this->y, val),
            std::max(this->z, val),
        };
    }

    Vec3 fract()
    {
        return {
            this->x - std::floor(this->x),
            this->y - std::floor(this->y),
            this->z - std::floor(this->z),
        };
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

    NormalColor toNormalColor(float alpha)
    {
        return {
            clamp(0.0f, 1.0f, this->x),
            clamp(0.0f, 1.0f, this->y),
            clamp(0.0f, 1.0f, this->z),
            clamp(0.0f, 1.0f, alpha),
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

float smoothstep(float edge0, float edge1, float x)
{
    x = clamp(0.0f, 1.0f, (x - edge0) / (edge1 - edge0));
    return x * x * (3 - 2 * x);
}

float step(float threshold, float val)
{
    if (val <= threshold)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int sign(float x)
{
    if (x < 0)
    {
        return -1;
    }
    else if (x > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

float fract(float val)
{
    return val - std::floor(val);
}

//*------------------------------------------->
//* SDFs
//*------------------------------------------->
float sphere(Vec3 p, float r)
{
    return p.magnitude() - r;
}

// TODO: ADD A HEART SDF FOR AJITH
float hexagram(Vec3 p, float r)
{
    // Define the constants from the GLSL version
    Vec3 k1 = {-0.5f, 0.8660254038f, 0.0f}; // First 2D vector from vec4
    Vec3 k2 = {0.8660254038f, -0.5f, 0.0f}; // Swapped components of the same vec4 for xy and yx

    // Absolute value of p (mirror across x and y axes)
    p = p.abs();

    // First transformation
    float dot1 = std::min(k1.dot(p), 0.0f);
    p = p.sub(k1.scale(2.0f * dot1));

    // Second transformation
    float dot2 = std::min(k2.dot(p), 0.0f);
    p = p.sub(k2.scale(2.0f * dot2));

    // Clamp x coordinate and subtract r from y
    float clampX = clamp(r * 0.5773502692f, r * 1.7320508076f, p.x); // r * k.z and r * k.w
    p = {p.x - clampX, p.y - r, 0.0f};

    // Return the distance, with sign based on y coordinate
    return p.magnitude() * sign(p.y);
}

float box(Vec3 p, Vec3 b)
{
    Vec3 q = p.abs().sub(b);
    return q.max(0.0f).magnitude() + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
}

//*------------------------------------------>
//* COLORS
//*------------------------------------------>
Vec3 color(float t, Vec3 a, Vec3 b, Vec3 c, Vec3 d)
{
    // C scales B elementwise
    return a.add(b.scale(c.scale(t).add(d).scale(2 * M_PI).cos()));
}

void shaderKishmishuExample1()
{
    Renderer r{96, 96};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 16;

    std::size_t frameCount = 0;

    const std::size_t TARGET_FPS = 60;
    const float DURATION_IN_SECONDS = 15.0f;
    const std::size_t MAX_FRAMES = std::round(TARGET_FPS * DURATION_IN_SECONDS);

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

                Vec3 uv0 = uv.copy();

                Vec3 fColor{0.0f};
                for (int i = 0; i < 3; ++i)
                {

                    uv = uv.scale(1.61803399f).fract().sub({0.5f, 0.5f});
                    // uv = uv.scale(4.0f).fract().sub({0.5f, 0.5f});

                    float d = sphere(uv, 0.5f);
                    // float d1 = sphere(uv.sub({0.5f}), 0.5f);
                    // float d2 = box(uv.sub({-0.25f}), {0.5f, 0.5f, 0.5f});

                    // float d = std::min(d1, d2);

                    Vec3 col = color(uv0.magnitude() + frameCount * 0.001f + i * 0.1,
                                     {0.5f, 0.5f, 0.5f},
                                     {0.5f, 0.5f, 0.5f},
                                     {1.0f, 1.0f, 1.0f},
                                     {0.00f, 0.10f, 0.20f});

                    // Vec3 col{1.0f, 0.5f, 2.0f};
                    d = std::sin(d * 8.0f + frameCount * 0.05f) / 8.0f;
                    d = std::fabs(d);

                    d = 0.009f / d;
                    d = std::pow(d, 1.2f);
                    // d = 0.01f / d;

                    fColor = fColor.add(col.scale(d));
                    // col = col.scale(d);
                }

                fragColor = fColor.toNormalColor(1.0f);
                // fragColor = uv.toNormalColor(1.0f);
                //*--------------------------------------->
                r.putPixel(x, y, normalColorToPixel(fragColor.r, fragColor.g, fragColor.b, fragColor.a));
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        // * SAVE THE FRAME TO FILE
        // std::ofstream file(std::string("./captures/") + std::to_string(frameCount));
        // file << r.render();
        // file.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay

        ++frameCount;

        // if (frameCount == MAX_FRAMES)
        //     break;
    }
}

void shaderHeartExample()
{
    Renderer r{96, 96};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 16;

    std::size_t frameCount = 0;

    const std::size_t TARGET_FPS = 60;
    const float DURATION_IN_SECONDS = 15.0f;
    const std::size_t MAX_FRAMES = std::round(TARGET_FPS * DURATION_IN_SECONDS);

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

                fragColor = uv.toNormalColor(1.0f);
                //*--------------------------------------->
                r.putPixel(x, y, normalColorToPixel(fragColor.r, fragColor.g, fragColor.b, fragColor.a));
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        // * SAVE THE FRAME TO FILE
        // std::ofstream file(std::string("./captures/") + std::to_string(frameCount));
        // file << r.render();
        // file.close();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay

        ++frameCount;

        // if (frameCount == MAX_FRAMES)
        //     break;
    }
}

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // shaderKishmishuExample1();
    shaderHeartExample();

    return EXIT_SUCCESS;
}