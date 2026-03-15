#include "game.h"
#include <stdlib.h>

void initGame(GameState *game) {
    // Initialisation du serpent
    game->snake.length = 3;
    game->snake.direction = 1; // Commence vers la droite
    
    // Position initiale du serpent
    for (int i = 0; i < game->snake.length; i++) {
        game->snake.body[i].x = GRID_WIDTH / 2 - i;
        game->snake.body[i].y = GRID_HEIGHT / 2;
    }
    
    // Initialisation des pommes
    game->apples.count = 3;
    for (int i = 0; i < 3; i++) {
        game->apples.active[i] = false;
    }
    generateApples(&game->apples);
    
    game->score = 0;
    game->applesEaten = 0;
    game->gameOver = false;
    game->speed = 0; // Lent par défaut
}

void generateApples(Apples *apples) {
    for (int i = 0; i < 3; i++) {
        if (!apples->active[i]) {
            apples->positions[i].x = rand() % GRID_WIDTH;
            apples->positions[i].y = rand() % GRID_HEIGHT;
            apples->active[i] = true;
        }
    }
}

void handleInput(GameState *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_UP:
                if (game->snake.direction != 2) // Pas de retour immédiat
                    game->snake.direction = 0;
                break;
            case SDLK_RIGHT:
                if (game->snake.direction != 3)
                    game->snake.direction = 1;
                break;
            case SDLK_DOWN:
                if (game->snake.direction != 0)
                    game->snake.direction = 2;
                break;
            case SDLK_LEFT:
                if (game->snake.direction != 1)
                    game->snake.direction = 3;
                break;
            case SDLK_s: // Changer de vitesse
                game->speed = !game->speed;
                break;
        }
    }
}

void moveSnake(GameState *game) {
    // Déplacer le corps
    for (int i = game->snake.length - 1; i > 0; i--) {
        game->snake.body[i] = game->snake.body[i - 1];
    }
    
    // Déplacer la tête
    Point *head = &game->snake.body[0];
    switch (game->snake.direction) {
        case 0: head->y--; break;
        case 1: head->x++; break;
        case 2: head->y++; break;
        case 3: head->x--; break;
    }
}

bool checkAppleCollision(GameState *game) {
    Point *head = &game->snake.body[0];
    bool ateApple = false;
    
    for (int i = 0; i < 3; i++) {
        if (game->apples.active[i] && 
            head->x == game->apples.positions[i].x && 
            head->y == game->apples.positions[i].y) {
            
            game->apples.active[i] = false;
            game->applesEaten++;
            game->score += 10;
            ateApple = true;
            
            // Si toutes les pommes sont mangées
            if (game->applesEaten >= 3) {
                int oldLength = game->snake.length;
                int newLength = oldLength + 3;
                if (newLength > MAX_SNAKE_LENGTH) {
                    newLength = MAX_SNAKE_LENGTH;
                }

                // Initialiser les nouvelles cases pour éviter des positions indéfinies
                for (int j = oldLength; j < newLength; j++) {
                    game->snake.body[j] = game->snake.body[oldLength - 1];
                }
                game->snake.length = newLength;
                game->applesEaten = 0;
                generateApples(&game->apples);
            }
        }
    }
    
    return ateApple;
}

bool checkCollision(GameState *game) {
    Point *head = &game->snake.body[0];
    
    // Collision avec les murs
    if (head->x < 0 || head->x >= GRID_WIDTH || 
        head->y < 0 || head->y >= GRID_HEIGHT) {
        return true;
    }
    
    // Collision avec le corps
    for (int i = 1; i < game->snake.length; i++) {
        if (head->x == game->snake.body[i].x && 
            head->y == game->snake.body[i].y) {
            return true;
        }
    }
    
    return false;
}

void updateGame(GameState *game) {
    if (game->gameOver) return;
    
    moveSnake(game);

    // Toujours vérifier les collisions après déplacement
    if (checkCollision(game)) {
        game->gameOver = true;
        return;
    }

    checkAppleCollision(game);
}

void renderGame(SDL_Renderer *renderer, GameState *game) {
    // Effacer l'écran
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Dessiner la grille (optionnel)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    for (int x = 0; x < SCREEN_WIDTH; x += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }
    for (int y = 0; y < SCREEN_HEIGHT; y += CELL_SIZE) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
    
    // Dessiner le serpent
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < game->snake.length; i++) {
        SDL_Rect rect = {
            game->snake.body[i].x * CELL_SIZE,
            game->snake.body[i].y * CELL_SIZE,
            CELL_SIZE - 1,
            CELL_SIZE - 1
        };
        SDL_RenderFillRect(renderer, &rect);
    }
    
    // Dessiner les pommes
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < 3; i++) {
        if (game->apples.active[i]) {
            SDL_Rect rect = {
                game->apples.positions[i].x * CELL_SIZE,
                game->apples.positions[i].y * CELL_SIZE,
                CELL_SIZE - 1,
                CELL_SIZE - 1
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    
    // Afficher le score
    // (Vous pouvez ajouter du texte avec SDL_ttf)
    
    SDL_RenderPresent(renderer);
}