#ifndef HSL_4_CAMOUFLAGE_H
#define HSL_4_CAMOUFLAGE_H

#include <cmath>
#include <sstream>
#include <iostream>

struct HSLColor
{
    int h = 0;
    double s = 0.0;
    double l = 0.0;

    std::string string()
    {
        std::ostringstream oss;
        oss << "h: " << h << " | " << "s: " << s << " | " << "l: " << l;
        return oss.str();
    }
};

struct RGBColor
{
    int r = 0;
    int g = 0;
    int b = 0;

    std::string string()
    {
        std::ostringstream oss;
        oss << "r: " << r << " | " << "g: " << g << " | " << "b: " << b;
        return oss.str();
    }
};

RGBColor HSLToRGB(int h, float s, float l);
RGBColor HSLToRGB(HSLColor hsl);

HSLColor RGBToHSL(int r, int g, int b);
HSLColor RGBToHSL(RGBColor rgb);

#endif