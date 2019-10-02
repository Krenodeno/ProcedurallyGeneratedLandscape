

#include "image.h"
#include <cstdio>
#include <string>
#include <algorithm>
#include <cmath>

#ifdef GK_MACOS
#include <SDL2_image/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif


Image::Image( const char *filename )
{
    // importer le fichier en utilisant SDL_image
    SDL_Surface *surface= IMG_Load(filename);
    if(surface == NULL)
    {
        printf("[error] loading image '%s'... sdl_image failed.\n", filename);
        return;
    }
    
    // verifier le format, rgb ou rgba
    const SDL_PixelFormat format= *surface->format;
    if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
    {
        printf("[error] loading image '%s'... format failed. (bpp %d)\n", filename, format.BitsPerPixel);
        SDL_FreeSurface(surface);
        return;
    }
    
    int width= surface->w;
    int height= surface->h;
    int channels= (format.BitsPerPixel == 32) ? 4 : 3;

    m_data = vector<vector<float> >(width*height, {0,0,0});
    m_width = width;
    m_height = height;

    printf("loading image '%s' %dx%d %d channels...\n", filename, width, height, channels);

    // converti les donnees en pixel rgba, et retourne l'image, origine en bas a gauche.
    if(format.BitsPerPixel == 32)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < width; x++)
            {
                Uint8 r= pixel[format.Rshift / 8];
                Uint8 g= pixel[format.Gshift / 8];
                Uint8 b= pixel[format.Bshift / 8];
                Uint8 a= pixel[format.Ashift / 8];

                m_data[y * m_width + x]= {(float) r / 255.f, (float) g / 255.f, (float) b / 255.f, (float) a / 255.f};
                pixel= pixel + format.BytesPerPixel;
            }
        }
    }

    else if(format.BitsPerPixel == 24)
    {
        int py= 0;
        for(int y= height -1; y >= 0; y--, py++)
        {
            Uint8 *pixel= (Uint8 *) surface->pixels + py * surface->pitch;

            for(int x= 0; x < surface->w; x++)
            {
                const Uint8 r= pixel[format.Rshift / 8];
                const Uint8 g= pixel[format.Gshift / 8];
                const Uint8 b= pixel[format.Bshift / 8];

                m_data[y * m_width + x] = {(float) r / 255.f, (float) g / 255.f, (float) b / 255.f};
                pixel= pixel + format.BytesPerPixel;
            }
        }
    }

    SDL_FreeSurface(surface);
}