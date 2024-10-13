#ifndef TILESET_4_TERMINAL_RENDERER_H
#define TILESET_4_TERMINAL_RENDERER_H

#include <string>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Tileset
{

public:
    unsigned char *data = nullptr;

    int tilesetWidth = 0;
    int tilesetHeight = 0;
    int tilesetChannels = 0;
    int TILE_SIZE = 0;

    // TODO: IMPLEMENT A WAY TO LOAD THESE TILESET TO A PIXEL ARRAY
    std::vector<std::vector<Pixel>> pixels;

    Tileset(const char *filename, int tile_size)
    {
        this->TILE_SIZE = tile_size;
        this->data = stbi_load(filename, &this->tilesetWidth, &this->tilesetHeight, &this->tilesetChannels, 4);
        if (this->data == nullptr)
            throw std::runtime_error("can't load image in tileset constructor");
        // std::cout << "Tileset Width: " << this->tilesetWidth << "\n";
        // std::cout << "Tileset Height: " << this->tilesetHeight << "\n";
        // std::cout << "Tileset Channels: " << this->tilesetChannels << "\n";

        // * INITIALIZE PIXEL VECTOR
        pixels = std::vector<std::vector<Pixel>>(this->tilesetHeight);
        for (int i = 0; i < this->tilesetHeight; ++i)
        {
            this->pixels.at(i).reserve(this->tilesetWidth);
        }

        // * ADDING PIXELS TO PIXEL VECTOR
        for (int y = 0; y < this->tilesetHeight; ++y)
        {
            for (int x = 0; x < this->tilesetWidth; ++x)
            {
                int index = y * this->tilesetWidth * this->tilesetChannels + x * this->tilesetChannels;
                Pixel p = {
                    this->data[index],
                    this->data[index + 1],
                    this->data[index + 2],
                    this->data[index + 3],
                };
                this->pixels.at(y).push_back(p);
            }
        }
    }

    ~Tileset()
    {
        if (this->data != nullptr)
        {
            stbi_image_free(this->data);
            this->data = nullptr;
        }
    }

    void renderTile(int n, int x, int y, Renderer &r)
    {
        int i0 = (this->TILE_SIZE * n) % this->tilesetWidth;
        int j0 = ((this->TILE_SIZE * n) / this->tilesetWidth) * this->TILE_SIZE;

        // std::cout << "n: " << n << "\n";
        // std::cout << "i0: " << i0 << "\n";
        // std::cout << "j0: " << j0 << "\n";

        for (int j = 0; j < this->TILE_SIZE; ++j)
        {
            int yIndex = j0 + j;
            for (int i = 0; i < this->TILE_SIZE; ++i)
            {
                int xIndex = i0 + i;
                // std::cout << "x: " << xIndex << " y: " << yIndex << "\n";
                r.putPixel(x + i, y + j, this->pixels.at(yIndex).at(xIndex));
            }
        }
    }
};

#endif