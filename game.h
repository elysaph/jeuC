#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20
#define CELL_SIZE 20
#define GRID_WIDTH (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)
#define MAX_SNAKE_LENGTH 1000

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    int direction; // 0: haut, 1: droite, 2: bas, 3: gauche
} Snake;

typedef struct {
    Point positions[3];
    int count;
    bool active[3];
} Apples;

typedef struct {
    Snake snake;
    Apples apples;
    int score;
    int applesEaten;
    bool gameOver;
    int speed; // 0: lent, 1: rapide
} GameState;

// Fonctions principales du jeu
void initGame(GameState *game);
void handleInput(GameState *game, SDL_Event *event);
void updateGame(GameState *game);
void renderGame(SDL_Renderer *renderer, GameState *game);
void generateApples(Apples *apples);
bool checkCollision(GameState *game);
void moveSnake(GameState *game);
bool checkAppleCollision(GameState *game);

#endif