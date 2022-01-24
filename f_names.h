#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct tile_t {
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;

typedef tile*** grid;

grid Grid(); // fait // non testée

void rotate_tile_clockwise(tile*); // fait // non testée

void rotate_tile_counter_clockwise(tile*); // fait // non testée

tile* Tile(); // fait // non testée

void free_grid(grid); // à faire

void free_tile(tile*); // à faire

void basic_print_grid(grid); // à faire

void add_tile_to_grid(grid, tile*); // à faire

tile** deck_shuffle(tile**); // à faire

tile** readcsv(char*) // à faire


