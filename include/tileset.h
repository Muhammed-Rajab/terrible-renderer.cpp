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

// class Tileset
// {

// public:
//     unsigned char *data = nullptr;

//     int tilesetWidth = 0;
//     int tilesetHeight = 0;
//     int tilesetChannels = 0;
//     int TILE_SIZE = 0;

//     // TODO: IMPLEMENT A WAY TO LOAD THESE TILESET TO A PIXEL ARRAY
//     Pixel *tileset = nullptr;

//     Tileset(const char *filename, int tile_size)
//     {
//         this->TILE_SIZE = tile_size;
//         this->data = stbi_load(filename, &this->tilesetWidth, &this->tilesetHeight, &this->tilesetChannels, 4);
//         if (this->data == nullptr)
//             throw std::runtime_error("can't load image in tileset constructor");
//         // std::cout << "Tileset Width: " << this->tilesetWidth << "\n";
//         // std::cout << "Tileset Height: " << this->tilesetHeight << "\n";
//         // std::cout << "Tileset Channels: " << this->tilesetChannels << "\n";

//         // * INITIALIZE PIXEL ARRAY
//         // int width = this->tilesetWidth;
//         // int height = this->tilesetHeight;
//         // texture = new Pixel[this];
//     }

//     ~Tileset()
//     {
//         if (this->data != nullptr)
//         {
//             stbi_image_free(this->data);
//             this->data = nullptr;
//         }
//     }

//     void renderTile(int n, int x, int y, Renderer &r)
//     {
//         // FIXME: TILE RENDERING LOGIC IS FAULTY
//         int linearWidth = this->TILE_SIZE * n;

//         // ((Tw * n) // w) * h
//         int j0 = (linearWidth / this->tilesetWidth) * this->TILE_SIZE;

//         // ( Tw * n ) % w
//         int i0 = (linearWidth % this->tilesetWidth) * this->tilesetChannels;

//         int xTemp = x;
//         int yTemp = y;
//         for (int j = j0; j < j0 + this->TILE_SIZE; ++j)
//         {
//             int x0 = xTemp;
//             for (int i = i0; i < i0 + this->tilesetChannels * this->TILE_SIZE; i += this->tilesetChannels)
//             {
//                 int index = j * this->tilesetWidth * this->tilesetChannels + i;

//                 uint8_t red = this->data[index];
//                 uint8_t green = this->data[index + 1];
//                 uint8_t blue = this->data[index + 2];
//                 uint8_t alpha = this->data[index + 3];

//                 r.putPixel(x0, yTemp, {red, green, blue, alpha});
//                 ++x0;
//             }
//             ++yTemp;
//         }
//     }
// };

#endif