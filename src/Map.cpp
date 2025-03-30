#include "Map.h"
#include "Camera.h"
#include "json.hpp"
#include "TextureManager.h"
#include <fstream>
#include <stdexcept>
#include "tinyxml2.h"
#include <iostream>

using json = nlohmann::json;

// Hàm lấy thông tin tileset từ file .tsx
std::pair<std::string, int> getTilesetImage(const std::string& tsxPath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(tsxPath.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to load tileset file: " + tsxPath);
    }

    auto root = doc.FirstChildElement("tileset");
    if (!root) {
        throw std::runtime_error("Invalid tileset file: Missing 'tileset' element.");
    }

    auto imageElement = root->FirstChildElement("image");
    if (!imageElement) {
        throw std::runtime_error("Invalid tileset file: Missing 'image' element.");
    }

    const char* imageSource = imageElement->Attribute("source");
    if (!imageSource) {
        throw std::runtime_error("Invalid tileset file: Missing 'source' attribute.");
    }

    int imageWidth = 0;
    if (imageElement->QueryIntAttribute("width", &imageWidth) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to retrieve 'width' attribute from image element.");
    }

    return {std::string(imageSource), imageWidth};
}

Map MapLoader::loadFromFile(const std::string& path, SDL_Renderer* renderer) {
    // Mở file JSON
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open map file: " + path);
    }

    // Parse JSON
    json mapData;
    file >> mapData;

    // Tạo map
    Map map;
    map.width = mapData["width"];
    map.height = mapData["height"];
    map.tileWidth = mapData["tilewidth"];
    map.tileHeight = mapData["tileheight"];
    map.tileData = mapData["layers"][0]["data"].get<std::vector<int>>(); // Dữ liệu layer Ground

    // Lấy đường dẫn tileset từ JSON
    std::string tsxFile = mapData["tilesets"][0]["source"].get<std::string>();
    std::string tsxPath = "" + tsxFile; // Đường dẫn đầy đủ

    // Lấy thông tin từ .tsx
    std::pair<std::string, int> tilesetInfo = getTilesetImage(tsxPath);
    std::string tilesetImage = tilesetInfo.first;
    int tilesetWidth = tilesetInfo.second;

    // Ghép đường dẫn đầy đủ cho tileset image
    std::string tilesetPath = "" + tilesetImage;

    // Load texture
    map.tilesetTexture = TextureManager::LoadTexture(tilesetPath, renderer);
    map.tilesetWidth = tilesetWidth; // Gán chiều rộng tileset vào map

    return map;
}

void MapLoader::render(const Map& map, SDL_Renderer* renderer) {
    int tileCountX = map.tilesetWidth / map.tileWidth; // Số cột trong tileset
    int spacing = 0; // Khoảng cách giữa các tile, nếu có
    int margin = 0; // Khoảng cách viền, nếu có

    SDL_Rect srcRect, dstRect;
    srcRect.w = map.tileWidth;
    srcRect.h = map.tileHeight;
    dstRect.w = map.tileWidth;
    dstRect.h = map.tileHeight;

    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            int tileID = map.tileData[y * map.width + x] - 1; // Tile ID (trừ 1 vì JSON bắt đầu từ 1)

            if (tileID >= 0) {
                srcRect.x = margin + (tileID % tileCountX) * (map.tileWidth + spacing);
                srcRect.y = margin + (tileID / tileCountX) * (map.tileHeight + spacing);
                dstRect.x = x * map.tileWidth;
                dstRect.y = y * map.tileHeight;

                SDL_RenderCopy(renderer, map.tilesetTexture, &srcRect, &dstRect);
            }
        }
    }
}

void MapLoader::renderWithCamera(const Map& map, SDL_Renderer* renderer, const Camera& camera) {
    int tileCountX = map.tilesetWidth / map.tileWidth;
    int spacing = 0;
    int margin = 0;

    // Tính các tile nằm trong viewport
    int startX = static_cast<int>(camera.getX()) / map.tileWidth;
    int startY = static_cast<int>(camera.getY()) / map.tileHeight;
    int endX = (static_cast<int>(camera.getX()) + 950) / map.tileWidth + 1;
    int endY = (static_cast<int>(camera.getY()) + 640) / map.tileHeight + 1;

    // Giới hạn trong phạm vi map
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(map.width, endX);
    endY = std::min(map.height, endY);

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            int tileID = map.tileData[y * map.width + x] - 1;

            if (tileID >= 0) {
                SDL_Rect srcRect;
                srcRect.x = margin + (tileID % tileCountX) * (map.tileWidth + spacing);
                srcRect.y = margin + (tileID / tileCountX) * (map.tileHeight + spacing);
                srcRect.w = map.tileWidth;
                srcRect.h = map.tileHeight;

                SDL_Rect dstRect = camera.getRenderRect(x, y, map);
                SDL_RenderCopy(renderer, map.tilesetTexture, &srcRect, &dstRect);
            }
        }
    }
}