#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class TextureManager {
public:
    // Hàm tĩnh (static) để load 1 file ảnh thành SDL_Texture*
    static SDL_Texture* LoadTexture(const std::string& filePath, SDL_Renderer* renderer);
};

#endif // TEXTURE_MANAGER_H
