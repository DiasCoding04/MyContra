#include "Camera.h"
#include "Map.h"
#include <algorithm>

Camera::Camera(int width, int height)
    : x(0), y(0)
    , targetX(0), targetY(0)
    , viewportWidth(width)
    , viewportHeight(height)
    , smoothness(5.0f)
    , deadzone(5.0f)
    , maxX(0), maxY(0)
{
}

void Camera::init(const Map& map) {
    maxX = map.width * map.tileWidth - viewportWidth;
    maxY = map.height * map.tileHeight - viewportHeight;
}

void Camera::update(float playerX, float playerY, float deltaTime) {
    // Tính vị trí mục tiêu (giữ player ở center)
    targetX = playerX - viewportWidth / 2;
    targetY = playerY - viewportHeight / 2;

    // Áp dụng deadzone
    float dx = targetX - x;
    float dy = targetY - y;

    if (std::abs(dx) > deadzone) {
        x += dx * smoothness * deltaTime;
    }
    if (std::abs(dy) > deadzone) {
        y += dy * smoothness * deltaTime;
    }

    // Giới hạn camera trong map
    x = std::max(0.0f, std::min(x, static_cast<float>(maxX)));
    y = std::max(0.0f, std::min(y, static_cast<float>(maxY)));
}

SDL_Point Camera::worldToScreen(float worldX, float worldY) const {
    return SDL_Point{
        static_cast<int>(worldX - x),
        static_cast<int>(worldY - y)
    };
}

SDL_Point Camera::screenToWorld(float screenX, float screenY) const {
    return SDL_Point{
        static_cast<int>(screenX + x),
        static_cast<int>(screenY + y)
    };
}

SDL_Rect Camera::getRenderRect(int tileX, int tileY, const Map& map) const {
    SDL_Rect rect;
    rect.x = tileX * map.tileWidth - static_cast<int>(x);
    rect.y = tileY * map.tileHeight - static_cast<int>(y);
    rect.w = map.tileWidth;
    rect.h = map.tileHeight;
    return rect;
}

bool Camera::isInView(int tileX, int tileY, const Map& map) const {
    int worldX = tileX * map.tileWidth;
    int worldY = tileY * map.tileHeight;
    
    return (worldX + map.tileWidth > x &&
            worldX < x + viewportWidth &&
            worldY + map.tileHeight > y &&
            worldY < y + viewportHeight);
}