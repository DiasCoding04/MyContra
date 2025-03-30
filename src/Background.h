#pragma once
#include <SDL.h>
#include <SDL_image.h>

class Background {
private:
    SDL_Texture* bgTexture;
    int width;
    int height;

public:
    Background();
    ~Background();
    
    bool loadBackground(SDL_Renderer* renderer, const char* path);
    void render(SDL_Renderer* renderer, float cameraX = 0);
    void cleanup();
};