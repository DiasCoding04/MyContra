#include "ResourceManager.h"
#include <iostream>

SDL_Texture* ResourceManager::LoadTexture(const std::string &filePath, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!tex) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    }
    return tex;
}
