#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "f_names.h"





/*
CONVENTION FOR TILE SIDE NUMBERING:
    0
   341
    2

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

int empty_grid_row(grid G, int row){
    int i;
    
    for(i = 0; i < N_CARDS*2; i++)
        if(G[row][i] != NULL)
            return 0;
    
    return 1;
}

int empty_grid_col(grid G, int col){
    int i;
    
    //return 0;
    
    for(i = 0; i < N_CARDS*2; i++)
        if(G[i][col] != NULL)
            return 0;
    
    return 1;
}

void basic_print_grid(grid G){
    
    int i = 0, 
        j = 0, 
        firstcol = 0,
        lastcol  = N_CARDS*2,
        lenght   = N_CARDS*2;
    
    while(empty_grid_col(G, i) == 1){
        firstcol ++;
        i++;
    }
    
    i = lenght;
    
    while(empty_grid_col(G, i) == 1){
        lastcol--;
        i--;
    }
    
    
    for(i = 0; i < lenght; i++){
        
        if(empty_grid_row(G, i) == 0){
            for(j = firstcol; j < lastcol; j++){
                if(G[i][j] != NULL)
                    printf("  %c  ", G[i][j]->sides[0]);
                
                else
                    printf("     ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol; j++){
                if(G[i][j] != NULL)
                    printf(" %c%c%c ", G[i][j]->sides[3], G[i][j]->sides[4], G[i][j]->sides[1]);
                
                else
                    printf("     ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol; j++){
                if(G[i][j] != NULL)
                    printf("  %c  ", G[i][j]->sides[2]);
            
                else
                    printf("     ");
            }
            printf("\n\n");
        }    
    }
}

tile** readcsv(char* filename) {
    tile **deck = malloc(sizeof(tile *) * 72);
    for (int i = 0; i <= N_CARDS; i++)
    {
        deck[i] = Tile();
    }
    int i = 0, j = 0;
    FILE *fp;
    char c;
    fp = fopen(filename, "r");

    c = fgetc(fp);
    deck[i]->sides[j] = c;
    j++;
    while (c != EOF)
    {
        c = fgetc(fp);
        if (c == ',')
        {
            c = fgetc(fp);
            if (c == 'a') {
                deck[i]->special = true;
                deck[i]->sides[j] = 'p';
            }
            else
                deck[i]->sides[j] = c;
            // printf("%c|",deck[i].sides[j]);

            j++;
        }
        if (c == '\n')
        {
            // printf("%d \n",i);
            c = fgetc(fp);
            // printf("%c|",c);
            i++;
            j = 0;
            if (c == 'a') {
                deck[i]->special = true;
                deck[i]->sides[j] = 'p';
            }
            else
                deck[i]->sides[j] = c;
            j++;
        }
    }

    fclose(fp);

    return deck;
}

void print_tile(tile* t) {
    printf("%c|%c|%c|%c|%c   special: %d\n", t->sides[0], t->sides[1], t->sides[2], t->sides[3], t->sides[4], (int)t->special);
}

void free_deck(tile** deck) {
    for (int i = 0; i < N_CARDS+1; i++) 
        free(deck[i]);
    free(deck);
}

int main() {
    srand(time(NULL));

    
    grid grid = Grid();
    for(int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = Tile();
        }
    }


    //basic_print_grid(g);

    tile** deck = readcsv("tuiles_base_simplifiees.csv");

    for (int i = 0; i < 72; i++) {
        print_tile(deck[i]);
    }

    free_grid(grid);
    free_deck(deck);
}
