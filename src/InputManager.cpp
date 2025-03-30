#include "InputManager.h"

// Khai báo, khởi tạo các biến static
bool InputManager::moveUp    = false;
bool InputManager::moveDown  = false;
bool InputManager::moveLeft  = false;
bool InputManager::moveRight = false;

void InputManager::HandleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            // Ở đây ta chưa có biến global running, có thể xử lý trong main
            // Hoặc tạm thời ignore, tuỳ thiết kế
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_w: moveUp    = true; break;
                case SDLK_s: moveDown  = true; break;
                case SDLK_a: moveLeft  = true; break;
                case SDLK_d: moveRight = true; break;
            }
        }
        else if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_w: moveUp    = false; break;
                case SDLK_s: moveDown  = false; break;
                case SDLK_a: moveLeft  = false; break;
                case SDLK_d: moveRight = false; break;
            }
        }
    }
}

void InputManager::HandleEvent(const SDL_Event &e)
{
    // Bắt từng event được truyền từ main.cpp
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_w: moveUp = true; break;
            case SDLK_s: moveDown = true; break;
            case SDLK_a: moveLeft = true; break;
            case SDLK_d: moveRight = true; break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_w: moveUp = false; break;
            case SDLK_s: moveDown = false; break;
            case SDLK_a: moveLeft = false; break;
            case SDLK_d: moveRight = false; break;
        }
    }
}
