#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N_CARDS 72
#define players 2

typedef unsigned int uint;

typedef struct tile_t {
    char sides[5]; // c = Champ, V = Ville, c = Champs, m = Monastère, v = Village, B = blazon
    uint meeple[5];
    uint claimed[5];
    bool special;
    bool closed[4];
    bool istemp;
} tile;

typedef struct joueur_t {
    uint points;
    uint meeples;
    uint id;
} joueur;

typedef struct coord_t {
    int x, y;
} coord;

typedef tile*** grid;

grid Grid(); // fait 

void rotate_tile_clockwise(tile*); // fait 

void rotate_tile_counterclockwise(tile*); // fait 

tile* Tile(); // fait

void free_grid(grid); // fait

//void free_tile(tile*); // pas nécassaire 

int empty_grid_col(grid, int); // fait

int empty_grid_row(grid, int); // fait

void basic_print_grid(grid); // fait

void print_grid_color(grid); // todo AC

//void add_tile_to_grid(grid, tile*); // pas nécassaire 

tile** deck_shuffle(tile**); // fait

tile** readcsv(char*); // fait

void free_deck(tile**); // fait

joueur* Joueur(uint id); // fait

void gen_rand_valid_grill(grid, tile**); // fait

coord* get_all_available_positions(grid, uint* size); // todo samy

bool is_valid_pos(grid, tile*, coord); // fait

coord* get_all_possible_possitions(grid, tile*); // 

void print_grid_with_pos(grid, coord*, tile*, uint size); // fait

void place_tile(grid, coord, tile); //

int* check_road(grid, coord, joueur[5]); // todo AB

int* check_town(grid, coord, joueur[5]); //

int check_abbey(grid, coord, joueur[5]); //

uint get_road_points(grid, coord); //

uint get_town_points(grid, coord); //

uint get_plain_points(grid, coord); //

