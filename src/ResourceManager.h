#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class ResourceManager {
public:
    // Hàm load 1 file ảnh thành SDL_Texture*
    static SDL_Texture* LoadTexture(const std::string &filePath, SDL_Renderer* renderer);
};

#endif
