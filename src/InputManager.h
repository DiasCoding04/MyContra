#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL.h>

class InputManager {
public:
    // Các biến static mô phỏng WASD
    static bool moveUp;
    static bool moveDown;
    static bool moveLeft;
    static bool moveRight;
    static bool shoot;

    // Hàm xử lý event, set các biến bool tương ứng
    static void HandleEvents();
    static void HandleEvent(const SDL_Event &e);
};

#endif
