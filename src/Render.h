#ifndef RENDER_H
#define RENDER_H

#include <glew.h>
#include <string>
#include <glm.hpp>

class Render {
public:
    void Init();
    GLuint LoadTexture(const std::string& path);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size, GLuint textureID);

    ~Render(); // Đảm bảo rằng destructor được khai báo

private:
    GLuint VAO, VBO, EBO;
    GLuint shaderProgram; // Thêm biến shaderProgram vào class để tránh lỗi undefined
};

#endif
