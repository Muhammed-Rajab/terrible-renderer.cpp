#include <iostream>
#include <cmath>
#include <thread>
#include "renderer.h"

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

int main()
{
    const int WIDTH = 64;
    const int HEIGHT = 64;
    Renderer renderer{WIDTH, HEIGHT};

    renderer.clearScreen();
    renderer.resetCursor();

    const int DELAY = 16;
    std::size_t frameCount = 0;

    float PHI_SPACING = 0.02;
    float THETA_SPACING = 0.07;
    const double TWO_PI = M_PI * 2.0f;

    float A = 0; // X AXIS ROTATION
    float C = 0; // Z AXIS ROTATION

    float r = 1.0f;
    float d = 2.0f;

    const float K2 = 5;
    const float K1 = renderer.width * K2 * 3 / (8 * (d + r));

    float lightX = 0;
    float lightY = 1;
    float lightZ = -1;
    float lightMag = std::sqrt(lightX * lightX + lightY * lightY + lightZ * lightZ);

    lightX /= lightMag;
    lightY /= lightMag;
    lightZ /= lightMag;

    float donutScaleX = 1.0f;
    float donutScaleY = 1.0f;
    float donutScaleZ = 1.0f;

    float donutTranslateX = 0.0f;
    float donutTranslateY = 0.0f;
    float donutTranslateZ = 7.0f;

    while (true)
    {
        renderer.resetBuffer(Pixel{0, 0, 0});
        //?>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        //? YOUR DRAW CODE GOES HERE
        //?>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        // ZBUFFER SETUP
        float zBuffer[HEIGHT][WIDTH] = {0};
        std::fill(&zBuffer[0][0], &zBuffer[0][0] + HEIGHT * WIDTH, -1e6f);

        // AROUND CENTER FOR TORUS
        for (float phi = 0; phi < TWO_PI; phi += PHI_SPACING)
        {
            // CIRCLE ANGLE
            float cosC = std::cos(C), sinC = std::sin(C);
            float cosA = std::cos(A), sinA = std::sin(A);
            float cosPhi = std::cos(phi), sinPhi = std::sin(phi);

            for (float theta = 0; theta < TWO_PI; theta += THETA_SPACING)
            {
                float cosTheta = std::cos(theta), sinTheta = std::sin(theta);

                // * CALCULATE ROTATED X, Y, Z coordinates of torus
                float x = ((d + r * cosTheta) * (cosC * cosPhi - sinC * sinA * sinPhi) - r * sinC * cosA * sinTheta) * donutScaleX + donutTranslateX;
                float y = (r * cosC * cosA * sinTheta + (d + r * cosTheta) * (sinC * cosPhi + cosC * sinA * sinPhi)) * donutScaleY + donutTranslateY;
                float z = (r * sinA * sinTheta - cosA * sinPhi * (d + r * cosTheta)) * donutScaleZ + donutTranslateZ;
                float ooz = 1.0f / z;

                int xp = static_cast<int>(renderer.width / 2 + K1 * ooz * x);
                int yp = static_cast<int>(renderer.height / 2 - K1 * ooz * y);

                // * CALCULATE NORMAL FOR LUMINANCE
                float nx = cosTheta * (cosC * cosPhi - sinC * sinA * sinPhi) - sinC * cosA * sinTheta;
                float ny = cosTheta * (sinC * cosPhi + cosPhi * sinA * sinPhi) + cosC * cosA * sinTheta;
                float nz = sinA * sinTheta - cosA * sinPhi * cosTheta;
                float nm = std::sqrt(nx * nx + ny * ny + nz * nz);

                nx /= nm;
                ny /= nm;
                nz /= nm;

                float luminance = nx * lightX + ny * lightY + nz * lightZ;

                if (luminance > 0)
                {
                    if (xp >= 0 && xp < renderer.width && yp >= 0 && yp < renderer.height)
                    {
                        if (ooz > zBuffer[yp][xp])
                        {
                            zBuffer[yp][xp] = ooz;
                            unsigned char val = (unsigned char)(luminance * 255);
                            renderer.putPixel(xp, yp, {val, val, val, 255});
                        }
                    }
                }
            }
        }

        A += 0.008;
        C += 0.005;
        //!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        //! YOUR DRAW CODE ENDS HERE
        //!>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

        renderer.swapBuffers();
        renderer.resetCursor();
        renderer.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY)); // Control main loop delay
        ++frameCount;
    }

    return EXIT_SUCCESS;
}