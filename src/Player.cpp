#include "Player.h"

#include <algorithm>

#include "ResourceManager.h"
#include "InputManager.h"
#include "Map.h"
#include <iostream>
#include <cmath>

Player::Player()
    : m_x(0), m_y(0), m_speed(200.f),
      m_texture(nullptr), m_width(32), m_height(32),
      m_isJumping(false), m_velocityY(0.f), m_gravity(500.f),
      m_touchingLeft(false), m_touchingRight(false),
      m_jumpKeyReleased(true),
      m_facingRight(true),
      m_shootCooldown(0.5f),
      m_currentCooldown(0.0f)
{
    // Xóa m_bullets.reserve(MAX_BULLETS) nếu có
}


Player::~Player()
{
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Player::init(SDL_Renderer* renderer, float startX, float startY,
                  const char* texturePath)
{
    m_x = startX;
    m_y = startY;
    m_texture = ResourceManager::LoadTexture(texturePath, renderer);
    m_width = 40;
    m_height = 40;
    m_speed = 110.f;

    m_isJumping = false;
    m_velocityY = 0.f;
    m_gravity = 500.f;

    m_touchingLeft = false;
    m_touchingRight = false;
    m_jumpKeyReleased = true;
    m_facingRight = true;


}

void Player::shoot(SDL_Renderer* renderer)
{
    if (m_currentCooldown > 0) return;

    // Kiểm tra giới hạn đạn
    if (m_bullets.size() >= MAX_BULLETS) {
        return; // Không bắn thêm nếu đã đạt giới hạn
    }

    // Tạo đạn mới
    m_bullets.emplace_back();
    float bulletStartX = m_facingRight ? m_x + m_width : m_x;
    float bulletStartY = m_y + m_height / 2;
    m_bullets.back().init(renderer, bulletStartX, bulletStartY, m_facingRight, "assets/bullet.png");
    m_currentCooldown = m_shootCooldown;
}

void Player::updateBullets(float deltaTime)
{
    // Xóa những viên đạn đã bay ra khỏi màn hình
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const Bullet& bullet) { return !bullet.isActive(); }),
        m_bullets.end()
    );

    // Cập nhật vị trí các viên đạn
    for (auto& bullet : m_bullets) {
        bullet.update(deltaTime);
    }
}

bool Player::isOnGround(const Map& map) {
    float checkY = m_y + m_height + 1.0f;

    int leftTile = (int)(m_x / map.tileWidth);
    int rightTile = (int)((m_x + m_width - 1) / map.tileWidth);
    int bottomTile = (int)(checkY / map.tileHeight);

    if (bottomTile >= map.height) return true;
    if (leftTile < 0 || rightTile >= map.width) return false;

    return (map.tileData[bottomTile * map.width + leftTile] != 0 ||
            map.tileData[bottomTile * map.width + rightTile] != 0);
}

bool Player::isCollidingHorizontally(float newX, float checkY, const Map& map) {
    int tileLeft = (int)(newX / map.tileWidth);
    int tileRight = (int)((newX + m_width - 1) / map.tileWidth);
    int tileTop = (int)(checkY / map.tileHeight);
    int tileBottom = (int)((checkY + m_height - 1) / map.tileHeight);

    if (tileLeft < 0 || tileRight >= map.width ||
        tileTop < 0 || tileBottom >= map.height) {
        return true;
    }

    for (int y = tileTop; y <= tileBottom; y++) {
        if (map.tileData[y * map.width + tileLeft] != 0 ||
            map.tileData[y * map.width + tileRight] != 0) {
            return true;
        }
    }
    return false;
}

bool Player::isCollidingVertically(float checkX, float newY, const Map& map) {
    int tileLeft = (int)floor((checkX / map.tileWidth));
    int tileRight = (int)floor(((checkX + m_width - 1) / map.tileWidth));
    int tileTop = (int)floor((newY / map.tileHeight));
    int tileBottom = (int)floor(((newY + m_height - 1) / map.tileHeight));

    if (tileLeft < 0 || tileRight >= map.width ||
        tileTop < 0 || tileBottom >= map.height) {
        return true;
    }

    for (int x = tileLeft; x <= tileRight; x++) {
        if (map.tileData[tileTop * map.width + x] != 0 ||
            map.tileData[tileBottom * map.width + x] != 0) {
            return true;
        }
    }
    return false;
}
void Player::update(float deltaTime, int screenWidth, int screenHeight, const Map& map) {
    float vx = 0.f;

    static bool wasOnGround = false;
    bool onGround = isOnGround(map);

    if (onGround != wasOnGround) {
        static float groundStateTimer = 0.0f;
        groundStateTimer += deltaTime;

        if (groundStateTimer < 0.05f) {
            onGround = wasOnGround;
        } else {
            groundStateTimer = 0.0f;
            wasOnGround = onGround;
        }
    }

    // Xử lý input ngang và cập nhật hướng nhìn
    if (InputManager::moveLeft && !m_touchingLeft) {
        vx = -1.f;
        m_facingRight = false;
    }
    if (InputManager::moveRight && !m_touchingRight) {
        vx = 1.f;
        m_facingRight = true;
    }

    // Xử lý nhảy
    if (InputManager::moveUp) {
        if (m_jumpKeyReleased && onGround && !m_isJumping) {
            m_isJumping = true;
            m_velocityY = -280.f;
            m_jumpKeyReleased = false;
        }
    } else {
        m_jumpKeyReleased = true;
    }

    // Cập nhật cooldown
    if (m_currentCooldown > 0) {
        m_currentCooldown -= deltaTime;
    }

    const float MIN_FALL_SPEED = 20.0f;
    const float MAX_FALL_SPEED = 500.0f;

    // Xử lý trọng lực và rơi
    if (!onGround) {
        m_velocityY += m_gravity * deltaTime;
        m_velocityY = std::min(m_velocityY, MAX_FALL_SPEED);

        if (m_velocityY > 0 && m_velocityY < MIN_FALL_SPEED) {
            m_velocityY = MIN_FALL_SPEED;
        }
    } else if (m_velocityY > 0) {
        m_velocityY = 0.f;
        m_isJumping = false;
    }

    // Tính toán vị trí mới
    float newX = m_x + vx * m_speed * deltaTime;
    float newY = m_y + m_velocityY * deltaTime;

    // Xử lý va chạm
    bool collidingHorizontally = isCollidingHorizontally(newX, m_y, map);
    bool collidingVertically = isCollidingVertically(m_x, newY, map);

    // Xử lý di chuyển ngang
    if (vx != 0) {
        if (!collidingHorizontally) {
            m_x = newX;
        } else {
            if (vx > 0) {
                int tileX = (int)floor((newX + m_width) / map.tileWidth);
                m_x = (tileX * map.tileWidth) - m_width;
            } else {
                int tileX = (int)ceil(newX / map.tileWidth);
                m_x = tileX * map.tileWidth;
            }
        }
    }

    // Xử lý di chuyển dọc
    if (!collidingVertically) {
        m_y = newY;
    } else {
        if (m_velocityY > 0) {
            m_y = floor((m_y + m_height) / map.tileHeight) * map.tileHeight - m_height;
            m_velocityY = 0;
            m_isJumping = false;
        } else if (m_velocityY < 0) {
            m_velocityY = std::abs(m_velocityY) * 0.3f;
            m_isJumping = false;
        }
    }

    // Cập nhật đạn
    updateBullets(deltaTime);
}

void Player::render(SDL_Renderer* renderer, const Camera& camera)
{
    if (!m_texture) return;

    SDL_Point screenPos = camera.worldToScreen(m_x, m_y);
    SDL_Rect dst;
    dst.x = screenPos.x;
    dst.y = screenPos.y;
    dst.w = m_width;
    dst.h = m_height;

    // Sử dụng SDL_RenderCopyEx để có thể lật texture
    SDL_RendererFlip flip = m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, m_texture, NULL, &dst, 0, NULL, flip);

    // Render đạn
    renderBullets(renderer, camera);
}

void Player::renderBullets(SDL_Renderer* renderer, const Camera& camera)
{
    for (auto& bullet : m_bullets) {
        bullet.render(renderer, camera);
    }
}