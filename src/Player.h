#pragma once

#include <SDL.h>
#include <vector>
#include "Map.h"
#include "Camera.h"


class Player {
public:
    Player();
    ~Player();

    void init(SDL_Renderer* renderer, float startX, float startY,
              const char* texturePath);
    void update(float deltaTime, int screenWidth, int screenHeight,
                const Map& map, const Camera& camera);
    void render(SDL_Renderer* renderer, const Camera& camera);

    // Getters cho vị trí
    float getX() const { return m_x; }
    float getY() const { return m_y; }



private:
    bool isOnGround(const Map& map);
    bool isCollidingHorizontally(float newX, float checkY, const Map& map);
    bool isCollidingVertically(float checkX, float newY, const Map& map);

    // Biến di chuyển
    bool m_touchingLeft;
    bool m_touchingRight;
    float m_x, m_y;
    float m_speed;
    SDL_Texture* m_texture;
    int m_width, m_height;

    // Biến nhảy
    bool m_jumpKeyReleased;
    bool m_isJumping;
    float m_velocityY;
    float m_gravity;

    // Biến bắn đạn

    float m_shootCooldown;
    float m_currentCooldown;
    bool m_facingRight;


};