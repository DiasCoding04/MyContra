cmake_minimum_required(VERSION 3.20)
project(GameSdl2)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)

# Include các thư mục header
include_directories(
        ${CMAKE_SOURCE_DIR}/libs/SDL2/include
        ${CMAKE_SOURCE_DIR}/libs/SDL2_image/include
        ${CMAKE_SOURCE_DIR}/libs/SDL2_mixer/include
        ${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/include
        ${CMAKE_SOURCE_DIR}/libs/Glew/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/libs
        ${CMAKE_SOURCE_DIR}/libs/tinyxml2 # Thêm TinyXML2
)

# Link các thư viện
link_directories(
        ${CMAKE_SOURCE_DIR}/libs/SDL2/lib
        ${CMAKE_SOURCE_DIR}/libs/SDL2_image/lib
        ${CMAKE_SOURCE_DIR}/libs/SDL2_mixer/lib
        ${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/lib
        ${CMAKE_SOURCE_DIR}/libs/Glew/lib
)

# Tạo file thực thi
add_executable(GameSdl2 src/main.cpp
        src/CommonFunc.h
        src/Player.cpp
        src/Player.h
        src/Camera.cpp
        src/Camera.h
        src/InputManager.cpp
        src/InputManager.h
        src/SoundManager.cpp
        src/SoundManager.h
        src/ResourceManager.cpp
        src/ResourceManager.h
        src/Utils.h
        src/TextureManager.h
        src/TextureManager.cpp
        src/Map.h
        src/Map.cpp
        libs/tinyxml2/tinyxml2.cpp
        src/GameTypes.h
        src/Background.h
        src/Background.cpp
        src/bullet.h
        src/bullet.cpp # Thêm TinyXML2
)

# Liên kết thư viện
target_link_libraries(GameSdl2
        SDL2
        SDL2main
        SDL2_image
        SDL2_mixer
        SDL2_ttf

)
