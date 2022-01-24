#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct tile_t {
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;


tile*** Grid();

void rotate_tile_clockwise(tile*);

void rotate_tile_counter_clockwise(tile*);

tile* Tile();




