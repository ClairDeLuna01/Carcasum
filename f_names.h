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

int empty_grid_col(grid, int); // a faire [Arthur C]

int empty_grid_row(grid, int); // a faire [Arthur C]

void basic_print_grid(grid); // à faire [Arthur C]

//void add_tile_to_grid(grid, tile*); // pas nécassaire 

tile** deck_shuffle(tile**); // à faire [Samy]

tile** readcsv(char*); // à faire [Samy]


