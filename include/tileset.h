#ifndef TILESET_4_TERMINAL_RENDERER_H
#define TILESET_4_TERMINAL_RENDERER_H

#include <string>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Tileset
{

public:
    char *filename = nullptr;
    unsigned char *data = nullptr;

    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    int TILE_SIZE;

    Tileset(char *filename, int tile_size)
    {
        this->filename = filename;
        this->TILE_SIZE = tile_size;
        this->data = stbi_load(filename, &this->tilesetWidth, &this->tilesetHeight, &this->tilesetChannels, 4);
        if (this->data == nullptr)
            throw std::string("can't load image");

        // std::cout << "Tileset Width: " << this->tilesetWidth << "\n";
        // std::cout << "Tileset Height: " << this->tilesetHeight << "\n";
        // std::cout << "Tileset Channels: " << this->tilesetChannels << "\n";
    }

    void renderTile(int n, int x, int y, Renderer &r)
    {
        int linearWidth = this->TILE_SIZE * n;

        // ((Tw * n) // w) * h
        int j0 = (linearWidth / this->tilesetWidth) * this->TILE_SIZE;

        // ( Tw * n ) % w
        int i0 = (linearWidth % this->tilesetWidth) * this->tilesetChannels;

        int xTemp = x;
        int yTemp = y;
        for (int j = j0; j < j0 + this->TILE_SIZE; ++j)
        {
            int x0 = xTemp;
            for (int i = i0; i < i0 + this->tilesetChannels * this->TILE_SIZE; i += this->tilesetChannels)
            {
                int index = j * this->tilesetWidth * this->tilesetChannels + i;

                uint8_t red = this->data[index];
                uint8_t green = this->data[index + 1];
                uint8_t blue = this->data[index + 2];
                uint8_t alpha = this->data[index + 3];

                r.putPixel(x0, yTemp, {red, green, blue, alpha});
                ++x0;
            }
            ++yTemp;
        }
    }
};

#endif