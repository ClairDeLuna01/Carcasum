#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define JOUEURS 2


typedef unsigned int uint;



typedef struct tile {
    char up, down, left, right, center; 
    uint roads[4]; // []
    unsigned short meeple; // 0000 0000 0000 0000
    bool closed_up, closed_down, closed_left, closed_right; // 
} tile;


void rotate_tile_clockwise(tile* t) { // untested 
    char tmp = t->up;
    t->up = t->left;
    t->left = t->down;
    t->down = t->right;
    t->right = tmp;

    for (int i = 0; i < 4; i++)
        if (t->roads[i] == 4) 
            t->roads[i] = 1;
        else
            t->roads[i]++;
    
    t->meeple << 1;
    if (t->meeple & 0b0000001000000000)
        t->meeple | 

} 


int main() {
    int joueur = 0;

}
