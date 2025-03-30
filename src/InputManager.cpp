#include "InputManager.h"

// Định nghĩa các biến static
bool InputManager::moveUp = false;
bool InputManager::moveDown = false;
bool InputManager::moveLeft = false;
bool InputManager::moveRight = false;
bool InputManager::shoot = false;
bool InputManager::shootKeyDown = false;  // Thêm biến này

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
                case SDLK_w: moveUp = true; break;
                case SDLK_s: moveDown = true; break;
                case SDLK_a: moveLeft = true; break;
                case SDLK_d: moveRight = true; break;
                case SDLK_SPACE:
                    if (!shootKeyDown) {  // Chỉ set shoot = true nếu phím chưa được nhấn
                        shoot = true;
                        shootKeyDown = true;
                    }
                    break;
            }
        }
        else if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_w: moveUp = false; break;
                case SDLK_s: moveDown = false; break;
                case SDLK_a: moveLeft = false; break;
                case SDLK_d: moveRight = false; break;
                case SDLK_SPACE:
                    shoot = false;
                    shootKeyDown = false;  // Reset trạng thái khi thả phím
                    break;
            }
        }
    }
}

void InputManager::HandleEvent(const SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_w: moveUp = true; break;
            case SDLK_s: moveDown = true; break;
            case SDLK_a: moveLeft = true; break;
            case SDLK_d: moveRight = true; break;
            case SDLK_SPACE:
                if (!shootKeyDown) {
                    shoot = true;
                    shootKeyDown = true;
                }
                break;
        }
    }
    else if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_w: moveUp = false; break;
            case SDLK_s: moveDown = false; break;
            case SDLK_a: moveLeft = false; break;
            case SDLK_d: moveRight = false; break;
            case SDLK_SPACE:
                shoot = false;
                shootKeyDown = false;
                break;
        }
    }
}