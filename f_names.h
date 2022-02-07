#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N_CARDS 72
#define players 2

typedef unsigned int uint;

typedef struct tile_t {
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;

typedef struct joueur_t {
    uint points;
    uint meeples;
    uint id;
} joueur;

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

tile** deck_shuffle(tile**); // à faire [Samy]

tile** readcsv(char*); // fait

void free_deck(tile**); // fait

joueur* Joueur(); // todo AB

void free_joueur(); // 

int** get_all_available_positions(grid); //

bool is_valid_pos(grid, tile*, int, int);

int** get_all_possible_possitions(grid, tile*); //

void print_grid_with_pos(grid, int**, tile*); //

void place_tile(grid, int, int, tile); //

bool check_road(grid, int, int); //

bool check_town(grid, int, int); //

bool check_abbey(grid, int, int); //

uint get_road_points(grid, int, int); //

uint get_town_points(grid, int, int); //

uint get_plain_points(grid, int, int); //

