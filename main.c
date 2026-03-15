#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "save.h"

#define SAVE_FILE "snake_save.dat"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    // Création de la fenêtre
    SDL_Window *window = SDL_CreateWindow("Snike Game",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         SCREEN_WIDTH, SCREEN_HEIGHT,
                                         SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Création du renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
                                                SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    srand(time(NULL));
    
    GameState game;
    bool running = true;
    bool newGame = true;
    Uint32 lastUpdate = 0;
    
    // Menu initial
    printf("=== SNAKE GAME ===\n");
    printf("1. Nouvelle partie\n");
    printf("2. Charger partie\n");
    printf("Choix: ");
    
    int choice = 1;
    if (scanf("%d", &choice) != 1) {
        choice = 1;
    }
    
    if (choice == 2) {
        if (loadGame(&game, SAVE_FILE)) {
            newGame = false;
            printf("Partie chargee avec succes!\n");
        } else {
            printf("Erreur de chargement. Nouvelle partie.\n");
        }
    }
    
    if (newGame) {
        initGame(&game);
    }
    
    SDL_Event event;
    
    // Boucle principale
    while (running) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            handleInput(&game, &event);
            
            // Sauvegarde avec la touche 'L'
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_l) {
                if (saveGame(&game, SAVE_FILE)) {
                    printf("Partie sauvegardee!\n");
                }
            }
        }
        
        // Mise à jour du jeu avec gestion de la vitesse
        Uint32 currentTime = SDL_GetTicks();
        Uint32 updateInterval = game.speed ? 80 : 150; // ms par frame
        
        if (currentTime - lastUpdate > updateInterval) {
            updateGame(&game);
            lastUpdate = currentTime;
        }
        
        // Rendu
        renderGame(renderer, &game);
        
        // Petite pause pour éviter une utilisation CPU excessive
        SDL_Delay(10);
        
        // Vérifier game over
        if (game.gameOver) {
            printf("Game Over! Score: %d\n", game.score);
            SDL_Delay(2000);
            running = false;
        }
    }
    
    // Nettoyage
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}