#include "CommonFunc.h"
#include "Player.h"
#include "InputManager.h"
#include "Map.h" // Thêm Map
#include "json.hpp"
#include "Camera.h"
#include "Background.h"


int main(int argc, char* argv[]) {
    // 1) Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init PNG Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    // 2) Tạo window, renderer
    const int SCREEN_WIDTH = 950;
    const int SCREEN_HEIGHT = 640;
    SDL_Window* window = SDL_CreateWindow("Smooth Player Movement",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 3) Tạo Player
    Player player;
    player.init(renderer,
                SCREEN_WIDTH / 5.f,
                SCREEN_HEIGHT / 2.f,
                "assets/myplayer.png"); // Đường dẫn ảnh

    // 4) Tải Map
    Map map;
    Background background;
    try {
        map = MapLoader::loadFromFile("baseMap.json", renderer);
    } catch (const std::exception& ex) {
        std::cerr << "Error loading map: " << ex.what() << std::endl;
        return 1;
    }

    bool running = true;
    Uint32 lastTime = SDL_GetTicks();

    Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    camera.init(map);

    if (!background.loadBackground(renderer, "assets/background2.jpg")) {
        std::cerr << "Failed to load background!" << std::endl;
        return 1;
    }



    while (running) {

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 500.0f;
        lastTime = currentTime;

        // Poll sự kiện duy nhất ở đây, bao gồm QUIT:
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                break;
            }
            // Gửi event này cho InputManager để giữ nguyên chức năng cũ
            // nhưng không poll event lần nữa bên trong InputManager
            InputManager::HandleEvent(e);
        }
        if (!running) break;

        // Nếu bạn vẫn muốn ESC để thoát, giữ nguyên
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        player.update(deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT, map, camera);

        if (InputManager::shoot) {
            player.shoot(renderer);
            InputManager::shoot = false;  // Reset sau khi đã bắn
        }

        camera.update(player.getX(), player.getY(), deltaTime);



        SDL_SetRenderDrawColor(renderer, 0x1E, 0x90, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        background.render(renderer, camera.getX() * 0.20f);

        // Render map
        MapLoader::renderWithCamera(map, renderer, camera);

        // Render player
        player.render(renderer, camera);



        SDL_RenderPresent(renderer);
    }

    // 5) Huỷ
    SDL_DestroyTexture(map.tilesetTexture); // Huỷ texture map
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    background.cleanup();
    IMG_Quit();
    SDL_Quit();

    return 0;
}