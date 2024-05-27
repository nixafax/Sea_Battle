/*#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int main(int argc, char* argv[]) {
    // Инициализация SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Инициализация SDL_image
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Создание окна
    SDL_Window* window = SDL_CreateWindow("My Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Создание рендерера
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Загрузка фонового изображения
    SDL_Texture* background = IMG_LoadTexture(renderer, "BG.png");
    if (background == nullptr) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Основной игровой цикл
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Отрисовка фона
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    // Очистка ресурсов
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Инициализация SDL и SDL_image
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    if (TTF_Init() < 0) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

// Создание окна и рендерера
SDL_Window* createWindow() {
    SDL_Window* window = SDL_CreateWindow("My Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    return window;
}

SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return nullptr;
    }
    return renderer;
}

// Загрузка фонового изображения
SDL_Texture* loadBackground(SDL_Renderer* renderer) {
    SDL_Texture* background = IMG_LoadTexture(renderer, "BG.png");
    if (background == nullptr) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return background;
}

int main(int argc, char* argv[]) {
    // Инициализация SDL и SDL_image
    if (!initSDL()) {
        return 1;
    }

    // Создание окна и рендерера
    SDL_Window* window = createWindow();
    if (window == nullptr) {
        return 1;
    }
    SDL_Renderer* renderer = createRenderer(window);
    if (renderer == nullptr) {
        return 1;
    }

    // Загрузка фонового изображения
    SDL_Texture* background = loadBackground(renderer);
    if (background == nullptr) {
        return 1;
    }

    // Инициализация SDL_ttf
    TTF_Font* font = TTF_OpenFont("Minecraft Rus NEW.otf", 48);
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyTexture(background);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int score = 100000;

    // Основной игровой цикл
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Очистка экрана
        SDL_RenderClear(renderer);

        // Отрисовка фонового изображения
        SDL_RenderCopy(renderer, background, nullptr, nullptr);

        // Отрисовка текста
        SDL_Color textColor = { 0, 0, 0, 255 };
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        SDL_Rect textRect = { WINDOW_WIDTH - 450, 198, textWidth, textHeight };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Вывод текста внизу экрана
        textSurface = TTF_RenderText_Solid(font, "This is a sample text", textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
        textRect = { (WINDOW_WIDTH - textWidth) / 2, WINDOW_HEIGHT - textHeight - 10, textWidth, textHeight };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Обновление экрана
        SDL_RenderPresent(renderer);
    }

    // Очистка ресурсов
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}