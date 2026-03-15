#include "save.h"
#include <stdio.h>

bool saveGame(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) return false;
    
    size_t written = fwrite(game, sizeof(GameState), 1, file);
    fclose(file);
    return written == 1;
}

bool loadGame(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return false;
    
    size_t read = fread(game, sizeof(GameState), 1, file);
    fclose(file);
    return read == 1;
}