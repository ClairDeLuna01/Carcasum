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
    char sides[5]; // C = Champ, V = Ville, C = Champs, M = Monastère 
    uint meeple;
    bool special;
    bool closed[4];
} tile;

typedef struct list2d {
    list2d *up, *down, *left, *right;
    tile t;
} list2d;

list2d* List2d(tile first_tile) {
    list2d* l = malloc(sizeof(list2d));
    l->up = NULL; 
    l->down = NULL; 
    l->right = NULL; 
    l->left = NULL;
    l->t = first_tile;
    return l;
}

void link_up(list2d* list, tile new_tile) {
    if (list->up != NULL) {
        perror("Error, cannot link tile");
        exit(1);
    }

    list2d* new_link = malloc(sizeof(list2d));
    list->up = new_link;
    new_link->t = new_tile;
    new_link->down = list;
}

void link_down(list2d* list, tile new_tile) {
    if (list->up != NULL) {
        perror("Error, cannot link tile");
        exit(1);
    }

    list2d* new_link = malloc(sizeof(list2d));
    list->down = new_link;
    new_link->t = new_tile;
    new_link->up = list;
}

void link_right(list2d* list, tile new_tile) {
    if (list->up != NULL) {
        perror("Error, cannot link tile");
        exit(1);
    }

    list2d* new_link = malloc(sizeof(list2d));
    list->right = new_link;
    new_link->t = new_tile;
    new_link->left = list;
}

void link_left(list2d* list, tile new_tile) {
    if (list->up != NULL) {
        perror("Error, cannot link tile");
        exit(1);
    }

    list2d* new_link = malloc(sizeof(list2d));
    list->left = new_link;
    new_link->t = new_tile;
    new_link->right = list;
}

void free_list(list2d* list) { // would be 1000% simpler in a language that supports objects
    list2d* up = NULL;
    list2d* down = NULL;
    list2d* left = NULL;
    list2d* right = NULL;


    if (list->up != NULL) {
        list->up->down = NULL;
        up = list->up;
        list->up = NULL;
    }
    if (list->down != NULL) {
        list->down->up = NULL;
        down = list->down;
        list->down = NULL;
    }
    if (list->left != NULL) {
        list->left->right = NULL;
        left = list->left;
        list->left = NULL;
    }
    if (list->right != NULL) {
        list->right->left = NULL;
        right = list->right;
        list->right = NULL;
    }
    
    if (up != NULL)
        free_list(list->up);
    if (down != NULL)
        free_list(list->down);
    if (left != NULL)
        free_list(list->left);
    if (right != NULL)
        free_list(list->right);
    

    free(list);
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

void alloc_tile(tile* t)
{
    tile *T = malloc(sizeof(tile));
    
    T->meeple  = 0;
    T->special = 0;
    
    int i = 0;
    
    for(; i < 4; i++)
    {
        T->closed[i] = 0; 
        T->sides[i] = "C";
    }
    
    T->sides[4] = "C";
    
    return T;
}

int main() {
    // test code
    tile test_tile;
    list2d* grid = List2d(test_tile);
    link_up(grid, test_tile);
    link_down(grid, test_tile);
    link_left(grid, test_tile);
    link_right(grid, test_tile);
    link_up(grid, test_tile);
    link_up(grid, test_tile);
    link_up(grid, test_tile);
    link_up(grid, test_tile);
    link_up(grid, test_tile);



}
