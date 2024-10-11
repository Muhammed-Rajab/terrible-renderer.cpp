#include <cmath>
#include <thread>
#include <iostream>
#include "renderer.h"

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3 add(Vec3 &b)
    {
        return {
            this->x + b.x,
            this->y + b.y,
            this->z + b.z,
        };
    }

    Vec3 sub(Vec3 &b)
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

    float dot(Vec3 &b)
    {
        return this->x * b.x + this->y * b.y + this->z * b.z;
    }

    Vec3 copy()
    {
        return *this;
    }
};

Vec3 calculateSphereNormal(float x, float y, float radius)
{
    float newX = float(x) / radius;
    float newY = float(y) / radius;
    return Vec3{
        newX,
        newY,
        std::sqrt(1.0f - (newX * newX + newY * newY)),
    };
}

int main()
{

    // Renderer r{170, 128};
    // Renderer r{200, 200};
    // Renderer r{64, 64};
    // Renderer r{96, 96};
    Renderer r{220, 220};
    // Renderer r{32, 32};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 16;

    int frameCount = 0;

    int W2 = r.width / 2;
    int H2 = r.width / 2;

    double angle = -1.57;

    while (true)
    {
        ++frameCount;

        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAWING CODE GOES HERE --------------------------------------->
        Vec3 light = {
            -300.0f * std::cos(angle),
            -300,
            -400.0f * std::sin(angle),
        };

        float radius = 100;

        for (int y = -H2; y < H2; ++y)
        {
            for (int x = -W2; x < W2; ++x)
            {
                if (x * x + y * y <= radius * radius)
                {
                    Vec3 pos = {x, y, 0};

                    // * CALCULATE NORMAL
                    Vec3 normal = calculateSphereNormal(x, y, radius);
                    normal.normalize();

                    // * GET LIGHT DIRECTION VECTOR
                    Vec3 lightDirection = light.sub(pos);
                    lightDirection.normalize();

                    // * GET REFLECTION VECTOR
                    float TWO_N_dot_L = 2 * (normal.dot(lightDirection));
                    Vec3 reflectionVector = (normal.scale(TWO_N_dot_L)).sub(lightDirection);

                    // * VIEW DIRECTION VECTOR
                    Vec3 viewVector = {0, 0, 1};
                    float V_dot_R = viewVector.dot(reflectionVector);

                    // * GET SPECULAR FACTOR
                    float shininess = 40.0f;
                    float specular_factor = std::max(0.0f, V_dot_R);
                    float specular_component = std::pow(specular_factor, shininess);

                    // * CALCULATE DIFFUSE LIGHTING POWER
                    float diffuse_power = std::max(0.0f, normal.dot(lightDirection));
                    ;

                    // ! NORMAL MAP VISUALIZATION
                    // Pixel color = {
                    //     (unsigned char)(std::max(0.0f, std::min(255.0f, (nx + 1) * 0.5f * 255.0f))),
                    //     (unsigned char)(std::max(0.0f, std::min(255.0f, (ny + 1) * 0.5f * 255.0f))),
                    //     (unsigned char)(std::max(0.0f, std::min(255.0f, (nz + 1) * 0.5f * 255.0f))),
                    //     (255),
                    // };

                    //* FINAL INTENSITY
                    float globalIntensity = .9f;
                    float finalIntensity = globalIntensity * (diffuse_power + specular_component + 0.1f);

                    Pixel color = {
                        (unsigned char)(std::max(0.0f, std::min(255.0f, 103 * finalIntensity))),
                        (unsigned char)(std::max(0.0f, std::min(255.0f, 11 * finalIntensity))),
                        (unsigned char)(std::max(0.0f, std::min(255.0f, 156 * finalIntensity))),
                        255,
                    };

                    r.putPixel(W2 + x, H2 + y, color);
                }
                else
                {
                    r.putPixel(W2 + x, H2 + y, {20, 2, 31, 255});
                }
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        angle += 0.01;

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }

    return EXIT_SUCCESS;
}