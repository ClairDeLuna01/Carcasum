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
    struct list2d *up, *down, *left, *right;
    tile t;
    bool allocated;
} list2d;



list2d* List2d() {
    list2d* l = malloc(sizeof(list2d));
    l->up = NULL; 
    l->down = NULL; 
    l->right = NULL; 
    l->left = NULL;
    return l;
}

void link_up(list2d* list, tile new_tile) {
    if (list->up == NULL) { // add a row to the top if needed
        list2d* L = list; // we need the list to stay rectangular
        while (L->left != NULL) { // go as far left as possible
            L = L->left;
        }
        while (L != NULL) {
            list2d* l = List2d();
            l->allocated = false;
            L->up = l;
            L = L->right;
            // add linking to other tiles 
        }
    }

    if (list->up->allocated == true) {
        perror("Error, cannot link tile, already linked");
        exit(1);
    }

    list->up->t = new_tile;
    list->up->down = list;
    list->up->allocated = true;

}

void link_down(list2d* list, tile new_tile) {
    if (list->down == NULL) { // add a row to the top if needed
        list2d* L = list; // we need the list to stay rectangular
        while (L->left != NULL) { // go as far left as possible
            L = L->left;
        }
        while (L != NULL) {
            list2d* l = List2d();
            l->allocated = false;
            L->down = l;
            L = L->right;
        }
    }

    if (list->down->allocated == true) {
        perror("Error, cannot link tile, already linked");
        exit(1);
    }

    list->down->t = new_tile;
    list->down->up = list;
    list->down->allocated = true;
}

void link_right(list2d* list, tile new_tile) {
    if (list->right == NULL) { // add a row to the top if needed
        list2d* L = list; // we need the list to stay rectangular
        while (L->up != NULL) { // go as far up as possible
            L = L->up;
        }
        while (L != NULL) {
            list2d* l = List2d();
            l->allocated = false;
            L->right = l;
            L = L->down;
        }
    }

    if (list->right->allocated == true) {
        perror("Error, cannot link tile, already linked");
        exit(1);
    }

    list->right->t = new_tile;
    list->right->left = list;
    list->right->allocated = true;
}

void link_left(list2d* list, tile new_tile) {
        if (list->right == NULL) { // add a row to the top if needed
        list2d* L = list; // we need the list to stay rectangular
        while (L->up != NULL) { // go as far up as possible
            L = L->up;
        }
        while (L != NULL) {
            list2d* l = List2d();
            l->allocated = false;
            L->left = l;
            L = L->down;
        }
    }

    if (list->left->allocated == true) {
        perror("Error, cannot link tile, already linked");
        exit(1);
    }

    list->left->t = new_tile;
    list->left->right = list;
    list->left->allocated = true;
}

void _free_row(list2d* row) {
    while(row->right != NULL) {
        list2d* tmp = row->right;
        // free tile
        free(row);
        row = tmp;
    }
}

void free_list(list2d* list) {
    while(list->left != NULL)  // we go to the top left corner
        list = list->left;
    while(list->up != NULL)
        list = list->up;
    
    while (list->down != NULL) {
        _free_row(list);
        list = list->down;
    }
    _free_row(list);
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



int main() {
    // test code
    tile test_tile;
    list2d* grid = List2d();
    grid->allocated = true; // cursed
    link_up(grid, test_tile);
    link_down(grid, test_tile);
    link_left(grid, test_tile);
    link_right(grid, test_tile);
    link_right(grid->down, test_tile);
    link_left(grid->down, test_tile);
    link_right(grid->up, test_tile);
    link_left(grid->up, test_tile);


}
