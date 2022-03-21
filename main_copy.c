#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
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
    for (int i = 0; i < 5; i++)
        T->meeple[i]  = 0;
    T->special = 0;
    
    int i = 0;
    
    for(; i < 4; i++)
    {
        T->closed[i] = 0; 
        T->sides[i] = 'p';
    }
    
    T->sides[4] = 'p';
    T->istemp = false;
    return T;
}

void free_grid(grid g){
    int i,j;
    for(i=0;i<N_CARDS*2;i++){
        for(j=0;j<N_CARDS*2;j++) {
            free(g[i][j]);
        }
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
        deck[i] = Tile();
    
    int i = 0, j = 1;
    FILE *fp;
    char c;
    
    fp = fopen(filename, "r");
    c  = fgetc(fp);
    deck[i]->sides[0] = c;
    
    while (c != EOF)
    {
        
        if (c == ',' || j == 0)
        {
            c = fgetc(fp);
            
            if(c == 'a') {
                deck[i]->special = true;
                deck[i]->sides[j] = 'p';
            }
            
            if(c == 'b') {
                deck[i]->special = true;
                deck[i]->sides[j] = 'V';
            }
            
            if(c != 'a' && c != 'b')
                deck[i]->sides[j] = c;
            
            j++;
        }
        
        if (c == '\n')
        {
            i++;
            j = 0;
        }
        
        else
            c = fgetc(fp);
    }

    fclose(fp);

    return deck;
}

void print_tile(tile* t) {
    printf("%c|%c|%c|%c|%c   special: %d\n", t->sides[0], t->sides[1], t->sides[2], t->sides[3], t->sides[4], (int)t->special);
}

void free_deck(tile** deck) {
    for (int i = 0; i < N_CARDS+1; i++) {
        free(deck[i]);
    }
    free(deck);
}

joueur* Joueur(uint id) {
    joueur *J = malloc(sizeof(joueur));
    J->points = 0;
    J-> meeples = 7;
    J-> id = id;

    return J;
}

void printside(char side, bool special, bool istemp, uint *nbpostmp){
	
	// routes  : blanc
	// près    : vert
	// ville   : rouge
	// abbaye  : cyan
	// blason  : jaune
	// village : violet
	
	if(istemp == 0){
	
	if(side == 'r'){
		printf("■");
		return;}
	
	if(side == 'p' && special != 1){
		printf("\033[0;32m■\033[0m");
		return;}
	
	if(side == 'V' && special != 1){
		printf("\033[0;31m■\033[0m");
		return;}
	
	if(side == 'p' && special == 1){
		printf("\033[0;36m■\033[0m");
		return;}
	
	if(side == 'V' && special == 1){
		printf("\033[0;33m■\033[0m");
		return;}
	
	if(side == 'v'){
		printf("\033[0;35m■\033[0m");
		return;}
	}
	
	if(nbpostmp == NULL){
	
	if(side == 'r'){
		printf("▢");
		return;}
	
	if(side == 'p' && special != 1){
		printf("\033[0;32m▢\033[0m");
		return;}
	
	if(side == 'V' && special != 1){
		printf("\033[0;31m▢\033[0m");
		return;}
	
	if(side == 'p' && special == 1){
		printf("\033[0;36m▢\033[0m");
		return;}
	
	if(side == 'V' && special == 1){
		printf("\033[0;33m▢\033[0m");
		return;}
	
	if(side == 'v'){
		printf("\033[0;35m▢\033[0m");
		return;}
	}
	
	if(nbpostmp != NULL){
	
	(*nbpostmp) ++;
	
	char c = (*nbpostmp) + 64;
	
	if(side == 'r'){
		printf("%c", c);
		return;}
	
	if(side == 'p' && special != 1){
		printf("\033[0;32m%c\033[0m", c);
		return;}
	
	if(side == 'V' && special != 1){
		printf("\033[0;31m%c\033[0m", c);
		return;}
	
	if(side == 'p' && special == 1){
		printf("\033[0;36m%c\033[0m", c);
		return;}
	
	if(side == 'V' && special == 1){
		printf("\033[0;33m%c\033[0m", c);
		return;}
	
	if(side == 'v'){
		printf("\033[0;35m%c\033[0m", c);
		return;}
	}
	
	printf("%c", side);
}

void print_grid_color(grid G){
    
    printf("\n\n\n");

    int i = 0, 
        j = 0, 
        firstcol = 0,
        lastcol  = N_CARDS*2 - 1,
        lenght   = N_CARDS*2 - 1,
        offset = 5;
	
	uint nbpostmp = 0;
    
    while(empty_grid_col(G, i) == 1){
        firstcol ++;
        i++;
    }
    
    i = lenght;
    
    while(empty_grid_col(G, i) == 1){
        lastcol--;
        i--;
    }

    if(lastcol < N_CARDS+offset)
        lastcol = N_CARDS+offset;

    if(firstcol > 0)
        firstcol -= 1;

    if(firstcol > N_CARDS-offset)
        firstcol = N_CARDS-offset;

    
    for(i = 0; i < lenght; i++){
        
        if((i < N_CARDS+offset && i > N_CARDS-offset) || empty_grid_row(G, i) == 0){
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
					printf("   ");
					printside(G[i][j]->sides[0], G[i][j]->special, G[i][j]->istemp, NULL);
					printf("  ");
				}
                else
                    printf("      ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
					printf(" ");
					printside(G[i][j]->sides[3], G[i][j]->special, G[i][j]->istemp, NULL);
					printf(" ");
					printside(G[i][j]->sides[4], G[i][j]->special, G[i][j]->istemp, &nbpostmp);
					printf(" ");
					printside(G[i][j]->sides[1], G[i][j]->special, G[i][j]->istemp, NULL);
				}
                else
                    printf("      ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
					printf("   ");
					printside(G[i][j]->sides[2], G[i][j]->special, G[i][j]->istemp, NULL);
					printf("  ");
				}
                else
                    printf("      ");
            }
            printf("\n");
        }    
    }
}

bool is_valid_pos(grid grid, tile* tile, coord coord){
    
    // retourne 0 si non valide, 1 si valide
    
    int x[4] = {coord.x,   coord.x+1, coord.x,   coord.x-1},
        y[4] = {coord.y-1, coord.y,   coord.y+1, coord.y},
        s1[4] = {0, 1, 2, 3},
        s2[4] = {2, 3, 0, 1},
        i = 0,
        imax = 4;
    
    if(coord.x == 0)
        imax = 3;
    
    if(coord.y == 0)
        i = 1;
    
    for(; i < imax; i++)
    {
        //printf("%d : %d\n", s2[i], s1[i]);
        
        if(grid[y[i]][x[i]] != NULL){
            
            //printf("%c : %c\n", grid[y[i]][x[i]]->sides[s2[i]], tile->sides[s1[i]]);
            
            if(grid[y[i]][x[i]]->sides[s2[i]] != tile->sides[s1[i]])
                return 0;
        }
    }
    
    return 1;
}

bool in_empty(grid grid, int i, int j){ // fonction pour gen_rand_valid_grill qui vérifie si une grilel est entourée de vide
    if(i >= 1)
        if(grid[i-1][j] != NULL)
            return 0;
    
    if(j >= 1)
        if(grid[i][j-1] != NULL)
            return 0;
        
    if(i < N_CARDS*2)
        if(grid[i+1][j] != NULL)
            return 0;
    
    if(j < N_CARDS*2)
        if(grid[i][j+1] != NULL)
            return 0;
        
    return 1;
}

void gen_rand_valid_grill(grid grid, tile **deck){
    coord c;
    int try,
        maxtry = 32000,
        k;
    srand(time(NULL));
   
    for(int i = 70; i < 80; i++){
        for(int j = 70; j < 80; j++) {
            
            do
                k = rand()%N_CARDS;
            while(deck[k] == NULL);
            
            c.x = j;
            c.y = i;
            try = 0;
            
            if((i == 70 && j == 70) || in_empty(grid, i, j) == 0)
            {
                while(try < maxtry && is_valid_pos(grid, deck[k], c) != 1)
                {
                    while(deck[k] == NULL)
                        k = rand()%N_CARDS;
                    
                    rotate_tile_clockwise(deck[k]);
                    
                    try ++;
                }
                
                if(try < maxtry)
                {
                    grid[i][j] = deck[k];
                    deck[k] = NULL;
                }
            }
        }
    }
}

void print_grid_with_pos(grid grid, coord* coo, tile* tile, uint size)
{
	tile->istemp = 1;
	
	int i;
	
	for(i = 0; i < size; i++)
		grid[coo[i].y][coo[i].x] = tile;
	
	print_grid_color(grid);
	
	for(i = 0; i < size; i++)
		grid[coo[i].y][coo[i].x] = NULL;
	
	tile->istemp = 0;
}

coord* get_all_available_positions(grid grid, uint* size)
{
    coord *coo = NULL;

    *size = 0;

    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
        {
           if(grid[i][j] == NULL && ( ( i < N_CARDS-1 && grid[i+1][j] != NULL ) ||
                                      ( j < N_CARDS-1 && grid[i][j+1] != NULL ) ||
                                      ( i > 0 && grid[i-1][j] != NULL ) ||
                                      ( j > 0 && grid[i][j-1] != NULL ) ))
            {
                (*size) ++;
                coo = realloc(coo, *size*sizeof(coo));
                coo[*size-1].x = j;
                coo[*size-1].y = i;
            }
        }

    return coo;
}

coord* get_all_possible_positions(grid grid, uint *size, tile* tile){ 
    
    uint sizetmp = 0, 
         i;
    
    coord *coo = get_all_available_positions(grid, &sizetmp);

    if(sizetmp == 0) {
        printf("Error : no empty position found for tile");
        return NULL;
    }

    uint idvalid[sizetmp];

    for(i = 0; i < sizetmp; i++){
        if(is_valid_pos(grid, tile, coo[i]) == 1)
        {
            idvalid[*size] = i;
            (*size) ++;
        }
    }

    coord *validcoo = malloc(*size * sizeof(coord));

    for(i = 0; i < *size; i++){
        validcoo[i] = coo[idvalid[i]];
    }

    free(coo);

    return validcoo;
}

int main() {
    srand(time(NULL));

    
    grid grid = Grid();


    //basic_print_grid(grid);

    tile** deck = readcsv("tuiles_base_simplifiees.csv");

    gen_rand_valid_grill(grid, deck);
    //print_grid_color(grid);

    /*
    for (int i = 0; i < csize; i++)
    {
        printf("\n(%d : %d)\n", coo[i].y, coo[i].x);
    }
    */

    int i=0, j=0;

    while(deck[i] == NULL)
    {
        i++;
    }

    system("clear");

    uint csize = 0;
    coord *coo = NULL;

    for(int k = 0; k < 4; k++)
    {
        csize = 0;
        coo = get_all_possible_positions(grid, &csize, deck[i]);
        
        print_grid_with_pos(grid, coo, deck[i], csize);

        free(coo);
        getchar();
        printf("\e[1;1H\e[2J");

        rotate_tile_clockwise(deck[i]);
    }

    
    joueur *joueur1 = Joueur(1);
    joueur *joueur2 = Joueur(2);

    free_grid(grid);
    free_deck(deck);


    free(joueur1);
    free(joueur2);
}
