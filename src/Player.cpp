#include "Player.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "Map.h"
#include <iostream>
#include <cmath>

Player::Player()
    : m_x(0), m_y(0), m_speed(200.f),
      m_texture(nullptr), m_width(32), m_height(32),
      m_isJumping(false), m_velocityY(0.f), m_gravity(500.f) {}

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
    m_jumpKeyReleased = true; // Ban đầu coi như phím đã được thả

}

bool Player::isOnGround(const Map& map) {
    // Chỉ kiểm tra một khoảng nhỏ bên dưới player
    float checkY = m_y + m_height + 1.0f; // Chỉ kiểm tra 1 pixel bên dưới

    // Kiểm tra 2 điểm: góc trái và phải của player
    int leftTile = (int)(m_x / map.tileWidth);
    int rightTile = (int)((m_x + m_width - 1) / map.tileWidth);
    int bottomTile = (int)(checkY / map.tileHeight);

    // Kiểm tra giới hạn bản đồ
    if (bottomTile >= map.height) return true;
    if (leftTile < 0 || rightTile >= map.width) return false;

    // Kiểm tra va chạm với tile
    return (map.tileData[bottomTile * map.width + leftTile] != 0 ||
            map.tileData[bottomTile * map.width + rightTile] != 0);
}



bool Player::isCollidingHorizontally(float newX, float checkY, const Map& map) {
    // Kiểm tra va chạm theo chiều ngang
    int tileLeft = (int)(newX / map.tileWidth);
    int tileRight = (int)((newX + m_width - 1) / map.tileWidth);
    int tileTop = (int)(checkY / map.tileHeight);
    int tileBottom = (int)((checkY + m_height - 1) / map.tileHeight);

    // Kiểm tra giới hạn bản đồ
    if (tileLeft < 0 || tileRight >= map.width ||
        tileTop < 0 || tileBottom >= map.height) {
        return true;
        }

    // Kiểm tra va chạm với các tile
    for (int y = tileTop; y <= tileBottom; y++) {
        if (map.tileData[y * map.width + tileLeft] != 0 ||
            map.tileData[y * map.width + tileRight] != 0) {
            return true;
            }
    }
    return false;
}



bool Player::isCollidingVertically(float checkX, float newY, const Map& map) {
    // Kiểm tra va chạm theo chiều dọc
    int tileLeft = (int)floor((checkX / map.tileWidth));
    int tileRight = (int)floor(((checkX + m_width - 1) / map.tileWidth));
    int tileTop = (int)floor((newY / map.tileHeight));
    int tileBottom = (int)floor(((newY + m_height - 1) / map.tileHeight));

    // Kiểm tra giới hạn bản đồ
    if (tileLeft < 0 || tileRight >= map.width ||
        tileTop < 0 || tileBottom >= map.height) {
        return true;
        }

    // Kiểm tra va chạm với các tile
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

    // THÊM: Caching trạng thái onGround
    static bool wasOnGround = false;
    bool onGround = isOnGround(map);

    // THÊM: Xử lý caching để tránh việc thay đổi trạng thái quá nhanh
    if (onGround != wasOnGround) {
        // Chỉ cập nhật trạng thái nếu đã ở trạng thái mới đủ lâu
        static float groundStateTimer = 0.0f;
        groundStateTimer += deltaTime;

        if (groundStateTimer < 0.05f) { // 50ms threshold
            onGround = wasOnGround;
        } else {
            groundStateTimer = 0.0f;
            wasOnGround = onGround;
        }
    }

    // Xử lý input ngang
    if (InputManager::moveLeft && !m_touchingLeft) {
        vx = -1.f;
    }
    if (InputManager::moveRight && !m_touchingRight) {
        vx = 1.f;
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

    // THAY ĐỔI: Cải thiện xử lý trọng lực
    const float MIN_FALL_SPEED = 20.0f;  // Tốc độ rơi tối thiểu
    const float MAX_FALL_SPEED = 500.0f; // Tốc độ rơi tối đa

    if (!onGround) {
        // Áp dụng trọng lực với giới hạn tốc độ rơi
        m_velocityY += m_gravity * deltaTime;
        m_velocityY = std::min(m_velocityY, MAX_FALL_SPEED);

        // THÊM: Đảm bảo luôn có một tốc độ rơi tối thiểu khi không đứng trên mặt đất
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

    // THAY ĐỔI: Thêm xử lý "coyote time" - cho phép nhảy một chút sau khi rời khỏi mặt đất
    static float coyoteTimer = 0.0f;
    const float COYOTE_TIME = 0.1f; // 100ms

    if (wasOnGround && !onGround) {
        coyoteTimer = COYOTE_TIME;
    } else if (!onGround) {
        coyoteTimer = std::max(0.0f, coyoteTimer - deltaTime);
    }

    // Xử lý va chạm
    bool collidingHorizontally = isCollidingHorizontally(newX, m_y, map);
    bool collidingVertically = isCollidingVertically(m_x, newY, map);

    // THÊM: Xử lý va chạm mượt mà hơn
    if (!collidingHorizontally) {
        m_x = newX;
    } else {
        // Điều chỉnh vị trí để áp sát vật thể
        if (vx > 0) {
            m_x = floor((m_x + m_width) / map.tileWidth) * map.tileWidth - m_width;
        } else if (vx < 0) {
            m_x = ceil(m_x / map.tileWidth) * map.tileWidth;
        }
    }

    if (vx != 0) { // Chỉ xử lý va chạm ngang khi đang di chuyển
        bool collidingHorizontally = isCollidingHorizontally(newX, m_y, map);

        if (!collidingHorizontally) {
            m_x = newX;
        } else {
            // Điều chỉnh vị trí một cách chính xác hơn
            if (vx > 0) {
                // Di chuyển sang phải
                int tileX = (int)floor((newX + m_width) / map.tileWidth);
                m_x = (tileX * map.tileWidth) - m_width;
            } else {
                // Di chuyển sang trái
                int tileX = (int)ceil(newX / map.tileWidth);
                m_x = tileX * map.tileWidth;
            }
        }
    }

    if (!collidingVertically) {
        m_y = newY;
    } else {
        if (m_velocityY > 0) {  // Khi rơi xuống
            m_y = floor((m_y + m_height) / map.tileHeight) * map.tileHeight - m_height;
            m_velocityY = 0;
            m_isJumping = false;
        } else if (m_velocityY < 0) {  // Khi nhảy lên
            // Không đặt lại vị trí Y, chỉ đảo ngược vận tốc
            m_velocityY = std::abs(m_velocityY) * 0.3f; // Chuyển vận tốc âm thành dương với 30% lực ban đầu
            m_isJumping = false;
        }
    }

    
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

    SDL_RenderCopy(renderer, m_texture, NULL, &dst);
}
