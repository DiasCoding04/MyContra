#pragma once
#include <SDL.h>
#include "GameTypes.h"

class Camera {
private:
    float x, y;
    float targetX, targetY;
    int viewportWidth;
    int viewportHeight;
    float smoothness;
    float deadzone;
    int maxX;
    int maxY;

public:
    Camera(int width = 950, int height = 640);
    ~Camera() = default;

    void init(const Map& map);
    void update(float playerX, float playerY, float deltaTime);
    SDL_Point worldToScreen(float worldX, float worldY) const;
    SDL_Point screenToWorld(float screenX, float screenY) const;
    SDL_Rect getRenderRect(int tileX, int tileY, const Map& map) const;
    bool isInView(int tileX, int tileY, const Map& map) const;

    float getX() const { return x; }
    float getY() const { return y; }
};