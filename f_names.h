#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N_CARDS 72
#define players 2

typedef struct tile_t {
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;

typedef tile*** grid;

grid Grid(); // fait

void rotate_tile_clockwise(tile*); // fait 

void rotate_tile_counterclockwise(tile*); // fait 

tile* Tile(); // fait

void free_grid(grid); // fait

//void free_tile(tile*); // pas nécassaire 

void basic_print_grid(grid); // à faire

void add_tile_to_grid(grid, tile*); // à faire

tile** deck_shuffle(tile**); // à faire

tile** readcsv(char*); // à faire


