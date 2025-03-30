#include "Bullet.h"
#include "ResourceManager.h"

Bullet::Bullet()
    : m_x(0), m_y(0), m_speed(400.0f), // Tăng tốc độ đạn lên 400
      m_active(false), m_movingRight(true),
      m_texture(nullptr), m_width(10), m_height(3)
{
}

Bullet::~Bullet() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Bullet::init(SDL_Renderer* renderer, float startX, float startY, bool movingRight, const char* texturePath) {
    m_x = startX;
    m_y = startY;
    m_movingRight = movingRight;
    m_active = true;

    if (!m_texture) {
        m_texture = ResourceManager::LoadTexture(texturePath, renderer);
    }
}

void Bullet::update(float deltaTime) {
    if (!m_active) return;
    m_x += m_speed * deltaTime * (m_movingRight ? 1 : -1);
}

void Bullet::render(SDL_Renderer* renderer, const Camera& camera) {
    if (!m_active || !m_texture) return;

    SDL_Point screenPos = camera.worldToScreen(m_x, m_y);
    SDL_Rect dst = {
        screenPos.x,
        screenPos.y,
        m_width,
        m_height
    };

    SDL_RenderCopy(renderer, m_texture, NULL, &dst);
}