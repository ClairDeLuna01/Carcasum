#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define JOUEURS 2


typedef unsigned int uint;

/*
CONVENTION FOR TILE SIDE NUMBERING:
    1
   4#2
    3

START UP
INCREMENT CLOCKWISE

* for corners start at the top right corner
eg: 

   4 1
    #
   3 2
*/

typedef struct tile {
    char up, down, left, right, center; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed_up, closed_down, closed_left, closed_right;
} tile;


void rotate_tile_clockwise(tile* t) { // untested 
    char tmp = t->up;
    t->up = t->left;
    t->left = t->down;
    t->down = t->right;
    t->right = tmp;
} 

void rotate_tile_counter_clockwise(tile* t) { // untested
    char tmp = t->up;
    t->up = t->right;
    t->right = t->down;
    t->down = t->left;
    t->left = tmp;
}


int main() {
    int joueur = 0;



}
