#pragma once
#include <SDL.h>
#include <vector>
#include "Camera.h"
#include "Map.h"
#include "Player.h"

class Enemy {
public:
    Enemy();
    ~Enemy();

    void init(SDL_Renderer* renderer, float startX, float startY,
              const char* texturePath, int enemyIndex);
    void update(float deltaTime, const Map& map, Player* player);
    void render(SDL_Renderer* renderer, const Camera& camera);
    bool isHit(); // Kiểm tra khi trúng đạn
    bool isDead() const { return m_hitPoints <= 0; }

private:
    // Di chuyển và va chạm
    bool isOnGround(const Map& map);
    bool isCollidingHorizontally(float newX, float checkY, const Map& map);
    bool isCollidingVertically(float checkX, float newY, const Map& map);
    void calculatePathToPlayer(const Player* player, const Map& map);

    // Thuộc tính cơ bản
    float m_x, m_y;
    float m_width, m_height;
    float m_speed;
    SDL_Texture* m_texture;

    // Thuộc tính chuyển động
    bool m_isJumping;
    float m_velocityY;
    float m_gravity;
    bool m_facingRight;

    // Chỉ số
    int m_hitPoints;
    int m_enemyIndex; // Để tăng độ khó theo thứ tự sinh ra

    // Pathfinding
    float m_jumpCooldown;
    float m_currentJumpCooldown;
    bool m_shouldJump;
};