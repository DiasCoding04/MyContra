#include "CommonFunc.h"
#include "Player.h"
#include "InputManager.h"
#include "Map.h"
#include "json.hpp"
#include "Camera.h"
#include "Background.h"
#include <ctime>
#include <vector>
#include <algorithm>
#include <fstream>

// Constants
const float LAVA_RISE_SPEED = 25.0f;
const int MAX_HIGH_SCORES = 5;

struct Score {
    int value;
    std::string date;
    Score(int v = 0, const std::string& d = "") : value(v), date(d) {}
    bool operator<(const Score& other) const {
        return value > other.value;
    }
};

enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

// Function to load high scores from file
std::vector<Score> loadHighScores() {
    std::vector<Score> scores;
    std::ifstream file("highscores.txt");
    if (file.is_open()) {
        int score;
        std::string date;
        while (file >> score) {
            file.ignore();
            std::getline(file, date);
            scores.emplace_back(score, date);
        }
        file.close();
    }
    std::sort(scores.begin(), scores.end());
    if (scores.size() > MAX_HIGH_SCORES) {
        scores.resize(MAX_HIGH_SCORES);
    }
    return scores;
}

// Function to save high scores to file
void saveHighScore(int score) {
    auto scores = loadHighScores();

    time_t now = time(nullptr);
    struct tm *ltm = localtime(&now);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M", ltm);

    scores.emplace_back(score, date);
    std::sort(scores.begin(), scores.end());
    if (scores.size() > MAX_HIGH_SCORES) {
        scores.resize(MAX_HIGH_SCORES);
    }

    std::ofstream file("highscores.txt");
    if (file.is_open()) {
        for (const auto& s : scores) {
            file << s.value << " " << s.date << "\n";
        }
        file.close();
    }
}

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
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    // 2) Tạo window, renderer
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

    // 3) Load font
    TTF_Font* font = TTF_OpenFont("assets/Arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return 1;
    }

    // 4) Tải Map
    Map map;
    Background background;
    try {
        map = MapLoader::loadFromFile("baseMap.json", renderer);
    } catch (const std::exception& ex) {
        std::cerr << "Error loading map: " << ex.what() << std::endl;
        return 1;
    }

    // 5) Game variables
    GameState gameState = GameState::MENU;
    bool running = true;
    bool gameOver = false;
    int currentScore = 0;
    float highestHeightReached = 0;
    float initialPlayerY = 0;
    Uint32 lastTime = SDL_GetTicks();

    // 6) Game components
    Player player;
    Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
    float lavaY = map.height * map.tileHeight;

    // Khởi tạo player
    player.init(renderer,
                SCREEN_WIDTH / 5.f,
                (map.height * map.tileHeight) - 150.0f,
                "assets/myplayer.png");
    initialPlayerY = player.getY();

    // Khởi tạo camera và background
    camera.init(map);
    if (!background.loadBackground(renderer, "assets/background.png")) {
        std::cerr << "Failed to load background!" << std::endl;
        return 1;
    }

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 500.0f;
        lastTime = currentTime;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (gameState == GameState::MENU) {
                    SDL_Rect playButton = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 25, 200, 50};
                    if (mouseX >= playButton.x && mouseX <= playButton.x + playButton.w &&
                        mouseY >= playButton.y && mouseY <= playButton.y + playButton.h) {
                        gameState = GameState::PLAYING;
                        gameOver = false;
                        lavaY = map.height * map.tileHeight;
                        player.init(renderer, SCREEN_WIDTH / 5.f,
                                  (map.height * map.tileHeight) - 150.0f,
                                  "assets/myplayer.png");
                        initialPlayerY = player.getY();
                        currentScore = 0;
                        highestHeightReached = 0;
                    }
                }
                else if (gameState == GameState::GAME_OVER) {
                    SDL_Rect restartButton = {SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT - 100, 200, 50};
                    SDL_Rect menuButton = {SCREEN_WIDTH/2 + 20, SCREEN_HEIGHT - 100, 200, 50};

                    if (mouseX >= restartButton.x && mouseX <= restartButton.x + restartButton.w &&
                        mouseY >= restartButton.y && mouseY <= restartButton.y + restartButton.h) {
                        gameState = GameState::PLAYING;
                        gameOver = false;
                        lavaY = map.height * map.tileHeight;
                        player.init(renderer, SCREEN_WIDTH / 5.f,
                                  (map.height * map.tileHeight) - 150.0f,
                                  "assets/myplayer.png");
                        initialPlayerY = player.getY();
                        currentScore = 0;
                        highestHeightReached = 0;
                    }
                    else if (mouseX >= menuButton.x && mouseX <= menuButton.x + menuButton.w &&
                             mouseY >= menuButton.y && mouseY <= menuButton.y + menuButton.h) {
                        gameState = GameState::MENU;
                    }
                }
            }

            InputManager::HandleEvent(e);
        }

        if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0x1E, 0x90, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        if (gameState == GameState::MENU) {
            SDL_Color textColor = {255, 255, 255, 255};

            // Title
            SDL_Surface* surfaceTitle = TTF_RenderText_Solid(font, "Lava Escape", textColor);
            SDL_Texture* textureTitle = SDL_CreateTextureFromSurface(renderer, surfaceTitle);
            SDL_Rect titleRect = {SCREEN_WIDTH/2 - surfaceTitle->w/2, SCREEN_HEIGHT/3, surfaceTitle->w, surfaceTitle->h};
            SDL_RenderCopy(renderer, textureTitle, NULL, &titleRect);

            // Play button
            SDL_Rect playButton = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 25, 200, 50};
            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
            SDL_RenderFillRect(renderer, &playButton);

            SDL_Surface* surfacePlay = TTF_RenderText_Solid(font, "Play Game", textColor);
            SDL_Texture* texturePlay = SDL_CreateTextureFromSurface(renderer, surfacePlay);
            SDL_Rect playTextRect = {SCREEN_WIDTH/2 - surfacePlay->w/2, SCREEN_HEIGHT/2 - surfacePlay->h/2, surfacePlay->w, surfacePlay->h};
            SDL_RenderCopy(renderer, texturePlay, NULL, &playTextRect);

            // High Scores
            auto highScores = loadHighScores();
            if (!highScores.empty()) {
                SDL_Surface* surfaceScoreTitle = TTF_RenderText_Solid(font, "High Scores:", textColor);
                SDL_Texture* textureScoreTitle = SDL_CreateTextureFromSurface(renderer, surfaceScoreTitle);
                SDL_Rect scoreTitleRect = {SCREEN_WIDTH/2 - surfaceScoreTitle->w/2, SCREEN_HEIGHT/2 + 50, surfaceScoreTitle->w, surfaceScoreTitle->h};
                SDL_RenderCopy(renderer, textureScoreTitle, NULL, &scoreTitleRect);

                int yPos = SCREEN_HEIGHT/2 + 100;
                for (const auto& score : highScores) {
                    std::string scoreText = std::to_string(score.value) + " (" + score.date + ")";
                    SDL_Surface* surfaceScore = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                    SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);
                    SDL_Rect scoreRect = {SCREEN_WIDTH/2 - surfaceScore->w/2, yPos, surfaceScore->w, surfaceScore->h};
                    SDL_RenderCopy(renderer, textureScore, NULL, &scoreRect);
                    SDL_FreeSurface(surfaceScore);
                    SDL_DestroyTexture(textureScore);
                    yPos += 30;
                }

                SDL_FreeSurface(surfaceScoreTitle);
                SDL_DestroyTexture(textureScoreTitle);
            }

            SDL_FreeSurface(surfaceTitle);
            SDL_FreeSurface(surfacePlay);
            SDL_DestroyTexture(textureTitle);
            SDL_DestroyTexture(texturePlay);
        }
        else if (gameState == GameState::PLAYING) {
            if (!gameOver) {
                player.update(deltaTime, SCREEN_WIDTH, SCREEN_HEIGHT, map, camera);
                lavaY -= LAVA_RISE_SPEED * deltaTime;

                // Update highest height reached
                float currentHeight = initialPlayerY - player.getY();
                if (currentHeight > highestHeightReached) {
                    highestHeightReached = currentHeight;
                    currentScore = static_cast<int>(highestHeightReached / map.tileHeight);
                }

                if (player.getY() > lavaY) {
                    gameOver = true;
                    gameState = GameState::GAME_OVER;
                    saveHighScore(currentScore);
                }



                camera.update(player.getX(), player.getY(), deltaTime);
            }

            // Render game
            background.render(renderer, camera.getX() * 0.20f);
            MapLoader::renderWithCamera(map, renderer, camera);
            player.render(renderer, camera);

            // Render lava
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x45, 0x00, 0xFF);
            SDL_Point lavaScreenPos = camera.worldToScreen(0, lavaY);
            SDL_Rect lavaRect = {0, lavaScreenPos.y, SCREEN_WIDTH, SCREEN_HEIGHT * 2};
            SDL_RenderFillRect(renderer, &lavaRect);

            // Render current score
            SDL_Color textColor = {255, 255, 255, 255};
            std::string scoreText = "Score: " + std::to_string(currentScore);
            SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {SCREEN_WIDTH/2 - surface->w/2, 10, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        else if (gameState == GameState::GAME_OVER) {
            SDL_Color textColor = {255, 255, 255, 255};

            // Game Over text
            SDL_Surface* surfaceGameOver = TTF_RenderText_Solid(font, "Game Over!", textColor);
            SDL_Texture* textureGameOver = SDL_CreateTextureFromSurface(renderer, surfaceGameOver);
            SDL_Rect gameOverRect = {SCREEN_WIDTH/2 - surfaceGameOver->w/2, 100, surfaceGameOver->w, surfaceGameOver->h};
            SDL_RenderCopy(renderer, textureGameOver, NULL, &gameOverRect);

            // Final score
            std::string finalScoreText = "Your Score: " + std::to_string(currentScore);
            SDL_Surface* surfaceScore = TTF_RenderText_Solid(font, finalScoreText.c_str(), textColor);
            SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);
            SDL_Rect scoreRect = {SCREEN_WIDTH/2 - surfaceScore->w/2, 150, surfaceScore->w, surfaceScore->h};
            SDL_RenderCopy(renderer, textureScore, NULL, &scoreRect);

            // High Scores
            auto highScores = loadHighScores();
            SDL_Surface* surfaceHighScoreTitle = TTF_RenderText_Solid(font, "High Scores:", textColor);
            SDL_Texture* textureHighScoreTitle = SDL_CreateTextureFromSurface(renderer, surfaceHighScoreTitle);
            SDL_Rect highScoreTitleRect = {SCREEN_WIDTH/2 - surfaceHighScoreTitle->w/2, 200, surfaceHighScoreTitle->w, surfaceHighScoreTitle->h};
            SDL_RenderCopy(renderer, textureHighScoreTitle, NULL, &highScoreTitleRect);

            int yPos = 250;
            for (const auto& score : highScores) {
                std::string scoreText = std::to_string(score.value) + " (" + score.date + ")";
                SDL_Surface* surfaceHighScore = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                SDL_Texture* textureHighScore = SDL_CreateTextureFromSurface(renderer, surfaceHighScore);
                SDL_Rect highScoreRect = {SCREEN_WIDTH/2 - surfaceHighScore->w/2, yPos, surfaceHighScore->w, surfaceHighScore->h};
                SDL_RenderCopy(renderer, textureHighScore, NULL, &highScoreRect);
                SDL_FreeSurface(surfaceHighScore);
                SDL_DestroyTexture(textureHighScore);
                yPos += 40;
            }

            // Buttons
            SDL_Rect restartButton = {SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT - 100, 200, 50};
            SDL_Rect menuButton = {SCREEN_WIDTH/2 + 20, SCREEN_HEIGHT - 100, 200, 50};

            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
            SDL_RenderFillRect(renderer, &restartButton);
            SDL_RenderFillRect(renderer, &menuButton);

            SDL_Surface* surfaceRestart = TTF_RenderText_Solid(font, "Play Again", textColor);
            SDL_Surface* surfaceMenu = TTF_RenderText_Solid(font, "Main Menu", textColor);

            SDL_Texture* textureRestart = SDL_CreateTextureFromSurface(renderer, surfaceRestart);
            SDL_Texture* textureMenu = SDL_CreateTextureFromSurface(renderer, surfaceMenu);

            SDL_Rect restartTextRect = {restartButton.x + (restartButton.w - surfaceRestart->w)/2,
                                      restartButton.y + (restartButton.h - surfaceRestart->h)/2,
                                      surfaceRestart->w, surfaceRestart->h};
            SDL_Rect menuTextRect = {menuButton.x + (menuButton.w - surfaceMenu->w)/2,
                                   menuButton.y + (menuButton.h - surfaceMenu->h)/2,
                                   surfaceMenu->w, surfaceMenu->h};

            SDL_RenderCopy(renderer, textureRestart, NULL, &restartTextRect);
            SDL_RenderCopy(renderer, textureMenu, NULL, &menuTextRect);

            // Cleanup
            SDL_FreeSurface(surfaceGameOver);
            SDL_FreeSurface(surfaceScore);
            SDL_FreeSurface(surfaceHighScoreTitle);
            SDL_FreeSurface(surfaceRestart);
            SDL_FreeSurface(surfaceMenu);
            SDL_DestroyTexture(textureGameOver);
            SDL_DestroyTexture(textureScore);
            SDL_DestroyTexture(textureHighScoreTitle);
            SDL_DestroyTexture(textureRestart);
            SDL_DestroyTexture(textureMenu);
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyTexture(map.tilesetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    background.cleanup();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}