#include <cmath>
#include <thread>
#include <iostream>
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float clamp(float min, float max, float val)
{
    return std::max(min, std::min(max, val));
}

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

Vec3 calculateSphereNormal(float x, float y, float radius)
{
    float nx = float(x) / radius;
    float ny = float(y) / radius;
    return Vec3{
        nx,
        ny,
        std::sqrt(1.0f - (nx * nx + ny * ny)),
    };
}

float getSpecularComponent(Vec3 &normal, Vec3 &lightDirection, float shininess)
{
    float TWO_N_dot_L = 2 * (normal.dot(lightDirection));
    Vec3 reflectionVector = (normal.scale(TWO_N_dot_L)).sub(lightDirection);

    // * VIEW DIRECTION VECTOR
    Vec3 viewVector = {0, 0, 1};

    // * GET SPECULAR FACTOR
    float specular_factor = std::max(0.0f, viewVector.dot(reflectionVector));
    float specular_component = std::pow(specular_factor, shininess);

    return specular_component;
}

float getDiffusePower(Vec3 &normal, Vec3 &lightDirection)
{
    return std::max(0.0f, normal.dot(lightDirection));
}

std::pair<float, float> getUV(Vec3 normal)
{
    float u = 0.5 + std::atan2(normal.z, normal.x) / (2 * M_PI);
    float v = 0.5 - normal.y * 0.5f;

    return {u, v};
}

std::pair<int, int> getTextCoords(Vec3 &normal, int textureWidth, int textureHeight)
{
    auto [u, v] = getUV(normal);

    u = clamp(0.0f, 1.0f, u);
    v = clamp(0.0f, 1.0f, v);

    int texX = u * (textureWidth - 1);
    int texY = v * (textureHeight - 1);

    texX = clamp(0, textureWidth - 1, texX);
    texY = clamp(0, textureHeight - 1, texY);

    return {texX, texY};
}

Pixel getTexturedColor(Vec3 &normal, float intensity, unsigned char *texture, int width, int height, int channels, Renderer &r)
{
    // TODO: TEXTURE
    auto [texX, texY] = getTextCoords(normal, width, height);

    int index = texY * width * channels + texX * channels;

    Vec3 colorV = {
        texture[index],
        texture[index + 1],
        texture[index + 2],
    };

    return colorV.scale(intensity).toPixel(255);
}

Pixel getProbabilisticColor(float intensity)
{
    Vec3 colorV;
    int prob = rand() % 100;
    if (prob < 33)
    {
        colorV = {103, 11, 156};
    }
    else if (prob < 66)
    {
        colorV = {255, 255, 255};
    }
    else
    {
        colorV = {135, 224, 18};
    }
    return colorV.scale(intensity).toPixel(255);
}

Pixel getFixedColor(Vec3 color, float intensity)
{
    return color.scale(intensity).toPixel(255);
}

int main()
{
    // ! CHANGE THE INDEX TO GET EXAMPLES
    int currentExample = 2;

    Pixel backgrounds[] = {
        {236, 197, 137, 255},
        {19, 3, 28, 255},
        {35, 35, 35, 255},
        {19, 3, 28, 255},
    };

    // ! CHANGE THIS TO CHANGE THE BUFFER SIZE
    Renderer r{96, 96};

    r.clearScreen();
    r.resetCursor();

    int DELAY = 16;

    int W2 = r.width / 2;
    int H2 = r.width / 2;

    double angle = -1.57f;
    double zoom = 0.785f;

    // * LOADING TEXTURE
    int width = 0, height = 0, channels = 0;
    unsigned char *texture = stbi_load("assets/sphere/texture.jpg", &width, &height, &channels, 3);

    while (true)
    {
        r.resetBuffer(Pixel{0, 0, 0});

        // * DRAWING CODE GOES HERE --------------------------------------->

        // * SIMPLE BOUNCING EFFECT
        float radius = 44.0f;
        // float radius = 44.0f;
        // float radius = 44.0f * std::sin(clamp(0.785, 2.35, zoom));

        Vec3 light = {
            -300.0f * std::cos(angle),
            -300,
            -300.0f * std::sin(angle),
        };

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

                    // * AMBIENT LIGHTING
                    float ambientLightingPower = 0.15f;

                    // * DIFFUSE LIGHTING
                    float diffusePower = getDiffusePower(normal, lightDirection);

                    // * SPECULAR LIGHTING
                    float specularComponent = getSpecularComponent(normal, lightDirection, 38.0f);

                    // * FINAL INTENSITY
                    float globalIntensity = 0.9f;
                    float finalIntensity = globalIntensity * (diffusePower + specularComponent + ambientLightingPower);

                    // * SHADING PART
                    Pixel color = {0, 0, 0};

                    switch (currentExample)
                    {
                    case 0:
                        color = getTexturedColor(normal, finalIntensity, texture, width, height, channels, r);
                        break;
                    case 1:
                        color = getProbabilisticColor(finalIntensity);
                        break;
                    case 2:
                        color = getFixedColor({255, 255, 255}, finalIntensity);
                        break;
                    case 3:
                        color = getFixedColor({103, 11, 156}, finalIntensity);
                        break;
                    }

                    r.putPixel(W2 + x, H2 + y, color);
                }
                else
                {
                    // ! POTENTIAL ERROR
                    r.putPixel(W2 + x, H2 + y, backgrounds[currentExample]);
                }
            }
        }

        //*---------------------------------------------------------------->
        r.swapBuffers();
        r.resetCursor();
        r.render();

        angle += 0.05;
        zoom += 0.01;

        if (zoom > 2.35)
        {
            zoom = 0.785;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }

    return EXIT_SUCCESS;
}