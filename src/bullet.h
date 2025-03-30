#pragma once
#include <SDL.h>
#include "Camera.h"

class Bullet {
public:
    Bullet();
    ~Bullet();

    void init(SDL_Renderer* renderer, float startX, float startY, bool movingRight, const char* texturePath);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer, const Camera& camera);
    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }

    // Thêm getters
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    float m_x, m_y;
    float m_speed;
    bool m_active;
    bool m_movingRight;
    SDL_Texture* m_texture;
    int m_width, m_height;
};