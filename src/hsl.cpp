#include <cmath>
#include <iostream>

#include "hsl.h"

RGBColor HSLToRGB(int h, float s, float l)
{

    h = h % 360;

    float c = (1 - std::fabs(2 * l - 1)) * s;

    float x = c * (1 - fabs(fmod(h / 60.0f, 2) - 1));

    float R1, G1, B1;

    if (h >= 0 && h < 60)
    {
        R1 = c;
        G1 = x;
        B1 = 0;
    }
    else if (h >= 60 && h < 120)
    {
        R1 = x;
        G1 = c;
        B1 = 0;
    }
    else if (h >= 120 && h < 180)
    {
        R1 = 0;
        G1 = c;
        B1 = x;
    }
    else if (h >= 180 && h < 240)
    {
        R1 = 0;
        G1 = x;
        B1 = c;
    }
    else if (h >= 240 && h < 300)
    {
        R1 = x;
        G1 = 0;
        B1 = c;
    }
    else if (h >= 300 && h < 360)
    {
        R1 = c;
        G1 = 0;
        B1 = x;
    }

    const float m = l - (c / 2);
    return RGBColor{
        static_cast<int>(round((R1 + m) * 255)),
        static_cast<int>(round((G1 + m) * 255)),
        static_cast<int>(round((B1 + m) * 255)),
    };
}

HSLColor RGBToHSL(int r, int g, int b)
{
    // NORMALISE
    double R1 = r / 255.0;
    double B1 = b / 255.0;
    double G1 = g / 255.0;

    // FIND MAX AND MIN
    double Cmax = fmax(R1, fmax(G1, B1));
    double Cmin = fmin(R1, fmin(G1, B1));
    double delta = Cmax - Cmin;

    // CALCULATE L
    double L = (Cmax + Cmin) / 2;

    // CALCULATE S
    double S = 0;
    if (delta == 0)
    {
        S = 0;
    }
    else if (L < 0.5)
    {
        S = delta / (Cmax + Cmin);
    }
    else
    {
        S = delta / (2.0f - Cmax - Cmin);
    }

    // CALCULATE HUE
    double H = 0;
    if (delta == 0)
    {
        H = 0;
    }
    else if (Cmax == R1)
    {
        H = 60 * fmod(((G1 - B1) / delta), 6);
    }
    else if (Cmax == G1)
    {
        H = 60 * (((B1 - R1) / delta) + 2);
    }
    else if (Cmax == B1)
    {
        H = 60 * (((R1 - G1) / delta) + 4);
    }

    if (H < 0)
    {
        H += 360;
    }

    return HSLColor{(int)round(H), S, L};
}

RGBColor HSLToRGB(HSLColor hsl)
{
    return HSLToRGB(hsl.h, hsl.l, hsl.l);
}

HSLColor RGBToHSL(RGBColor rgb)
{
    return RGBToHSL(rgb.r, rgb.g, rgb.b);
}
