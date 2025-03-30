#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy()
    : m_x(0), m_y(0), m_speed(90.f),
      m_texture(nullptr), m_width(32), m_height(32),
      m_isJumping(false), m_velocityY(0.f), m_gravity(500.f),
      m_facingRight(true), m_hitPoints(10),
      m_enemyIndex(0), m_jumpCooldown(1.0f),
      m_currentJumpCooldown(0.0f), m_shouldJump(false)
{
}

Enemy::~Enemy() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Enemy::init(SDL_Renderer* renderer, float startX, float startY,
                 const char* texturePath, int enemyIndex) {
    m_x = startX;
    m_y = startY;
    m_texture = ResourceManager::LoadTexture(texturePath, renderer);
    m_width = 40;
    m_height = 40;
    m_enemyIndex = enemyIndex;

    // Tăng độ khó theo thứ tự enemy
    m_speed = 90.f + (enemyIndex * 10.f); // Tăng tốc độ
    m_hitPoints = 10 + enemyIndex;         // Tăng máu
}

void Enemy::calculatePathToPlayer(const Player* player, const Map& map) {
    // Tính khoảng cách đến player
    float dx = player->getX() - m_x;
    float dy = player->getY() - m_y;

    // Xác định hướng di chuyển
    m_facingRight = dx > 0;

    // Kiểm tra xem có vật cản phía trước không
    float checkX = m_facingRight ? m_x + m_width + 10 : m_x - 10;
    int tileX = static_cast<int>(checkX / map.tileWidth);
    int tileY = static_cast<int>((m_y + m_height/2) / map.tileHeight);

    // Nếu có vật cản phía trước và đang ở trên mặt đất
    if (tileY < map.height && tileX >= 0 && tileX < map.width) {
        if (map.tileData[tileY * map.width + tileX] != 0 && isOnGround(map)) {
            m_shouldJump = true;
        }
    }
}

void Enemy::update(float deltaTime, const Map& map, Player* player) {
    // Cập nhật cooldown nhảy
    if (m_currentJumpCooldown > 0) {
        m_currentJumpCooldown -= deltaTime;
    }

    calculatePathToPlayer(player, map);

    float vx = m_facingRight ? 1.0f : -1.0f;
    bool onGround = isOnGround(map);

    // Xử lý nhảy
    if (m_shouldJump && onGround && m_currentJumpCooldown <= 0) {
        m_isJumping = true;
        m_velocityY = -280.f;
        m_currentJumpCooldown = m_jumpCooldown;
        m_shouldJump = false;
    }

    // Xử lý trọng lực
    if (!onGround) {
        m_velocityY += m_gravity * deltaTime;
        m_velocityY = std::min(m_velocityY, 500.f);
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

    // Cập nhật vị trí theo trục X
    if (!collidingHorizontally) {
        m_x = newX;
    } else {
        if (vx > 0) {
            int tileX = static_cast<int>((newX + m_width) / map.tileWidth);
            m_x = (tileX * map.tileWidth) - m_width;
        } else {
            int tileX = static_cast<int>(ceil(newX / map.tileWidth));
            m_x = tileX * map.tileWidth;
        }
        m_shouldJump = true; // Gặp vật cản thì nhảy
    }

    // Cập nhật vị trí theo trục Y
    if (!collidingVertically) {
        m_y = newY;
    } else {
        if (m_velocityY > 0) {
            m_y = floor((m_y + m_height) / map.tileHeight) * map.tileHeight - m_height;
            m_velocityY = 0;
            m_isJumping = false;
        } else if (m_velocityY < 0) {
            m_velocityY = 0;
            m_isJumping = false;
        }
    }
}

void Enemy::render(SDL_Renderer* renderer, const Camera& camera) {
    if (!m_texture) return;

    SDL_Point screenPos = camera.worldToScreen(m_x, m_y);
    SDL_Rect dst;
    dst.x = screenPos.x;
    dst.y = screenPos.y;
    dst.w = m_width;
    dst.h = m_height;

    SDL_RendererFlip flip = m_facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, m_texture, NULL, &dst, 0, NULL, flip);
}

bool Enemy::isHit() {
    m_hitPoints--;
    return isDead();
}

bool Enemy::isOnGround(const Map& map) {
    float checkY = m_y + m_height + 1.0f;

    int leftTile = static_cast<int>(m_x / map.tileWidth);
    int rightTile = static_cast<int>((m_x + m_width - 1) / map.tileWidth);
    int bottomTile = static_cast<int>(checkY / map.tileHeight);

    if (bottomTile >= map.height) return true;
    if (leftTile < 0 || rightTile >= map.width) return false;

    return (map.tileData[bottomTile * map.width + leftTile] != 0 ||
            map.tileData[bottomTile * map.width + rightTile] != 0);
}

bool Enemy::isCollidingHorizontally(float newX, float checkY, const Map& map) {
    int tileLeft = static_cast<int>(newX / map.tileWidth);
    int tileRight = static_cast<int>((newX + m_width - 1) / map.tileWidth);
    int tileTop = static_cast<int>(checkY / map.tileHeight);
    int tileBottom = static_cast<int>((checkY + m_height - 1) / map.tileHeight);

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

bool Enemy::isCollidingVertically(float checkX, float newY, const Map& map) {
    int tileLeft = static_cast<int>(floor(checkX / map.tileWidth));
    int tileRight = static_cast<int>(floor((checkX + m_width - 1) / map.tileWidth));
    int tileTop = static_cast<int>(floor(newY / map.tileHeight));
    int tileBottom = static_cast<int>(floor((newY + m_height - 1) / map.tileHeight));

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