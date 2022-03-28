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

bool is_struct_closed(grid G, int i, int j, uint *score, char c, int side)
{
    if(G[i][j] == NULL || G[i][j]->closed[side] == 1 ||
      ((side == 0 || G[i][j]->sides[4] == c) && G[i][j]->sides[0] == c && G[i-1][j] == NULL)||
      ((side == 2 || G[i][j]->sides[4] == c) && G[i][j]->sides[2] == c && G[i+1][j] == NULL)||
      ((side == 3 || G[i][j]->sides[4] == c) && G[i][j]->sides[3] == c && G[i][j-1] == NULL)||
      ((side == 1 || G[i][j]->sides[4] == c) && G[i][j]->sides[1] == c && G[i][j+1] == NULL))
        return 0;

    if(G[i][j]->istemp == 1)
        return 1;
    
    if(c == 'V')
    {
        (*score) += 2;
        if(G[i][j]->special == 1)
            (*score) += 2;
    }

    if(c == 'r')
        (*score) ++;

    G[i][j]->istemp = 1;

    if(G[i][j]->sides[4] != c)
    {
        switch(side)
        {
            case 0 :
                return is_struct_closed(G, i-1, j, score, c, 2);;
            case 1 :
                return is_struct_closed(G, i, j+1, score, c, 3);
            case 2 :
                return is_struct_closed(G, i+1, j, score, c, 0);
            case 3 :
                return is_struct_closed(G, i, j-1, score, c, 1);
        }
    }

    bool itc = 1;

    if(G[i][j]->sides[0] == c)
        itc = itc && is_struct_closed(G, i-1, j, score, c, 2);
        
    if(G[i][j]->sides[2] == c)
        itc = itc &&  is_struct_closed(G, i+1, j, score, c, 0);
        
    if(G[i][j]->sides[3] == c)
         itc = itc && is_struct_closed(G, i, j-1, score, c, 1);

    if(G[i][j]->sides[1] == c)
        itc = itc &&  is_struct_closed(G, i, j+1, score, c, 3);

    return itc;
}

void clear_tmpmark(grid G)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(G[i][j] != NULL)
                G[i][j]->istemp = 0;
}

void close_stuct(grid G, int i, int j, uint score, char c, int side, joueur* p[5], bool winners[5])
{
    if(G[i][j] == NULL){
        printf("\nError : close_struct function can't close open srtuct at position %d : %d in type %c\n", i, j, c);
        return;}

    if(G[i][j]->sides[side] != c){
        printf("\nError : close_struct function can't close stuct with wrong type at position %d : %d\n\t\tExpected type %c but got %c\n", i, j, c, G[i][j]->sides[side]);
        return;}

    if(G[i][j]->closed[side] == 1)
        return;
    
    for(int k = 0; k < 5; k++)
        if(k == side || (G[i][j]->sides[4] == c && G[i][j]->sides[k] == c))
        {
            G[i][j]->closed[k] = 1;

            if(G[i][j]->meeple[k] != 0)
            {
                if(winners[G[i][j]->meeple[k]-1] == 0)
                {
                    winners[G[i][j]->meeple[k]-1] = 1;
                    p[G[i][j]->meeple[k]-1]->points += score;
                }
                
                p[G[i][j]->meeple[k]-1]->meeples ++;
                G[i][j]->meeple[k] = 0;            
            }
            
            switch(k)
            {
                case 0 :
                    close_stuct(G, i-1, j, score, c, 2, p, winners);
                    break;
                case 1 :
                    close_stuct(G, i, j+1, score, c, 3, p, winners);
                    break;
                case 2 :
                    close_stuct(G, i+1, j, score, c, 0, p, winners);
                    break;
                case 3 :
                    close_stuct(G, i, j-1, score, c, 1, p, winners);
                    break;   
            }
        }
}

int main() {
    srand(1); // bug

    
    gen_rand_game();


}
