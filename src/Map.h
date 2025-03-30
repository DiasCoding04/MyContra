#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <string>
#include <vector>
#include "GameTypes.h"

struct Map {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    int tilesetWidth;
    std::vector<int> tileData;
    SDL_Texture* tilesetTexture;
};

class MapLoader {
public:
    static Map loadFromFile(const std::string& path, SDL_Renderer* renderer);
    static void render(const Map& map, SDL_Renderer* renderer);
    // Thêm overload của hàm render với camera
    static void renderWithCamera(const Map& map, SDL_Renderer* renderer, const Camera& camera);
};

std::pair<std::string, int> getTilesetImage(const std::string& tsxPath);

#endif