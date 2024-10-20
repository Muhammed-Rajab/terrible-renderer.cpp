#include <iostream>
#include <cmath>
#include <thread>
#include "renderer.h"

int main()
{
    const int WIDTH = 64;
    const int HEIGHT = 64;
    const int WIDTH_2 = WIDTH / 2;
    const int HEIGHT_2 = HEIGHT / 2;

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

                int xp = static_cast<int>(WIDTH_2 + K1 * ooz * x);
                int yp = static_cast<int>(HEIGHT_2 - K1 * ooz * y);

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

        // * DRAWING GRIDS TO MAKE THINGS EASY TO UNDERSTAND
        for (int x = 0; x < WIDTH; ++x)
        {
            renderer.putPixel(x, HEIGHT_2, {255, 255, 0});
        }

        for (int y = 0; y < HEIGHT; ++y)
        {
            renderer.putPixel(WIDTH_2, y, {255, 0, 0});
        }

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