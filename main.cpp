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

    Vec3 add(float val)
    {
        return {
            this->x + val,
            this->y + val,
            this->z + val,
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

float smin(float a, float b, float k)
{
    return (a + b - std::sqrt(std::pow(a - b, 2.0f) + k * k)) * 0.5f;
}

//*------------------------------------------>
//* COLORS
//*------------------------------------------>
Vec3 color(float t, Vec3 a, Vec3 b, Vec3 c, Vec3 d)
{
    // C scales B elementwise
    return a.add(b.scale(c.scale(t).add(d).scale(2 * M_PI).cos()));
}

//*------------------------------------------->
//* SDFs
//*------------------------------------------->
float sdSphere(Vec3 p, float r)
{
    return p.magnitude() - r;
}

float sdBox(Vec3 p, Vec3 b)
{
    Vec3 q = p.abs().sub(b);
    return q.max(0.0f).magnitude() + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
}

float map(Vec3 p, std::size_t frameCount)
{
    Vec3 spherePos = Vec3{3.0f * std::sin(frameCount * 0.05f), 0.0f, 0.0f};
    float sphere = sdSphere(p.sub(spherePos), .9f);

    Vec3 boxPos = Vec3{0.0f, 0.0f, 0.0f};
    float box = sdBox(p.sub(boxPos), Vec3{0.75f, 0.75f, 0.75f});

    float ground = p.y + 0.75f;

    // return std::min(ground, smin(box, sphere, 0.75f));
    return smin(ground, smin(box, sphere, 0.75f), .1f);
    // return std::min(ground, std::max(box, -sphere));
}

int main()
{
    // ! SEEDING
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Renderer r{96, 96};
    // Renderer r{220, 220};
    Renderer r{128, 128};
    // Renderer r{64, 64};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 16;

    std::size_t frameCount = 0;

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

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
                //?->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                //  * INITIALIZATION
                Vec3 ro = {0, 0, -3};
                Vec3 rd = {uv.x, uv.y, 1};
                rd.normalize();

                // Vec3 col{1.0f, 1.0f, 1.0f};

                // Ray matching
                float t = 0.0f;

                for (int i = 0; i < 80; ++i)
                {

                    Vec3 p = ro.add(rd.scale(t));

                    float d = map(p, frameCount);

                    t += d;

                    // col = Vec3{1.0f, 1.0f, 1.0f}.scale(i / 80.0f);

                    if (d < 0.001 || t > 100.0f)
                        break;
                }

                Vec3 col{0.1f, 0.1f, 0.1f};
                if (t > 5.0f)
                {
                    col = Vec3{0.1f, 0.1f, 0.1f};
                }
                else
                {
                    // col = Vec3{1.0f, 1.0f, 1.0f}.scale(t * 0.1f);
                    col = color(t,
                                {0.5f, 0.5f, 0.5f},
                                {0.5f, 0.5f, 0.5f},
                                {1.0f, 1.0f, 1.0f},
                                {0.00f, 0.10f, 0.20f});

                    // col = color(t,
                    //             {0.5f, 0.5f, 0.5f},
                    //             {0.5f, 0.5f, 0.5f},
                    //             {1.0f, 1.0f, 1.0f},
                    //             {0.00f, 0.10f, 0.20f});
                }

                // col = col.scale(t * 0.1f);

                fragColor = col.toNormalColor(1.0f);
                // fragColor = Vec3{1.0f, 1.0f, 1.0f}.scale((1.0f / t)).toNormalColor(1.0f);
                // fragColor = uv.toNormalColor(1.0f);
                //?->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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
    }

    return EXIT_SUCCESS;
}