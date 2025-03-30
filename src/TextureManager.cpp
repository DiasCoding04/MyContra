#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const std::string &filePath, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << filePath
                  << ", Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture from: " << filePath
                  << ", Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}
