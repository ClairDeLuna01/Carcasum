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
        T->claimed[i]  = 0;
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
        if(G[row][i] != NULL && G[row][i]->istemp == 0)
            return 0;
    
    return 1;
}

int empty_grid_col(grid G, int col){
    int i;
    
    //return 0;
    
    for(i = 0; i < N_CARDS*2; i++)
        if(G[i][col] != NULL && G[i][col]->istemp == 0)
            return 0;
    
    return 1;
}


void color_reset(){printf("\033[0m");}
void color_p(){printf("\033[48;5;2m");}
void color_a(){printf("\033[48;5;5m");}
void color_v(){printf("\033[48;5;102m");}
void color_V(){printf("\033[48;5;95m");}
void color_r(){printf("\033[48;5;250m");}

void icon_blue(){}
void icon_red (){}

void printside(char side, bool special, bool istemp, uint *nbpostmp, uint player){
	
    // routes  : blanc   ◼
	// près    : vert    ▦
	// ville   : rouge   ♜
	// abbaye  : violet  ✚
	// blason  : rouge   ⛨
	// village : rouge   ◼
	
    // ▣ ✠ ⇧ ✚ ⟰ ⛶ ▄▄▄

    if(istemp == 0)
    {
        if(side == 'r')
            color_r();
            
        if(side == 'p' && special != 1)
            color_p();

        if(side == 'p' && special == 1)
            color_a();
            
        if(side == 'V')
            color_V();
            
        if(side == 'v')
            color_v();
    }

    if(player > 0)
    {
        //♦ ●

        // couleurs : 
        // bleu   21
        // jaune  220 
        // rouge  124 / 9
        // orange 208
        // cyan 51 

        // bleu
        if(player == 1){
            printf("\033[38;5;21m ● \033[0m");
            return;}

        //rouge
        if(player == 2){
            printf("\033[38;5;196m ● \033[0m");
            return;}

        //jaune
        if(player == 3){
            printf("\033[38;5;220m ● \033[0m");
            return;}

        //cyan
        if(player == 4){
            printf("\033[38;5;51m ● \033[0m");
            return;}
        
        //orange
        if(player == 5){
            printf("\033[38;5;208m ● \033[0m");
            return;}
    }


	else if(istemp == 0)
    {
        if(side == 'r'){
            printf("   \033[0m");
            return;}
        
        if(side == 'p' && special != 1){
            printf(" · \033[0m");
            return;}

        if(side == 'V' && special != 1){
            printf(" ^ \033[0m");
            return;}
        
        if(side == 'p' && special == 1){
            printf(" ♱ \033[0m");
            return;}
        
        if(side == 'V' && special == 1){
            printf(" ⚑ \033[0m");
            return;}
        
        if(side == 'v'){
            printf(" ⌂ \033[0m");
            return;}
	}

	if(nbpostmp == NULL)
    {
        if(side == 'r'){
            printf(" ⛶ ");
            return;}
        
        if(side == 'p' && special != 1){
            printf("\033[0;32m ⛶ \033[0m");
            return;}
        
        if(side == 'V'){
            printf("\033[0;31m ⛶ \033[0m");
            return;}
        
        if(side == 'p' && special == 1){
            printf("\033[0;36m ⛶ \033[0m");
            return;}

        if(side == 'v'){
            printf("\033[0;31m ⛶ \033[0m");
            return;}
	}
	
	if(nbpostmp != NULL)
    {
        (*nbpostmp) ++;
        
        char c = (*nbpostmp) + 64;
        
        if(side == 'r'){
            printf(" %c ", c);
            return;}
        
        if(side == 'p' && special != 1){
            printf("\033[0;32m %c \033[0m", c);
            return;}
        
        if(side == 'V'){
            printf("\033[0;31m %c \033[0m", c);
            return;}
        
        if(side == 'p' && special == 1){
            printf("\033[0;36m %c \033[0m", c);
            return;}
        
        if(side == 'v'){
            printf("\033[0;35m %c \033[0m", c);
            return;}
	}
	
	printf("%c", side);
}

void print_grid_color(grid G){
    
    int i = 0, 
        j = 0, 
        firstcol = 0,
        lastcol  = N_CARDS*2 - 1,
        lenght   = N_CARDS*2 - 1,
        offset = 4;
	
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

    if(firstcol > N_CARDS-offset)
        firstcol = N_CARDS-offset;

    firstcol --;
    lastcol  ++;

    for(i = 0; i < lenght; i++){
        
        if((i < N_CARDS+offset && i > N_CARDS-offset) || 
           (i < lenght-1 && empty_grid_row(G, i+1) == 0))
            {
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
                    if(G[i][j]->sides[3] != 'r' && G[i][j]->sides[0] == G[i][j]->sides[3])
                        printside(G[i][j]->sides[3], 0, G[i][j]->istemp, NULL, 0);
                    else
                        printside('p', 0, G[i][j]->istemp, NULL, 0);

					printside(G[i][j]->sides[0], 0, G[i][j]->istemp, NULL, G[i][j]->meeple[0]);
					
                    if(G[i][j]->sides[0] != 'r' && G[i][j]->sides[0] == G[i][j]->sides[1])
                        printside(G[i][j]->sides[1], 0, G[i][j]->istemp, NULL, 0);
                    else
                        printside('p', 0, G[i][j]->istemp, NULL, 0);
				}
                else
                    printf("         ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
					printside(G[i][j]->sides[3], 0, G[i][j]->istemp, NULL, G[i][j]->meeple[3]);
					printside(G[i][j]->sides[4], G[i][j]->special, G[i][j]->istemp, &nbpostmp, G[i][j]->meeple[4]);
					printside(G[i][j]->sides[1], 0, G[i][j]->istemp, NULL, G[i][j]->meeple[1]);
				}
                else
                    printf("         ");
            }
            printf("\n");
            
            for(j = firstcol; j < lastcol+1; j++){
                if(G[i][j] != NULL){
                    if(G[i][j]->sides[2] != 'r' && G[i][j]->sides[2] == G[i][j]->sides[3])
                        printside(G[i][j]->sides[2], 0, G[i][j]->istemp, NULL, 0);
                    else
                        printside('p', 0, G[i][j]->istemp, NULL, 0);


					printside(G[i][j]->sides[2], 0, G[i][j]->istemp, NULL, G[i][j]->meeple[2]);
                    
                    if(G[i][j]->sides[2] != 'r' && G[i][j]->sides[2] == G[i][j]->sides[1])
                        printside(G[i][j]->sides[2], 0, G[i][j]->istemp, NULL, 0);
                    else
                        printside('p', 0, G[i][j]->istemp, NULL, 0);
                    //printf("  ");
				}
                else
                    printf("         ");
            }
            printf("\n");
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
           if(grid[i][j] == NULL && ( ( i < (2*N_CARDS)-1 && grid[i+1][j] != NULL ) ||
                                      ( j < (2*N_CARDS)-1 && grid[i][j+1] != NULL ) ||
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

void gen_rand_game()
{
    grid g = Grid();
    tile** deck = readcsv("tuiles_base_simplifiees.csv");
    coord *coo = NULL;

    uint csize = 0, 
         i = 1, 
         randid = 0,
         player = 0;

    joueur joueurs[5];
    for (int i = 0; i < 5; i++) {
        joueurs[i].id = i;
    }

    g[72][72] = deck[0];
    deck[0] = NULL;

    printf("\e[1;1H\e[2J");

    for(; i < 72; i++)
    {
        player = (i-1)%5 + 1;

        csize = 0;
        
        while(csize == 0)
        {
            rotate_tile_clockwise(deck[i]);
            coo = get_all_possible_positions(g, &csize, deck[i]);
        }
        
        print_grid_with_pos(g, coo, deck[i], csize);

        randid = rand()%csize;

        g[coo[randid].y][coo[randid].x] = deck[i];
        deck[i] = NULL;

        if(rand()%4 == 0) {
            int n = rand()%5;
            g[coo[randid].y][coo[randid].x]->meeple[n] = player;
            g[coo[randid].y][coo[randid].x]->claimed[n] = (uint)pow(2, player);
        }

        printf("\n\n\t===> Tuile n°%d", i);
        printf("\n\t===> Le joueur %d joue aléatoirement", player);
        printf("\n\t===> Entrer pour continuier");

        
        //getchar();
        printf("\e[1;1H\e[2J");

        print_grid_color(g);

        printf("\n\n\t===> Tuile n°%d", i);
        printf("\n\t===> Le joueur %d joue aléatoirement", player);
        
        bool road = false;
        for (int i = 0; i < 4; i++) {
            if (g[coo[randid].y][coo[randid].x]->sides[i] == 'r')
                road = true;
        }

        if (road) {
            int* points = check_road(g, coo[randid], joueurs);
            
            for (int i = 0; i < 5; i++) {
                if (points[i] > 0) {
                    printf("\n\tjoueur %d a gagner %d points", i, points[i]);
                    getchar();
                }
            }
            free(points);
        }

        printf("\n\t===> Entrer pour continuier");


        
        //getchar();
        printf("\e[1;1H\e[2J");
        free(coo);
    }

    print_grid_color(g);

    free_grid(g);
    free_deck(deck);
}


int rewind_path(uint from, coord to, grid g) {
    if (g[to.y][to.x] == NULL) {
        return -1;
    }
    if (g[to.y][to.x]->sides[4] != 'r') {
        g[to.y][to.x]->closed[from] = true;
        return 1;
    }

    if ((g[to.y][to.x]->sides[0] == 'r') && (from != 2)) {
        coord c;
        c.y = to.y-1;
        c.x = to.x;
        int v = rewind_path(0, c, g);
        if (v == -1)
            return -1;
        else {
            g[to.y][to.x]->closed[from] = true;
            g[to.y][to.x]->closed[0] = true;
            return v+1;
        }
    }
    
    if ((g[to.y][to.x]->sides[1] == 'r') && (from != 3)) {
        coord c;
        c.y = to.y;
        c.x = to.x+1;
        int v = rewind_path(1, c, g);
        if (v == -1)
            return -1;
        else {
            g[to.y][to.x]->closed[from] = true;
            g[to.y][to.x]->closed[1] = true;
            return v+1;
        }
    }
    if ((g[to.y][to.x]->sides[2] == 'r') && (from != 0)) {
        coord c;
        c.y = to.y+1;
        c.x = to.x;
        int v = rewind_path(2, c, g);
        if (v == -1)
            return -1;
        else {
            g[to.y][to.x]->closed[from] = true;
            g[to.y][to.x]->closed[2] = true;
            return v+1;
        }
    }
    if ((g[to.y][to.x]->sides[3] == 'r') && (from != 1)) {
        coord c;
        c.y = to.y;
        c.x = to.x-1;
        int v = rewind_path(3, c, g);
        if (v == -1)
            return -1;
        else {
            g[to.y][to.x]->closed[from] = true;
            g[to.y][to.x]->closed[3] = true;
            return v+1;
        }
    }

    return -1;
}


int* check_road(grid g, coord c, joueur *joueurs) {
    tile *new_tile = g[c.y][c.x]; 
    tile *old_tile;
    if (c.y-1 >= 0)
    if ((old_tile = g[c.y-1][c.x]) != NULL) 
    if ((old_tile->sides[2] == 'r') && (new_tile->sides[0] == 'r')) {
        if (old_tile->claimed[2] != 0) {
            new_tile->claimed[0] |= old_tile->claimed[2];
            if (new_tile->sides[4] == 'r') {
                new_tile->claimed[4] |= new_tile->claimed[0];
                if (new_tile->sides[1] == 'r')
                    new_tile->claimed[1] |= new_tile->claimed[4];
                if (new_tile->sides[2] == 'r')
                    new_tile->claimed[2] |= new_tile->claimed[4];
                if (new_tile->sides[3] == 'r')
                    new_tile->claimed[3] |= new_tile->claimed[4];
            }
        }
    };
    if (c.y+1 < 144)
    if ((old_tile = g[c.y+1][c.x]) != NULL) 
    if ((old_tile->sides[0] == 'r') && (new_tile->sides[2] == 'r')) {
        if (old_tile->claimed[0] != 0) {
            new_tile->claimed[2] |= old_tile->claimed[0];
            if (new_tile->sides[4] == 'r') {
                new_tile->claimed[4] |= new_tile->claimed[2];
                if (new_tile->sides[1] == 'r')
                    new_tile->claimed[1] |= new_tile->claimed[4];
                if (new_tile->sides[0] == 'r')
                    new_tile->claimed[0] |= new_tile->claimed[4];
                if (new_tile->sides[3] == 'r')
                    new_tile->claimed[3] |= new_tile->claimed[4];
            }
        }
    };
    if (c.x-1 >= 0)
    if ((old_tile = g[c.y][c.x-1]) != NULL) 
    if ((old_tile->sides[1] == 'r') && (new_tile->sides[3] == 'r')) {
        if (old_tile->claimed[1] != 0) {
            new_tile->claimed[3] = old_tile->claimed[1];
            if (new_tile->sides[4] == 'r') {
                new_tile->claimed[4] |= new_tile->claimed[3];
                if (new_tile->sides[1] == 'r')
                    new_tile->claimed[1] |= new_tile->claimed[4];
                if (new_tile->sides[2] == 'r')
                    new_tile->claimed[2] |= new_tile->claimed[4];
                if (new_tile->sides[0] == 'r')
                    new_tile->claimed[0] |= new_tile->claimed[4];
            }
        }
    };
    if (c.x+1 < 144)
    if ((old_tile = g[c.y][c.x+1]) != NULL) 
    if ((old_tile->sides[3] == 'r') && (new_tile->sides[1] == 'r')) {
        if (old_tile->claimed[3] != 0) {
            new_tile->claimed[1] = old_tile->claimed[3];
            if (new_tile->sides[4] == 'r') {
                new_tile->claimed[4] |= new_tile->claimed[1];
                if (new_tile->sides[0] == 'r')
                    new_tile->claimed[0] |= new_tile->claimed[4];
                if (new_tile->sides[2] == 'r')
                    new_tile->claimed[2] |= new_tile->claimed[4];
                if (new_tile->sides[3] == 'r')
                    new_tile->claimed[3] |= new_tile->claimed[4];
            }
        }
    };

    int* points = calloc(5, sizeof(int));
    int entry;
    if (new_tile->sides[0] == 'r')
        entry = 0;
    else if (new_tile->sides[1] == 'r')
        entry = 1;
    else if (new_tile->sides[2] == 'r')
        entry = 2;
    else if (new_tile->sides[3] == 'r')
        entry = 3;

    coord new_c;
    if (entry == 0) {
        new_c.y = c.y - 1;
        new_c.x = c.x;
    }
    else if (entry == 1) {
        new_c.y = c.y;
        new_c.x = c.x + 1;
    }
    else if (entry == 2) {
        new_c.y = c.y + 1;
        new_c.x = c.x;
    }
    else if (entry == 3) {
        new_c.y = c.y;
        new_c.x = c.x - 1;
    }
    
    int ret = rewind_path(2, new_c, g) + 1;
    if (ret != -1) {
        if (new_tile->claimed[entry] & 0b00001) {
            points[0] += ret;
        }
        if (new_tile->claimed[entry] & 0b00010) {
            points[1] += ret;
        }
        if (new_tile->claimed[entry] & 0b00100) {
            points[2] += ret;
        }
        if (new_tile->claimed[entry] & 0b01000) {
            points[3] += ret;
        }
        if (new_tile->claimed[entry] & 0b10000) {
            points[4] += ret;
        }
    }  

    if (new_tile->sides[4] == 'r') {
        int exit;
        if      (new_tile->sides[0] == 'r' && entry != 0) 
            exit = 0;
        else if (new_tile->sides[1] == 'r' && entry != 1) 
            exit = 1;
        else if (new_tile->sides[2] == 'r' && entry != 2) 
            exit = 2;
        else if (new_tile->sides[3] == 'r' && entry != 3) 
            exit = 3;




        if (exit == 0) {
            new_c.y = c.y - 1;
            new_c.x = c.x;
        }
        else if (exit == 1) {
            new_c.y = c.y;
            new_c.x = c.x + 1;
        }
        else if (exit == 2) {
            new_c.y = c.y + 1;
            new_c.x = c.x;
        }
        else if (exit == 3) {
            new_c.y = c.y;
            new_c.x = c.x - 1;
        }

        int ret = rewind_path(2, new_c, g);
        if (ret != -1) {
            if (new_tile->claimed[exit] & 0b00001) {
                points[0] += ret;
            }
            if (new_tile->claimed[exit] & 0b00010) {
                points[1] += ret;
            }
            if (new_tile->claimed[exit] & 0b00100) {
                points[2] += ret;
            }
            if (new_tile->claimed[exit] & 0b01000) {
                points[3] += ret;
            }
            if (new_tile->claimed[exit] & 0b10000) {
                points[4] += ret;
            }
        }

        for (int i = 0; i < 5; i++)
            joueurs[i].points += points[i];

    }
    return points;
}


int main() {
    srand(3); // bug

    
    gen_rand_game();


}
