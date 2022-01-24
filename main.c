#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "f_names.h"


#define JOUEURS 2
#define N_CARDS 72


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


typedef struct tile_t {
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;


tile*** Grid() {
    tile*** l = malloc(sizeof(tile**) * N_CARDS * 2 + 1);
    for (int i = 0; i < N_CARDS * 2; i++) {
        l[i] = malloc(sizeof(tile*) * N_CARDS * 2 + 1);
        for (int j = 0; j < N_CARDS * 2; j++) {
            l[i][j] = NULL;
        }
    }
    return l;
}



void rotate_tile_clockwise(tile* t) { // untested 
    char tmp = t->sides[0];
    t->sides[0] = t->sides[3];
    t->sides[3] = t->sides[2];
    t->sides[2] = t->sides[1];
    t->sides[1] = tmp;
} 

void rotate_tile_counter_clockwise(tile* t) { // untested
    char tmp = t->sides[0];
    t->sides[0] = t->sides[1];
    t->sides[1] = t->sides[2];
    t->sides[2] = t->sides[3];
    t->sides[3] = tmp;
}

tile* Tile()
{
    tile* T = malloc(sizeof(tile));
    if (!T) {
        perror("Error while allocating tile");
        exit(1);
    }
    T->meeple  = 0;
    T->special = 0;
    
    int i = 0;
    
    for(; i < 4; i++)
    {
        T->closed[i] = 0; 
        T->sides[i] = 'C';
    }
    
    T->sides[4] = 'C';
    
    return T;
}

int main() {
    tile*** grid = Grid();

    int a;
    scanf("%d", &a);
    return 0;
}
