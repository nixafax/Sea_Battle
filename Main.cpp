#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Вывод двумерного массива
void printGrid(const std::vector<std::vector<int>>& grid) {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            std::cout << cell << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

// Проверка на потапление
// Проверяет, есть ли значения от 1 до 10 вокруг группы ячеек с -2
bool isShipAdjacent(const std::vector<std::vector<int>>& grid, const std::vector<std::pair<int, int>>& shipCells) {
    std::vector<std::pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
    for (const auto& cell : shipCells) {
        int x = cell.first;
        int y = cell.second;
        for (const auto& dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;
            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
                if (grid[nx][ny] > 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Заполняет пустые ячейки вокруг группы ячеек с -2 значением -1
void fillSurroundings(std::vector<std::vector<int>>& grid, const std::vector<std::pair<int, int>>& shipCells) {
    std::vector<std::pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
    for (const auto& cell : shipCells) {
        int x = cell.first;
        int y = cell.second;
        for (const auto& dir : directions) {
            int nx = x + dir.first;
            int ny = y + dir.second;
            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10) {
                if (grid[nx][ny] == 0) {
                    grid[nx][ny] = -1;
                }
            }
        }
    }
}

// Функция для поиска всех групп ячеек с -2 и проверки их окружения
void surroundSunkShips(std::vector<std::vector<int>>& grid) {
    int rows = grid.size();
    int cols = grid[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::vector<std::pair<int, int>> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (int x = 0; x < rows; ++x) {
        for (int y = 0; y < cols; ++y) {
            if (grid[x][y] == -2 && !visited[x][y]) {
                std::vector<std::pair<int, int>> shipCells;
                std::queue<std::pair<int, int>> q;
                q.push({ x, y });
                visited[x][y] = true;

                while (!q.empty()) {
                    auto cell = q.front();
                    int cx = cell.first;
                    int cy = cell.second;
                    q.pop();
                    shipCells.push_back({ cx, cy });

                    for (const auto& dir : directions) {
                        int nx = cx + dir.first;
                        int ny = cy + dir.second;
                        if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] == -2 && !visited[nx][ny]) {
                            q.push({ nx, ny });
                            visited[nx][ny] = true;
                        }
                    }
                }

                if (!isShipAdjacent(grid, shipCells)) {
                    fillSurroundings(grid, shipCells);
                }
            }
        }
    }
}

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
// Загрузка экрана победы
SDL_Texture* loadBackground_Win(SDL_Renderer* renderer) {
    SDL_Texture* background_win = IMG_LoadTexture(renderer, "BG_Win.png");
    if (background_win == nullptr) {
        std::cerr << "Failed to load background_win image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return background_win;
}
// Загрузка экрана поражения
SDL_Texture* loadBackground_Loose(SDL_Renderer* renderer) {
    SDL_Texture* background_loose = IMG_LoadTexture(renderer, "BG_Loose.png");
    if (background_loose == nullptr) {
        std::cerr << "Failed to load background_loose image: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return background_loose;
}
//Для размещения
const int CELL_SIZE = 54;
const int CELL_SPACING = 6;
const int GRID_OFFSET_X = 72;
const int GRID_ENEMY_OFFSET_X = 792;
const int GRID_OFFSET_Y = 72;

struct Ship {
    int x, y, length;
    bool horizontal;
    Ship(int l) : x(0), y(0), length(l), horizontal(true) {}

    void rotate() {
        horizontal = !horizontal;
    }
};

//void renderGrid(SDL_Renderer* renderer) {
//    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
//    for (int i = 0; i < 10; ++i) {
//        for (int j = 0; j < 10; ++j) {
//            int x = GRID_OFFSET_X + i * (CELL_SIZE + CELL_SPACING);
//            int y = GRID_OFFSET_Y + j * (CELL_SIZE + CELL_SPACING);
//            SDL_Rect cell = { x, y, CELL_SIZE, CELL_SIZE };
//            SDL_RenderDrawRect(renderer, &cell);
//        }
//    }
//}

void renderShip(SDL_Renderer* renderer, const Ship& ship) {
    SDL_SetRenderDrawColor(renderer, 91, 110, 225, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < ship.length; ++i) {
        int x = GRID_OFFSET_X + (ship.x + (ship.horizontal ? i : 0)) * (CELL_SIZE + CELL_SPACING);
        int y = GRID_OFFSET_Y + (ship.y + (ship.horizontal ? 0 : i)) * (CELL_SIZE + CELL_SPACING);
        SDL_Rect cell = { x, y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &cell);
    }
}

bool isValidPlacement(const Ship& ship, const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < ship.length; ++i) {
        int x = ship.x + (ship.horizontal ? i : 0);
        int y = ship.y + (ship.horizontal ? 0 : i);
        if (x < 0 || x >= 10 || y < 0 || y >= 10 || grid[x][y] != 0) return false;
        // Проверьте окружающие ячейки на смежность
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && grid[nx][ny] != 0) return false;
            }
        }
    }
    return true;
}

void placeShip(Ship& ship, std::vector<std::vector<int>>& grid, int shipID) {
    for (int i = 0; i < ship.length; ++i) {
        int x = ship.x + (ship.horizontal ? i : 0);
        int y = ship.y + (ship.horizontal ? 0 : i);
        grid[x][y] = shipID;
    }
}
// Заполнение поля противника
void fillGridWithShips(std::vector<std::vector<int>>& grid) {
    std::vector<int> shipSizes = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
    int shipID = 1;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int size : shipSizes) {
        bool placed = false;
        while (!placed) {
            Ship ship(size);
            //ship.length = size;
            ship.horizontal = std::rand() % 2;
            ship.x = std::rand() % 10;
            ship.y = std::rand() % 10;

            if (isValidPlacement(ship, grid)) {
                placeShip(ship, grid, shipID);
                placed = true;
                shipID++;
            }
        }
    }
}
// Отрисовка поля игрока
void Player_fild_render(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int x = GRID_OFFSET_X + i * (CELL_SIZE + CELL_SPACING);
            int y = GRID_OFFSET_Y + j * (CELL_SIZE + CELL_SPACING);
            SDL_Rect cell = { x, y, CELL_SIZE, CELL_SIZE };
            if (grid[i][j] == -1) {
                SDL_SetRenderDrawColor(renderer, 135, 206, 250, SDL_ALPHA_OPAQUE); // Голубой для промаха
            }
            else if (grid[i][j] == -2) {
                SDL_SetRenderDrawColor(renderer, 0, 18, 129, SDL_ALPHA_OPAQUE); // Синий для попадания
            }
            else if (grid[i][j] == 0) {
                SDL_SetRenderDrawColor(renderer, 183, 180, 186, SDL_ALPHA_OPAQUE); // Для пустой клетки
            }
            else if (grid[i][j] > 0) {
                SDL_SetRenderDrawColor(renderer, 91, 110, 225, SDL_ALPHA_OPAQUE); // Для целого корабля
            }
            SDL_RenderFillRect(renderer, &cell);
            //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            //SDL_RenderDrawRect(renderer, &cell);
        }
    }
}
// Стрельба по пративнику
void renderCursor(SDL_Renderer* renderer, const std::vector<std::vector<int>>& grid, int cursorX, int cursorY) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int x = GRID_ENEMY_OFFSET_X + i * (CELL_SIZE + CELL_SPACING);
            int y = GRID_OFFSET_Y + j * (CELL_SIZE + CELL_SPACING);
            SDL_Rect cell = { x, y, CELL_SIZE, CELL_SIZE };
            if (grid[i][j] == -1) {
                SDL_SetRenderDrawColor(renderer, 135, 206, 250, SDL_ALPHA_OPAQUE); // Голубой для промаха
            }
            else if (grid[i][j] == -2) {
                SDL_SetRenderDrawColor(renderer, 0, 18, 129, SDL_ALPHA_OPAQUE); // Синий для попадания
            }
            else {
                SDL_SetRenderDrawColor(renderer, 183, 180, 186, SDL_ALPHA_OPAQUE); // Для пустой клетки
            }
            SDL_RenderFillRect(renderer, &cell);
            //SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            //SDL_RenderDrawRect(renderer, &cell);
        }
    }

    // Рисуем курсор
    int cursorXPos = GRID_ENEMY_OFFSET_X + cursorX * (CELL_SIZE + CELL_SPACING);
    int cursorYPos = GRID_OFFSET_Y + cursorY * (CELL_SIZE + CELL_SPACING);
    SDL_Rect cursorRect = { cursorXPos, cursorYPos, CELL_SIZE, CELL_SIZE };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(renderer, &cursorRect);
}
// Выстрел играка
bool handleShooting(std::vector<std::vector<int>>& grid, int cursorX, int cursorY) {
    if (grid[cursorX][cursorY] > 0) {
        grid[cursorX][cursorY] = -2; // Попадание
        return true; 
    }
    else if (grid[cursorX][cursorY] == 0) {
        grid[cursorX][cursorY] = -1; // Промах
        return false;
    }
}
// Проверка перед выстрелом играка
bool CheckHandleShooting(std::vector<std::vector<int>>& grid, int cursorX, int cursorY) {
    if (grid[cursorX][cursorY] == -2 || grid[cursorX][cursorY] == -1) {
        return false;
    }
    else {
        return true;
    }
}

// Атака апонента
bool EnemyAttack(std::vector<std::vector<int>>& grid) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int rows = grid.size();
    int cols = grid[0].size();

    bool found = false;
    while (!found) {
        int x = std::rand() % rows;
        int y = std::rand() % cols;

        if (grid[x][y] != -1 && grid[x][y] != -2) {
            if (grid[x][y] == 0) {
                grid[x][y] = -1; // Промах
                return false;
            }
            else if (grid[x][y] >= 1 && grid[x][y] <= 10) {
                grid[x][y] = -2; // Попадание
                return true;
            }
            found = true;
        }
    }
}

// Изменение счёта
int ChangScore(std::vector<std::vector<int>>& grid, std::vector<std::vector<int>>& enemy_field) {
    int score = 0;
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            if (grid[i][j] == -2) {
                score = score - 10;
            }
            if (enemy_field[i][j] == -2) {
                score = score + 10;
            }
        }
    }
    return score;
}
// Проверка на целые корабли
bool CheckShip(std::vector<std::vector<int>>& grid) {
    bool existence = false;
    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            if (grid[i][j] > 0) {
                existence = true;
            }
        }
    }
    return existence;
}

void ArrowReset(std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = 0;
        }
    }
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

    // Загрузка экрана победы
    SDL_Texture* background_win = loadBackground_Win(renderer);
    if (background_win == nullptr) {
        return 1;
    }

    // Загрузка экрана поражения
    SDL_Texture* background_loose = loadBackground_Loose(renderer);
    if (background_loose == nullptr) {
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

    int score = 100;
    int number_of_shots = 100;
    bool Pause = false;
    bool Placement = true;
    bool Play = false;
    bool Player_attack = false;
    bool Enemy_attack = false;
    bool Win = false;
    bool Loose = false;
    int currentShip = 0;
    int cursorX = 0;
    int cursorY = 0;

    std::vector<Ship> ships = { Ship(4), Ship(3), Ship(3), Ship(2), Ship(2), Ship(2), Ship(1), Ship(1), Ship(1), Ship(1) };
    //std::vector<Ship> ships = { Ship(1), Ship(1) };
    std::vector<std::vector<int>> grid(10, std::vector<int>(10, 0));

    std::vector<std::vector<int>> enemy_field(10, std::vector<int>(10, 0));

    // Основной игровой цикл
    bool running = true;
    while (running) {
        // Обработка событий
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (Placement) {
                    switch (event.key.keysym.sym) {
                    case SDLK_w:
                        if (ships[currentShip].y > 0) ships[currentShip].y--;
                        break;
                    case SDLK_s:
                        if (ships[currentShip].horizontal && ships[currentShip].y < 9 || !ships[currentShip].horizontal && ships[currentShip].y < 10 - ships[currentShip].length)
                            ships[currentShip].y++;
                        break;
                    case SDLK_a:
                        if (ships[currentShip].x > 0) ships[currentShip].x--;
                        break;
                    case SDLK_d:
                        if (ships[currentShip].horizontal && ships[currentShip].x < 10 - ships[currentShip].length || !ships[currentShip].horizontal && ships[currentShip].x < 9)
                            ships[currentShip].x++;
                        break;
                    case SDLK_r:
                        ships[currentShip].rotate();
                        break;
                    case SDLK_RETURN:
                        if (isValidPlacement(ships[currentShip], grid)) {
                            placeShip(ships[currentShip], grid, currentShip + 1);
                            currentShip++;
                        }
                        printGrid(grid);
                    }
                }
                if (Player_attack) {
                    switch (event.key.keysym.sym) {
                    case SDLK_w:
                        if (cursorY > 0) cursorY--;
                        break;
                    case SDLK_s:
                        if (cursorY < 9) cursorY++;
                        break;
                    case SDLK_a:
                        if (cursorX > 0) cursorX--;
                        break;
                    case SDLK_d:
                        if (cursorX < 9) cursorX++;
                        break;
                    case SDLK_RETURN:
                        if (CheckHandleShooting(enemy_field, cursorX, cursorY)) {
                            if (handleShooting(enemy_field, cursorX, cursorY)) {
                                printGrid(enemy_field);
                            }
                            else {
                                printGrid(enemy_field);
                                Player_attack = false;
                                Enemy_attack = true;
                            }
                            number_of_shots--;
                            score = number_of_shots + ChangScore(grid, enemy_field);
                            if (CheckShip(enemy_field) == false) {
                                Win = true;
                                Play = false;
                                Player_attack = false;
                                Enemy_attack = false;
                            }
                        }
                        break;
                    }
                }
                if (Win || Loose) {
                    
                    if (event.key.keysym.sym == SDLK_r) {
                        score = 100;
                        number_of_shots = 100;
                        Pause = false;
                        Placement = true;
                        Play = false;
                        Player_attack = false;
                        Enemy_attack = false;
                        Win = false;
                        Loose = false;
                        currentShip = 0;
                        cursorX = 0;
                        cursorY = 0;
                        ArrowReset(grid);
                        ArrowReset(enemy_field);
                        std::cout << "rrrrrrrrrrrrrrrrrrrrrrrrrrr" << std::endl;
                        printGrid(grid);
                        printGrid(enemy_field);
                        for (int i = 0; i < 10; i++) {
                            ships[i].y = 0;
                            ships[i].x = 0;
                            if (ships[i].horizontal == false) {
                                ships[i].rotate();
                            }
                        }
                    }
                }
            }
        }
        // Очистка экрана
        SDL_RenderClear(renderer);

        // Отрисовка фонового изображения
        {
            if (Win) {
                SDL_RenderCopy(renderer, background_win, nullptr, nullptr);
                //std::cout << "Win" << std::endl;
            }
            else if (Loose) {
                SDL_RenderCopy(renderer, background_loose, nullptr, nullptr);
                //std::cout << "Loose" << std::endl;
            }
            else {
                SDL_RenderCopy(renderer, background, nullptr, nullptr);
                //std::cout << "Play" << std::endl;
            }
        }
        //SDL_RenderCopy(renderer, background, nullptr, nullptr);
        
        // Вывод текста при размещении
        if (Placement == true)
        {
            SDL_Color textColor = { 0, 0, 0, 255 };
            // Первая строчка
            {
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "w/a/s/d - передвижение", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { 66, 726, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture); }
            // Вторая строчка
            {
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "r - поворот", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { 66, 798, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture); }
            // Третья строчка
            {
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "Enter - разместить", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { 66, 870, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture); }
        }
        // Отрисовка счёта при игре
        if (Play == true) {
            SDL_Color textColor = { 0, 0, 0, 255 };
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, std::to_string(score).c_str(), textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { WINDOW_WIDTH - 450, 198, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        // Вывод текста при игре
        if (Play == true)
        {
            SDL_Color textColor = { 0, 0, 0, 255 };
            // Первая строчка
            {
                SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "w/a/s/d - передвижение", textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
                SDL_Rect textRect = { 66, 726, textWidth, textHeight };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture); }
            // Вторая строчка
            {
                SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, "Enter - выстрел", textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textWidth, textHeight;
                SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
                SDL_Rect textRect = { 66, 798, textWidth, textHeight };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture); }
        }
        // Отрисовка счёта при выигрыше
        if (Win == true) {
            SDL_Color textColor = { 0, 0, 0, 255 };
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, std::to_string(score).c_str(), textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { 582, 594, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        // Отрисовка счёта при проигрыше
        if (Loose == true) {
            SDL_Color textColor = { 0, 0, 0, 255 };
            SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font, std::to_string(score).c_str(), textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);
            SDL_Rect textRect = { 306, 594, textWidth, textHeight };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        // Рендер размещённых кораблей
        if (Placement) {
            for (int i = 0; i < currentShip; ++i) {
                renderShip(renderer, ships[i]);
            }
        }
        // Рендер кораблей, которые щас размещаются и изменение статуса
        if (currentShip < ships.size()) {
            renderShip(renderer, ships[currentShip]);
        }
        else if(Placement == true)
        {
            std::cout << "not plasing" << std::endl;
            Placement = false;
            Play = true;
            Player_attack = true;
            fillGridWithShips(enemy_field);
            printGrid(enemy_field);
        }
        // Рендер во время игры
        else if (Play == true) {
            renderCursor(renderer, enemy_field, cursorX, cursorY);
            surroundSunkShips(enemy_field);
            Player_fild_render(renderer, grid);
        }
        
        // Обновление экрана
        SDL_RenderPresent(renderer);

        if (Pause) {
            SDL_Delay(500);
            Pause = false;
        }
        // Атака опанента
        if (Enemy_attack) {
            std::cout << "tut budet atacka" << std::endl;
            if (EnemyAttack(grid)) {
                surroundSunkShips(grid);
                Pause = true;
            }
            else {
                surroundSunkShips(grid);
                Player_attack = true;
                Enemy_attack = false;
            }
            score = number_of_shots + ChangScore(grid, enemy_field);
            if (CheckShip(grid) == false) {
                Loose = true;
                Play = false;
                Player_attack = false;
                Enemy_attack = false;
            }
        }
    }

    // Очистка ресурсов
    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}