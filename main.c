#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "f_names.h"





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





grid Grid() {
    grid l = malloc(sizeof(tile**) * N_CARDS * 2);
    for (int i = 0; i < N_CARDS * 2; i++) {
        l[i] = malloc(sizeof(tile*) * N_CARDS * 2);
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

void rotate_tile_counterclockwise(tile* t) { // untested
    char tmp = t->sides[0];
    t->sides[0] = t->sides[1];
    t->sides[1] = t->sides[2];
    t->sides[2] = t->sides[3];
    t->sides[3] = tmp;
}

tile* Tile()
{
    srand(time(NULL));
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

void free_grid(grid g){
    int i,j;
    for(i=0;i<N_CARDS*2;i++){
        for(j=0;j<N_CARDS*2;j++)
            free(g[i][j]);
        free(g[i]);
    }
    free(g);      // nécéssaire
}

int main() {
    grid g = Grid();
    for(int i = 0; i < N_CARDS * 2; i++) {
        for (int j = 0; j < N_CARDS * 2; j++) {
            g[i][j] = Tile();
        }
    }



    free_grid(g);

    

    
}
