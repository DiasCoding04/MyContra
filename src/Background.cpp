#include "Background.h"
#include <stdio.h>

Background::Background() : bgTexture(nullptr), width(0), height(0) {}

Background::~Background() {
    cleanup();
}

bool Background::loadBackground(SDL_Renderer* renderer, const char* path) {
    // Giải phóng texture cũ nếu có
    cleanup();

    // Load ảnh từ file
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Failed to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
        return false;
    }

    // Tạo texture từ surface
    bgTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!bgTexture) {
        printf("Failed to create texture! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    // Lấy kích thước của ảnh
    width = surface->w;
    height = surface->h;

    // Giải phóng surface vì đã có texture
    SDL_FreeSurface(surface);
    return true;
}

void Background::render(SDL_Renderer* renderer, float cameraX) {
    if (!bgTexture) return;

    // Tính toán vị trí render dựa trên camera
    int x = static_cast<int>(-cameraX * 0.7f); // Scroll speed 70% của camera

    SDL_Rect dstRect = {
        x,                      // x position
        0,                      // y position
        width,                  // width
        height                  // height
    };

    SDL_RenderCopy(renderer, bgTexture, NULL, &dstRect);

    // Render thêm một phần nếu cần để tránh khoảng trống
    if (x > 0) {
        dstRect.x = x - width;
        SDL_RenderCopy(renderer, bgTexture, NULL, &dstRect);
    }
    else if (x + width < 950) { // Giả sử màn hình rộng 950px
        dstRect.x = x + width;
        SDL_RenderCopy(renderer, bgTexture, NULL, &dstRect);
    }
}

void Background::cleanup() {
    if (bgTexture) {
        SDL_DestroyTexture(bgTexture);
        bgTexture = nullptr;
    }
    width = 0;
    height = 0;
}