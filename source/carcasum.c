#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "carcasum.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <limits.h>


short secret  = 0;
uint AI_frame_wait = 0;

typedef tile*** grid;

uint get_line_number(char* filename) { // gets the number of non empty lines in file
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error while opening file");
        exit(1);
    }
    uint n = 0;
    char str[100];

    while (!feof(fp)) {
        if (fscanf(fp, "%[^\n]%*c", str) == 1) {
            n++;
        }
    }
    fclose(fp);
    return n;
}

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

void rotate_tile_clockwise(tile* t)
{
    char tmp = t->sides[0];
    t->sides[0] = t->sides[3];
    t->sides[3] = t->sides[2];
    t->sides[2] = t->sides[1];
    t->sides[1] = tmp;
} 

void rotate_tile_counterclockwise(tile* t)
{
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

void free_grid(grid g)
{
    for(int i = 0; i < N_CARDS*2; i++)
        free(g[i]);
    
    free(g);
}

void lowercase(char* str)
{
    for (int i = 0; i < strlen(str); i++)
        str[i] = tolower(str[i]);
}

tile** readcsv(char* filename)
{
    tile **deck = malloc(sizeof(tile *) * N_CARDS);
    if (!deck) {
        perror("Error while allocating deck");
        exit(1);
    }
    for (int i = 0; i <= N_CARDS; i++)
        deck[i] = Tile();

    int i = 0, j = 0;
    FILE *fp;
    char str[8+1];

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error opening csv tile list file!\n");
        fprintf(stderr, "Make sure you have the file in the same directory as the executable!\n");
        fprintf(stderr, "The filename must be \"tuiles_base_simplifiees.csv\"!\n");
        exit(1);
    }

    while (!feof(fp)) {
        if (j == N_CARDS) {
            break;
        }

        if (fscanf(fp, "%[^,\n]%*c", str) != 1) {
            fprintf(stderr, "Error while reading csv tile list file!\n");
            exit(1);
        }



        lowercase(str); // convert to lowercase

        if (strcmp(str, "pre") == 0) {
            deck[j]->sides[i] = 'p';
        }
        else if (strcmp(str, "route") == 0) {
            deck[j]->sides[i] = 'r';
        }
        else if (strcmp(str, "ville") == 0) {
            deck[j]->sides[i] = 'V';
        }
        else if (strcmp(str, "village") == 0) {
            deck[j]->sides[i] = 'v';
        }
        else if (strcmp(str, "blason") == 0) {
            deck[j]->sides[i] = 'V';
            deck[j]->special = true;
        }
        else if (strcmp(str, "abbaye") == 0) {
            deck[j]->sides[i] = 'p';
            deck[j]->special = true;
        }

        i++;
        if (i == 5) {
            i = 0;
            j++;
        }

    }

    fclose(fp);

    

    deck_shuffle(deck);
    return deck;
}

void free_deck(tile** deck)
{
    if(deck == NULL)
        return;

    for (int i = 0; i <= N_CARDS; i++)
    {
        if(deck[i] != NULL)
            free(deck[i]);
    }

    // free(*deck);
}

joueur* Joueur(uint id)
{
    joueur *J = malloc(sizeof(joueur));
    J->points = 0;
    J->meeples = 7;
    J->id = id;
    J->is_ai = false;

    return J;
}

joueur* Bot(uint id)
{
    joueur *J = malloc(sizeof(joueur));
    J->points = 0;
    J->meeples = 7;
    J->id = id;
    J->is_ai = true;

    return J;
}

bool is_valid_pos(grid grid, tile* tile, coord coord)
{
    
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
        if(grid[y[i]][x[i]] != NULL)
        {    
            if(grid[y[i]][x[i]]->sides[s2[i]] != tile->sides[s1[i]])
                return 0;
        }
    }
    
    return 1;
}

void deck_shuffle(tile** deck)
{
    for (int i = 0; i < 100; i++) {
        int a = rand() % (N_CARDS-1) + 1; // -1 + 1 cause we dont want to shuffle the first tile
        int b = rand() % (N_CARDS-1) + 1;
        tile* tmp = deck[a];
        deck[a] = deck[b];
        deck[b] = tmp;
    }
}

coord* get_all_available_positions(grid grid, uint* size)
{
    coord *coo = malloc(0);
    if (coo == NULL) {
        perror("malloc error in get_all_available_positions");
        exit(1);
    }

    *size = 0;

    for(int i = 1; i < N_CARDS*2 - 1; i++)
        for(int j = 1; j < N_CARDS*2 - 1; j++)
        {
           if(
               grid[i][j] == NULL 
               && ( 
                    ( i < (2*N_CARDS)-1 && grid[i+1][j] != NULL ) ||
                    ( j < (2*N_CARDS)-1 && grid[i][j+1] != NULL ) ||
                    ( i > 0             && grid[i-1][j] != NULL ) ||
                    ( j > 0             && grid[i][j-1] != NULL ) 
                )
            )
            {
                (*size) ++;
                coo = realloc(coo, *size*sizeof(coo));
                if (coo == NULL) {
                    perror("realloc error in get_all_available_positions");
                    exit(1);
                }
                coo[*size-1].x = j;
                coo[*size-1].y = i;
            }
        }

    return coo;
}

coord* get_all_possible_positions(grid g, tile* t, uint* size)
{
    // returns all the positions for the tile t that are both available and valid
    // the size of the array is returned in size
    uint final_size = 0;

    coord* all_pos = get_all_available_positions(g, size);
    bool *valid_pos = malloc(*size * sizeof(bool));

    for(int i = 0; i < *size; i++)
    {
        if(is_valid_pos(g, t, all_pos[i]))
        {
            final_size ++;
            valid_pos[i] = 1;
        }
        else
            valid_pos[i] = 0;
    }

    coord* final_pos = malloc(final_size*sizeof(coord));

    int i = 0;
    for(int j = 0; j < *size; j++)
    {
        if(valid_pos[j])
        {
            final_pos[i] = all_pos[j];
            i++;
        }
    }

    *size = final_size;

    free(valid_pos);
    free(all_pos);

    return final_pos;
}

void place_temp_tile(grid grid, tile* tile)
{ 
    
    tile->istemp = 1;

    uint sizetmp = 0, 
         i,
         size = 0;
    
    coord *coo = get_all_available_positions(grid, &sizetmp);

    if(sizetmp == 0)
    {
        grid[N_CARDS][N_CARDS] = tile;
        return;
    }

    uint *idvalid = malloc(sizeof(uint) * sizetmp);

    for(i = 0; i < sizetmp; i++){
        if(is_valid_pos(grid, tile, coo[i]) == 1)
        {
            idvalid[size] = i;
            size ++;
        }
    }

    for(i = 0; i < size; i++)
        grid[ coo[idvalid[i]].y ][ coo[idvalid[i]].x ] = tile;

    free(idvalid);
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
                return is_struct_closed(G, i-1, j, score, c, 2);
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

bool is_struct_unoccupied(grid G, int i, int j, uint* score, char c, int side, bool* playerID, bool cities_count_double, bool close_struct)
{
    // 0 : la struct est occupée, on ne peu donc pas y placer un meeple
    // 1 : la struct n'est pas occupée, on peu donc y placer un meeple
    if(G[i][j] == NULL || G[i][j]->istemp == 1 || G[i][j]->closed[side] == 1)
        return 1;

    if(score != NULL && playerID != NULL && close_struct)
    {
        if(G[i][j]->sides[4] != c)
            G[i][j]->closed[side] = 1;
        else
            for(int k = 0; k < 4; k ++)
                if(G[i][j]->sides[k] == c)
                    G[i][j]->closed[k] = 1;
    }

    for(int k = 0; k < 5; k ++)
        if(((G[i][j]->sides[4] == c && G[i][j]->sides[k] == c) || k == side) && G[i][j]->meeple[k] > 0 
            && G[i][j]->meeple[k] < 6)
        {
            if(playerID != NULL && score != NULL)
                playerID[G[i][j]->meeple[k]-1] = 1;
            else
                return 0;
        }

    G[i][j]->istemp = 1;

    if(score != NULL)
    {
        if (cities_count_double && c == 'V') {
            (*score) += 2;
        } else {
            (*score) ++;
        }
        if(c == 'V' && G[i][j]->sides[4] == c && G[i][j]->special == 1) {
            if (cities_count_double && c == 'V') {
                (*score) += 2;
            } else {
                (*score) ++;
            }
        }
    }

    if(G[i][j]->sides[4] != c)
    {
        switch(side)
        {
            case 0 :
                return is_struct_unoccupied(G, i-1, j, score, c, 2, playerID, cities_count_double, close_struct);
            case 1 :
                return is_struct_unoccupied(G, i, j+1, score, c, 3, playerID, cities_count_double, close_struct);
            case 2 :
                return is_struct_unoccupied(G, i+1, j, score, c, 0, playerID, cities_count_double, close_struct);
            case 3 :
                return is_struct_unoccupied(G, i, j-1, score, c, 1, playerID, cities_count_double, close_struct);
        }
    }

    bool itc = 1;

    if(G[i][j]->sides[0] == c)
        itc = itc && is_struct_unoccupied(G, i-1, j, score, c, 2, playerID, cities_count_double, close_struct);
        
    if(G[i][j]->sides[2] == c)
        itc = itc && is_struct_unoccupied(G, i+1, j, score, c, 0, playerID, cities_count_double, close_struct);
        
    if(G[i][j]->sides[3] == c)
        itc = itc && is_struct_unoccupied(G, i, j-1, score, c, 1, playerID, cities_count_double, close_struct);

    if(G[i][j]->sides[1] == c)
        itc = itc && is_struct_unoccupied(G, i, j+1, score, c, 3, playerID, cities_count_double, close_struct);

    return itc;
}

void clear_tmpmark(grid G)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(G[i][j] != NULL)
                G[i][j]->istemp = 0;
}

void empty_tmpmark(grid G)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(G[i][j] != NULL && G[i][j]->istemp == 1)
                G[i][j] = NULL;
}

void clear_HL_TempMepple(grid G)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(G[i][j] != NULL)
                for(int k = 0; k < 5; k++)
                    if(G[i][j]->meeple[k] == 7)
                    {
                        G[i][j]->meeple[k] = 6;
                        return;                        
                    }

}

void clear_ALL_TempMeeple(grid G)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(G[i][j] != NULL)
                for(int k = 0; k < 5; k++)
                    if(G[i][j]->meeple[k] > 5)
                        G[i][j]->meeple[k] = 0;

}

void close_stuct(grid G, int i, int j, uint score, char c, int side, joueur* p[5], uint winners[5])
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
                // if(winners[G[i][j]->meeple[k]-1] == 0)
                //     p[G[i][j]->meeple[k]-1]->points += score;
                
                winners[G[i][j]->meeple[k]-1] ++;

                (p[G[i][j]->meeple[k]-1]->meeples) ++;
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

bool is_abbey_complete(grid G, int i, int j)
{
    for(int k = -1; k <= 1; k++)
        for(int l = -1; l <= 1; l++)
            if((G[i+k][j+l] == NULL) || (G[i+k][j+l] != NULL && G[i+k][j+l]->istemp == 1))
                return 0;
    return 1;
}

grid gen_rand_grid(tile *** pdeck)
{
    *pdeck = readcsv("tuiles_base_simplifiees.csv");
    tile** deck = *pdeck;
    grid grid = Grid();
    coord *coo = NULL;

    uint csize = 0, 
         i = 1, 
         randid = 0,
         randi;

    for(int i = 0; i < N_CARDS; i++)
    {
        randi = rand()%3;
        for(int j = 0; j < randi; j++)
            rotate_tile_clockwise(deck[i]);
    }

    grid[N_CARDS][N_CARDS] = deck[0];

    for(; i < 600; i++)
    {
        randi = rand()%(N_CARDS-1);

        csize = 0;
        
        coo = get_all_possible_positions(grid, deck[randi], &csize);

        if(csize > 0)
        {
            randid = rand()%csize;

            grid[coo[randid].y][coo[randid].x] = deck[randi];
        }
        
        free(coo);
    }

    for(int i = 1; i < round(N_CARDS*1.20); i++)
        for(int j = 1; j < round(N_CARDS*1.20); j++)
        {
            if(grid[i][j] == NULL && (grid[i-1][j] != NULL || grid[i][j-1] != NULL || grid[i+1][j] != NULL || grid[i][j+1] != NULL ))
            {
                grid[i][j] = Tile();

                if(grid[i-1][j] != NULL)
                    grid[i][j]->sides[0] = grid[i-1][j]->sides[2];
                else 
                    grid[i][j]->sides[0] = 'p';
                
                if(grid[i+1][j] != NULL)
                    grid[i][j]->sides[2] = grid[i+1][j]->sides[0];
                else 
                    grid[i][j]->sides[4] = 'p';

                if(grid[i][j-1] != NULL)
                    grid[i][j]->sides[3] = grid[i][j-1]->sides[1];
                else 
                    grid[i][j]->sides[3] = 'p';

                if(grid[i][j+1] != NULL)
                    grid[i][j]->sides[1] = grid[i][j+1]->sides[3];
                else 
                    grid[i][j]->sides[1] = 'p';

                if(grid[i][j]->sides[1] == 'V' || grid[i][j]->sides[0] == 'V' || grid[i][j]->sides[2] == 'V' || grid[i][j]->sides[3] == 'V')
                    grid[i][j]->sides[4] = 'V';
                else if(grid[i][j]->sides[0] != 'r' && grid[i][j]->sides[1] != 'r' && grid[i][j]->sides[2] != 'r' && grid[i][j]->sides[3] != 'r')
                    grid[i][j]->sides[4] = 'p';
                else
                    grid[i][j]->sides[4] = 'v';
            }
        }
    return grid;
}

void free_main_menu_grid(grid g, tile **deck)
{
    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
            if(g[i][j] != NULL)
            {
                bool is_outside_deck = 1;

                for(int k = 0; k < N_CARDS; k++)
                    if(g[i][j] == deck[k])
                        is_outside_deck = 0;
                
                if(is_outside_deck == 1)
                    free(g[i][j]);
            }
}

void sdlCustom_FindPlayerColor(SDL_Color *color, uint playerID)
{
    switch(playerID)
    {
        case 1 :
            color->r = 224;
            color->g = 71;
            color->b = 71;
            break;

        case 2 : 
            color->r = 57;
            color->g = 116;
            color->b = 197;
            break;

        case 3 :
            color->r = 232;
            color->g = 204;
            color->b = 52;
            break;

        case 4 :
            color->r = 54;
            color->g = 195;
            color->b = 61;
            break;

        case 5 :
            color->r = 195;
            color->g = 63;
            color->b = 153;
            break;

        default:
            break;
    }
}

void sdlCustom_LoadGameSounds(RenderVar *RV)
{
    RV->Game_Sounds.BackgroundMusic = Mix_LoadMUS("ressources/music/BackgroundMusic.mp3");
    RV->Game_Sounds.place_tile      = Mix_LoadWAV("ressources/music/place_tile.ogg");
    RV->Game_Sounds.place_meeple    = Mix_LoadWAV("ressources/music/place_meeple.ogg");
    RV->Game_Sounds.main_menu_numb  = Mix_LoadWAV("ressources/music/main_menu_numb.ogg");
    RV->Game_Sounds.end_game        = Mix_LoadWAV("ressources/music/end_game.wav");
    RV->Game_Sounds.complete_struct = Mix_LoadWAV("ressources/music/complete_struct.wav"); 
}

void sdlCustom_LoadMeepleCounterTexture(RenderVar *RV, EventVar *EV, int playerID)
{
    if(*((&RV->Texture_UI.Player1_Meeples)+playerID-1) != NULL)
        SDL_DestroyTexture(*((&RV->Texture_UI.Player1_Meeples)+playerID-1));

    char meeple[2] = "0";
    meeple[0] = EV->player_list[playerID-1]->meeples+48;
    SDL_Color color = {0, 0, 0, 255};
    sdlCustom_FindPlayerColor(&color, playerID);
    SDL_Surface *surface = TTF_RenderUTF8_Solid(RV->font, meeple, color);

    *((&RV->Texture_UI.Player1_Meeples)+playerID-1) = SDL_CreateTextureFromSurface(RV->renderer, surface);

    SDL_FreeSurface(surface);
}

void sdlCustom_LoadPointsCounterTexture(RenderVar *RV, EventVar *EV, int playerID)
{
    SDL_DestroyTexture(*((&RV->Texture_UI.Player1_Points)+playerID-1));
    char score[4] = "000";
    score[0] = ((EV->player_list[playerID-1]->points - EV->player_list[playerID-1]->points%100)/100)+48;
    score[1] = ((EV->player_list[playerID-1]->points - EV->player_list[playerID-1]->points%10)/10)+48;
    score[2] = (EV->player_list[playerID-1]->points%10)+48;
    SDL_Color color = {0, 0, 0, 255};
    sdlCustom_FindPlayerColor(&color, playerID);
    SDL_Surface *surface = TTF_RenderUTF8_Solid(RV->font, score, color);

    *((&RV->Texture_UI.Player1_Points)+playerID-1) = SDL_CreateTextureFromSurface(RV->renderer, surface);

    SDL_FreeSurface(surface);
}

void sdlCustom_LoadTextureUI(SDL_Renderer* renderer, Texture_UI *T)
{
    T->PlayersInfo = IMG_LoadTexture(renderer, "ressources/ui/PlayersInfo.png");
    T->PlayersTurn = IMG_LoadTexture(renderer, "ressources/ui/PlayersTurn.png");
    T->no_position = IMG_LoadTexture(renderer, "ressources/ui/no_position.png");
    T->cat         = IMG_LoadTexture(renderer, "ressources/secret/DoNotPass/LockedArea/NotHere/No/Stop/GoBackNow/cat.png");
}

void sdlCustom_LoadMainMenuTextures(SDL_Renderer* renderer, Texture_MainMenu *T)
{
    T->background = IMG_LoadTexture(renderer, "ressources/menu/background.png");
}

void sdlCustom_LoadWinMessageTexture(RenderVar *RV, uint playerID)
{
    if(RV->Texture_UI.win_message != NULL)
        SDL_DestroyTexture(RV->Texture_UI.win_message);

    SDL_Color color = {255, 255, 255, 255};
    sdlCustom_FindPlayerColor(&color, playerID);
    char str1[128] = "Player . win the game!";
    char str2[128] = "No one win the game!";

    char *str = NULL;

    if(playerID != 0)
    {
        str = str1;
        str[7] = playerID+48;
    }
    else
        str = str2;

    SDL_Surface *surface = TTF_RenderUTF8_Solid(RV->font, str, color);

    RV->Texture_UI.win_message = SDL_CreateTextureFromSurface(RV->renderer, surface);

    SDL_FreeSurface(surface);
}

void sdlCustom_LoadEventLogTexture(RenderVar *RV, EventLog *EL)
{
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface *surface = NULL;

    char str1[128] = "Player   get    point(s) for an abbey";
    char str2[128] = "Player   has completed an abbey and gets back   meeple";
    char str3[128] = "Player   has closed a city with    points and gets back   meeple";
    char str4[128] = "Player   has closed a road with    points and gets back   meeple";
    char str5[128] = "Player   has closed a city and gets back    meeple";
    char str6[128] = "Player   has closed a road and gets back    meeple";
    char str7[128] = "Player   get    point(s) for all the incomplete(s) structure(s)";

    switch (EL->EventID)
    {
        case GameEvent_Points_abbey :
            surface = TTF_RenderUTF8_Solid(RV->font, str1, color);
            break;

        case GameEvent_Complete_abbey:
            surface = TTF_RenderUTF8_Solid(RV->font, str2, color);
            break;
        
        case GameEvent_Complete_city :
            surface = TTF_RenderUTF8_Solid(RV->font, str3, color);
            break;

        case GameEvent_Complete_roads :
            surface = TTF_RenderUTF8_Solid(RV->font, str4, color);
            break;
        
        case GameEvent_MeepleBack_city :
            surface = TTF_RenderUTF8_Solid(RV->font, str5, color);
            break;

        case GameEvent_MeepleBack_roads :
            surface = TTF_RenderUTF8_Solid(RV->font, str6, color);
            break;

        case GameEvent_Points_IncompleteStruct :
            surface = TTF_RenderUTF8_Solid(RV->font, str7, color);
            break;

        default:
            break;
    }

    EL->TextureText = SDL_CreateTextureFromSurface(RV->renderer, surface);
    
    sdlCustom_FindPlayerColor(&color, EL->playerID);

    switch (EL->EventID)
    {
        case GameEvent_Points_abbey :
            str1[7] = EL->playerID+48;
            if(EL->score < 10)
                str1[13] = EL->score+48;
            else
            {
                str1[13] = ((EL->score - EL->score%10)/10)+48;
                str1[14] = (EL->score%10)+48;
            }
            
            surface = TTF_RenderUTF8_Solid(RV->font, str1, color);
            break;

        case GameEvent_Complete_abbey:
            str2[7] = EL->playerID+48;
            str2[46] = EL->meeple+48;
            surface = TTF_RenderUTF8_Solid(RV->font, str2, color);
            break;
        
        case GameEvent_Complete_city :
            str3[7] = EL->playerID+48;
            if(EL->score < 10)
                str3[32] = (EL->score%10)+48;
            else
            {
                str3[32] = ((EL->score - EL->score%10)/10)+48;
                str3[33] = (EL->score%10)+48;
            }

            str3[56] = EL->meeple+48;
            surface = TTF_RenderUTF8_Solid(RV->font, str3, color);
            break;

        case GameEvent_Complete_roads :
            str4[7] = EL->playerID+48;
            if(EL->score < 10)
                str4[32] = EL->score+48;
            else
            {
                str4[32] = ((EL->score - EL->score%10)/10)+48;
                str4[33] = (EL->score%10)+48;
            }

            str4[56] = EL->meeple+48;
            surface = TTF_RenderUTF8_Solid(RV->font, str4, color);
            break;

        case GameEvent_MeepleBack_city:
            str5[7] = EL->playerID+48;
            str5[41] = EL->meeple+48;
            surface = TTF_RenderUTF8_Solid(RV->font, str5, color);
            break;
        
        case GameEvent_MeepleBack_roads :
            str6[7] = EL->playerID+48;
            str6[41] = EL->meeple+48;
            surface = TTF_RenderUTF8_Solid(RV->font, str6, color);
            break;

        case GameEvent_Points_IncompleteStruct:
            str7[7] = EL->playerID+48;
            if(EL->score < 10)
                str7[13] = EL->score+48;
            else
            {
                str7[13] = ((EL->score - EL->score%10)/10)+48;
                str7[14] = (EL->score%10)+48;
            }
            surface = TTF_RenderUTF8_Solid(RV->font, str7, color);
            break;

        default:
            break;
    }

    EL->TextureNumb = SDL_CreateTextureFromSurface(RV->renderer, surface);
    SDL_FreeSurface(surface);
} 

void sdlCustom_EventLog_ADD(RenderVar *RV, EventLog **EL, uint EventID, uint playerID, uint score, uint meeple)
{
    if(EventID == GameEvent_Complete_abbey || EventID == GameEvent_Complete_roads || EventID == GameEvent_Complete_city)
        Mix_PlayChannel(-1, RV->Game_Sounds.complete_struct, 0);

    EventLog *NewEL = malloc(sizeof(EventLog));

    NewEL->EventID = EventID;
    NewEL->playerID = playerID;
    NewEL->score = score;
    NewEL->next = *EL;
    NewEL->Time_On_Screen = RV->FPS_Lock*(TimeBeforeEventLogDisapear/1000);
    NewEL->meeple = meeple;

    sdlCustom_LoadEventLogTexture(RV, NewEL);

    if(*EL != NULL)
        (*EL)->Time_On_Screen += (RV->FPS_Lock/2)*(TimeBeforeEventLogDisapear/1000);

    *EL = NewEL;
}

void sdlCustom_EventLog_Free(EventLog *EL)
{
    if(EL->TextureText != NULL)
        SDL_DestroyTexture(EL->TextureText);

    if(EL->TextureNumb != NULL)
        SDL_DestroyTexture(EL->TextureNumb);

    free(EL);
}

void sdlCustom_EventLog_free_All(EventLog **EL)
{
    EventLog *tmp;

    while(*EL != NULL)
    {
        tmp = (*EL)->next;
        sdlCustom_EventLog_Free(*EL);
        *EL = tmp;
    }
}

void sdlCustom_EventLog_freeLast(EventLog **EL)
{
    if(*EL == NULL)
        return;
    
    EventLog *last = *EL;

    while(last->next != NULL)
        last = last->next;

    if(last->Time_On_Screen == 0)
    {
        sdlCustom_EventLog_Free(last);
        
        if(last == *EL)
        {
            *EL = NULL;
            return;
        }

        EventLog *newlast = *EL;

        while(newlast->next != last)
            newlast = newlast->next;
        
        newlast->next = NULL;
    }
}

void sdlCustom_LoadTilesTextures(SDL_Renderer* renderer, Texture_Tiles *T)
{
    *((&T->abbey)+0)  = IMG_LoadTexture(renderer, "ressources/tiles/abbey.png");
    *((&T->abbey)+1)  = IMG_LoadTexture(renderer, "ressources/tiles/blazon.png");
    *((&T->abbey)+2)  = IMG_LoadTexture(renderer, "ressources/tiles/prairies.png");
    *((&T->abbey)+3)  = IMG_LoadTexture(renderer, "ressources/tiles/roads.png");
    *((&T->abbey)+4)  = IMG_LoadTexture(renderer, "ressources/tiles/town.png");
    *((&T->abbey)+5)  = IMG_LoadTexture(renderer, "ressources/tiles/city.png");
    *((&T->abbey)+6)  = IMG_LoadTexture(renderer, "ressources/tiles/roads_perpendicular.png");
    *((&T->abbey)+7)  = IMG_LoadTexture(renderer, "ressources/tiles/wall.png");
    *((&T->abbey)+8)  = IMG_LoadTexture(renderer, "ressources/tiles/wall_gate.png");
    *((&T->abbey)+9)  = IMG_LoadTexture(renderer, "ressources/tiles/wall_diagonal_left.png");
    *((&T->abbey)+10) = IMG_LoadTexture(renderer, "ressources/tiles/wall_diagonal_right.png");
    *((&T->abbey)+11) = IMG_LoadTexture(renderer, "ressources/tiles/alt_wall.png");
    *((&T->abbey)+12) = IMG_LoadTexture(renderer, "ressources/tiles/patch_wall.png");
    *((&T->abbey)+13) = IMG_LoadTexture(renderer, "ressources/tiles/alt_wall_gate.png");
    *((&T->abbey)+14) = IMG_LoadTexture(renderer, "ressources/tiles/roads_intersection.png");
    *((&T->abbey)+15) = IMG_LoadTexture(renderer, "ressources/tiles/blazon_castle.png");
    *((&T->abbey)+16) = IMG_LoadTexture(renderer, "ressources/tiles/p1.png");
    *((&T->abbey)+17) = IMG_LoadTexture(renderer, "ressources/tiles/p2.png");
    *((&T->abbey)+18) = IMG_LoadTexture(renderer, "ressources/tiles/p3.png");
    *((&T->abbey)+19) = IMG_LoadTexture(renderer, "ressources/tiles/p4.png");
    *((&T->abbey)+20) = IMG_LoadTexture(renderer, "ressources/tiles/p5.png");
    *((&T->abbey)+21) = IMG_LoadTexture(renderer, "ressources/tiles/p_select.png");
    *((&T->abbey)+22) = IMG_LoadTexture(renderer, "ressources/tiles/p_highlight.png");
    *((&T->abbey)+23) = IMG_LoadTexture(renderer, "ressources/tiles/tempmark.png");
    *((&T->abbey)+24) = IMG_LoadTexture(renderer, "ressources/tiles/tempmark_highlight.png");
    *((&T->abbey)+25) = IMG_LoadTexture(renderer, "ressources/tiles/empty.png");
}

void sdlCustom_UnloadSounds(Game_Sounds *GS)
{
    if(GS->BackgroundMusic != NULL)
        Mix_FreeMusic(GS->BackgroundMusic);

    for(int i = 0; i < (sizeof(Game_Sounds) - sizeof(Mix_Music*))/sizeof(Mix_Chunk*); i++)
        if(*(&(GS->place_tile)+i) == NULL)
        Mix_FreeChunk(*(&(GS->place_tile)+i));
}

void sdlCustom_UnloadTilesTexture(Texture_Tiles *T)
{
    for(int i = 0; i < sizeof(Texture_Tiles)/sizeof(SDL_Texture*); i++)
        if(*(&(T->abbey)+i) != NULL)
            SDL_DestroyTexture(*(&(T->abbey)+i));
}

void sdlCustom_UnloadUITextures(Texture_UI *T)
{
    if(T != NULL)
        for(int i = 0; i < sizeof(Texture_UI)/sizeof(SDL_Texture*); i++)
            if(*(&(T->PlayersInfo)+i) != NULL)
                SDL_DestroyTexture(*(&(T->PlayersInfo)+i));
}

void sdlCustom_UnloadMainMenuTextures(Texture_MainMenu *T)
{
    if(T == NULL)
        return;
    
    for(int i = 0; i < sizeof(Texture_MainMenu)/sizeof(SDL_Texture*); i++)
        if(*(&(T->background)+i) != NULL)
            SDL_DestroyTexture(*(&(T->background)+i));
}

void sdlCustom_RenderBeautifulCat(RenderVar *RV)
{
    SDL_Rect dst = {0, RV->WIN_h/2, RV->WIN_h/2, RV->WIN_h/2};
    SDL_RenderCopy(RV->renderer, RV->Texture_UI.cat, NULL, &dst);
}

void sdlCustom_RenderSideTexturePNG(RenderVar *RV, int side, int ipos, int jpos, int id)
{
    int angle = side == 5 ? 90 : 0;

    side = side == 5 ? 4 : side;

    int wtext  = Tile_texture_size, 
        htext  = Tile_texture_size*(side%5),
        wscale = round(Tile_texture_size*RV->scale/1000.0),
        hscale = wscale;
    
    SDL_Rect src = {htext, 0, wtext, wtext};
    SDL_Rect dst = {jpos, ipos, wscale, hscale};

    SDL_RenderCopyEx(RV->renderer, *(&(RV->Texture_Tiles.abbey)+id), &src, &dst, angle, NULL, SDL_FLIP_NONE);
}

void sdlCustom_RenderSide_Base(RenderVar *RV, int side, int ipos, int jpos, tile *t)
{
    int id = 2;

    if(t->sides[side] == 'v')
        id = 4;

    else if(side == 4 && t->sides[side] == 'r')
    {

        if(t->sides[0] == 'r' && t->sides[2] == 'r')
            id = 3;

        if(t->sides[1] == 'r' && t->sides[3] == 'r')
        {
            side ++;
            id = 3;
        }

        int i;
        for(i = 0; i < 4; i++)
        {
            if(t->sides[i] == 'r' && t->sides[(i+3)%4] == 'r')
            {
                side = i;
                id = 6;
            }

            if(t->sides[i] != 'p' && t->sides[(i+1)%4] != 'p' && t->sides[(i+2)%4] != 'p')
            {
                side = i;
                id = 14;
                break;
            }
        }
    }

    else if(side != 4 || t->special == 0)
    {
        if(t->sides[side] == 'V')
            id = 5;
        
        else if(t->sides[side] == 'p')
            id = 2;
        
        else if(t->sides[side] == 'r')
            id = 3;
    }

    else if(t->sides[side] == 'V')
        id = 1;

    sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, id);
}

void sdlCustom_RenderSide_Overlay(RenderVar *RV, int side, int ipos, int jpos, tile *t)
{   
    // affiche les murs des villes, les blasons  et les abbeys en overlay

    if(side == 4 && t->sides[side] == 'p' && t->special == 1)
        sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 0);

    if(t->sides[side] != 'V')
        return;
    
    if(side != 4)
    {
        if(t->sides[(side+3)%4] != 'V')
            sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 9);
            
        if(t->sides[(side+1)%4] != 'V')
            sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 10);

        if(t->sides[4] != 'V')
        {
            if(t->sides[4] != 'r')
                sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 11);
            else
                sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 13);
        }
    }


    else
    {
        for(int i = 0; i < 4; i++)
            if(t->sides[i] != 'V')
            {
                if(t->sides[i] != 'r')
                {
                    sdlCustom_RenderSideTexturePNG(RV, i, ipos, jpos, 7);

                    if(t->sides[i] != 'V' && t->sides[(i+3)%4] != 'V')
                        sdlCustom_RenderSideTexturePNG(RV, i, ipos, jpos, 12);
                }

                else
                {
                    sdlCustom_RenderSideTexturePNG(RV, i, ipos, jpos, 8);
                }
                    
            }
        
        if(t->special == 1)
            sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 15);
    }
}

void sdlCustom_RenderSide_Details(RenderVar *RV, int side, int ipos, int jpos, tile *t)
{
    if(t->meeple[side] == 0 || t->meeple[side] > 7)
        return;

    sdlCustom_RenderSideTexturePNG(RV, side, ipos, jpos, 15+t->meeple[side]);
}

void sdlCustom_RenderTile(RenderVar *RV, int ipos, int jpos, tile *tile, bool highlight)
{
    int i;
    for(i = 0; i < 5; i++)
        sdlCustom_RenderSide_Base(RV, i, ipos, jpos, tile);

    for(i = 0; i < 5; i++)
        sdlCustom_RenderSide_Overlay(RV, i, ipos, jpos, tile);
    
    for(i = 0; i < 5; i++)
        sdlCustom_RenderSide_Details(RV, i, ipos, jpos, tile);
    
    if(tile->istemp == 1)
    {
        if(highlight == 0)
            sdlCustom_RenderSideTexturePNG(RV, 0, ipos, jpos, 23);

        if(highlight == 1)
            sdlCustom_RenderSideTexturePNG(RV, 0, ipos, jpos, 24);
    }
}

void sdlCustom_RenderGrid(RenderVar *RV, grid G, int i_TileHL, int j_TileHL)
{
    int i = 0,
        j = 0,
        HL = 0,
        sizetext = (Tile_texture_size*RV->scale)/1000,
        ipos = -sizetext-1, 
        jpos = -sizetext-1;
    
    int begi = 0,
        begj = 0,
        endi = 0,
        endj = 0;
    
    while(ipos+sizetext <= 0 && begi < N_CARDS*2)
    {
        ipos = (RV->TAR_h-(sizetext/2)) + sizetext*(begi-N_CARDS);
        begi++;
    }
    begi--;

    while(jpos+sizetext <= 0 && begi < N_CARDS*2)
    {
        jpos = (RV->TAR_w-(sizetext/2)) + sizetext*(begj-N_CARDS);
        begj++;
    }
    begj--;

    for(endi = begi; ipos < RV->WIN_h && endi < N_CARDS*2; endi++)
        ipos = (RV->TAR_h-(sizetext/2)) + sizetext*(endi-N_CARDS);
    
    for(endj = begj; jpos < RV->WIN_w && endj < N_CARDS*2; endj++)
        jpos = (RV->TAR_w-(sizetext/2)) + sizetext*(begj-N_CARDS);

    for(i = begi; i < endi; i++)
        for(j = begj; j < endj; j++)
        {
            ipos = (RV->TAR_h-(sizetext/2)) + sizetext*(i-N_CARDS);
            jpos = (RV->TAR_w-(sizetext/2)) + sizetext*(j-N_CARDS);

            if(G[i][j] != NULL)
            {
                HL = 0;
                if(i == i_TileHL && j == j_TileHL)
                    HL = 1;
                sdlCustom_RenderTile(RV, ipos, jpos, G[i][j], HL);
            }

            else
                sdlCustom_RenderSideTexturePNG(RV, 0, ipos, jpos, 25);
        }
}

bool sdlCustom_ColorBackground(SDL_Renderer *renderer, int r, int g, int b)
{
    if(SDL_SetRenderDrawColor(renderer, r, g, b, 255) != 0)
    {
        printf("\nErreur SDL_SetRenderDrawColor : %s", SDL_GetError());
        return 1;
    }
    if(SDL_RenderClear(renderer) != 0)
    {
        printf("\nErreur SDL_SetRenderDrawColor : %s", SDL_GetError());
        return 1;
    }
    return 0;
}

void sdlCustom_RenderGameFrame(RenderVar *RV, grid grid, int i_TileHL, int j_TileHL)
{
    SDL_RenderClear(RV->renderer);
    sdlCustom_RenderGrid(RV, grid, i_TileHL, j_TileHL);
}

void sdlCustom_GetPos_TileOnCursor(RenderVar *RV, SDL_Event event, int *i_TileHL, int *j_TileHL)
{
    float sizetext = (Tile_texture_size*RV->scale)/1000.0;

    int cursor_pos_i, cursor_pos_j;
    SDL_GetMouseState(&cursor_pos_j, &cursor_pos_i);

    float itmp, jtmp;

    jtmp = (float)-(RV->TAR_w +(sizetext/2.0) - cursor_pos_j)/sizetext + N_CARDS;
    itmp = (float)-(RV->TAR_h +(sizetext/2.0) - cursor_pos_i)/sizetext + N_CARDS;

    *i_TileHL = ceil(itmp);
    *j_TileHL = ceil(jtmp);
    
    if(*i_TileHL < 0 || *i_TileHL >= N_CARDS*2 || *j_TileHL < 0 || *j_TileHL >= N_CARDS*2)
    {
        (*i_TileHL) = 0;
        (*j_TileHL) = 0;
    }
}

void sdlCustom_give_points_for_abbey(RenderVar *RV, EventVar *EV, grid G)
{
    int i = EV->i_TileHL;
    int j = EV->j_TileHL;

    for(int k = -1; k <= 1; k++)
        for(int l = -1; l <= 1; l++)
            if((k != 0 || l != 0) && G[i+k][j+l] != NULL 
                                  && G[i+k][j+l]->sides[4] == 'p' 
                                  && G[i+k][j+l]->special == 1 
                                  && G[i+k][j+l]->meeple[4] > 0
                                  && G[i+k][j+l]->meeple[4] <= 5)
            {
                (EV->player_list[G[i+k][j+l]->meeple[4]-1]->points) ++;
                sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Points_abbey, G[i+k][j+l]->meeple[4], 1, 0);
                sdlCustom_LoadPointsCounterTexture(RV, EV, G[i+k][j+l]->meeple[4]);

                if(is_abbey_complete(G, i+k, j+l) == 1)
                {
                    (EV->player_list[G[i+k][j+l]->meeple[4]-1]->meeples) ++;
                    sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Complete_abbey, G[i+k][j+l]->meeple[4], 0, 1);
                    sdlCustom_LoadMeepleCounterTexture(RV, EV, G[i+k][j+l]->meeple[4]);
                    G[i+k][j+l]->meeple[4] = 0;
                }
            }
}

void sdlCustom_CheckStruct(RenderVar *RV, EventVar *EV, grid grid)
{
    for(int k = 0; k < 5; k++)
    {
        uint winners[5] = {0};
        uint score = 0;

        if(grid[EV->i_TileHL][EV->j_TileHL] == NULL)
            return;

        if(is_struct_closed(grid, EV->i_TileHL, EV->j_TileHL, &score, grid[EV->i_TileHL][EV->j_TileHL]->sides[k], k) == 1)
        {
            close_stuct(grid,  EV->i_TileHL, EV->j_TileHL, score, grid[EV->i_TileHL][EV->j_TileHL]->sides[k], k, EV->player_list, winners);
            
            int max_meeple = 0;

            for(int l = 0; l < 5; l++)
                if(winners[l] > max_meeple)
                    max_meeple = winners[l];
            
            for(int l = 0; l < 5; l++)
            {
                if(winners[l] != 0)
                {
                    if(winners[l] == max_meeple)
                    {
                        if(grid[EV->i_TileHL][EV->j_TileHL]->sides[k] == 'r')
                            sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Complete_roads, l+1, score, winners[l]);

                        if(grid[EV->i_TileHL][EV->j_TileHL]->sides[k] == 'V')
                            sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Complete_city, l+1, score, winners[l]);

                        EV->player_list[l]->points += score;
                    }

                    else
                    {
                        if(grid[EV->i_TileHL][EV->j_TileHL]->sides[k] == 'r')
                            sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_MeepleBack_roads, l+1, 0, winners[l]);

                        if(grid[EV->i_TileHL][EV->j_TileHL]->sides[k] == 'V')
                           sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_MeepleBack_city, l+1, 0, winners[l]);
                    }

                    sdlCustom_LoadMeepleCounterTexture(RV, EV, l+1);
                    sdlCustom_LoadPointsCounterTexture(RV, EV, l+1);
                }       
            }
        }
        clear_tmpmark(grid);
    }
}

void sdlCustom_PlaceMeeple(RenderVar *RV, EventVar *EV, grid grid)
{
    for(int i = 0; i < 2*N_CARDS; i++)
        for(int j = 0; j < 2*N_CARDS; j++)
            if(grid[i][j] == EV->deck[EV->current_tile])
            {
                EV->i_TileHL = i;
                EV->j_TileHL = j;
                break;
            }

    if(EV->deck[EV->current_tile] != NULL)
    {
        // Place le Meeple
        for(int k = 0; k < 5; k++)
        {
            if(EV->deck[EV->current_tile]->meeple[k] == 7)
            {
                EV->deck[EV->current_tile]->meeple[k] = EV->player_turn;
                (EV->player_list[EV->player_turn-1]->meeples) --;
                sdlCustom_LoadMeepleCounterTexture(RV, EV, EV->player_turn);
                Mix_PlayChannel(-1, RV->Game_Sounds.place_meeple, 0);
            }
            
            else
                EV->deck[EV->current_tile]->meeple[k] = 0;
        }

        if( EV->deck[EV->current_tile]->sides[4] == 'p' &&
            EV->deck[EV->current_tile]->special == 1    &&
            EV->deck[EV->current_tile]->meeple[4] > 0   &&
            EV->deck[EV->current_tile]->meeple[4] < 6)
        {
            int score = 0;

            for(int k = -1; k <= 1; k++)
                for(int l = -1; l <= 1; l++)
                    if(grid[EV->i_TileHL +k][EV->j_TileHL +l] != NULL)
                        score ++;

            (EV->player_list[EV->player_turn-1]->points) += score;

            // sdlCustom_AddEventLog_PlayerWinPoint_Abbey(EV->player_list[EV->player_turn], score);
            sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Points_abbey, EV->player_turn, score, 0);
            sdlCustom_LoadPointsCounterTexture(RV, EV, EV->player_turn);
        }
        
    }
    
    else
        clear_ALL_TempMeeple(grid);

    // Vérifie si les structures sont fermées 
    sdlCustom_CheckStruct(RV, EV, grid);

    EV->player_turn = EV->player_turn%EV->player_number +1;
    (EV->current_tile) ++;

    if(EV->current_tile < N_CARDS)
        EV->GameState = GameState_CHECK_TILE;
        
    else
        EV->GameState = GameState_INIT_END;
}

void sdlCustom_PlaceTile(RenderVar *RV, EventVar *EV, grid grid)
{
    if(grid[EV->i_TileHL][EV->j_TileHL] != NULL && grid[EV->i_TileHL][EV->j_TileHL]->istemp == 1)
    {  
        empty_tmpmark(grid);
        grid[EV->i_TileHL][EV->j_TileHL] = EV->deck[EV->current_tile];
        EV->deck[EV->current_tile]->istemp = 0;

        if(EV->player_list[EV->player_turn-1]->meeples > 0)
            EV->GameState = GameState_PLACE_MEEPLE;
        else
            sdlCustom_PlaceMeeple(RV, EV, grid);
        
        sdlCustom_give_points_for_abbey(RV, EV, grid);

        Mix_PlayChannel(-1, RV->Game_Sounds.place_tile, 0);
    }
}

void sdlCustom_HL_MeppleOnCursor(RenderVar *RV, EventVar *EV, grid grid, int i, int j)
{
    int sizetext = (Tile_texture_size*RV->scale)/1000;
    int sizemeeple = sizetext/3;
    int ipos = (RV->TAR_h-(sizetext/2)) + sizetext*(i-N_CARDS);
    int jpos = (RV->TAR_w-(sizetext/2)) + sizetext*(j-N_CARDS);
    int cursor_pos_i, cursor_pos_j;
    int meeple = -1;
    SDL_GetMouseState(&cursor_pos_j, &cursor_pos_i);

    if(cursor_pos_i > ipos+sizemeeple && cursor_pos_i < ipos+2*sizemeeple)
    {
        if(cursor_pos_j > jpos && cursor_pos_j < jpos+sizemeeple)
            meeple = 3;
            
        if(cursor_pos_j > jpos+sizemeeple && cursor_pos_j < jpos+2*sizemeeple)
            meeple = 4;

        if(cursor_pos_j > jpos+2*sizemeeple && cursor_pos_j < jpos+sizetext)
            meeple = 1;
    }

    if(cursor_pos_j > jpos+sizemeeple && cursor_pos_j < jpos+2*sizemeeple)
    {
        if(cursor_pos_i > ipos && cursor_pos_i < ipos+sizemeeple)
            meeple = 0;

        if(cursor_pos_i > ipos+2*sizemeeple && cursor_pos_i < ipos+sizetext)
            meeple = 2;
    }

    for(int k = 0; k < 5; k++)
    {
        if(  ((grid[i][j]->sides[k] == 'V' || grid[i][j]->sides[k] == 'r') && is_struct_unoccupied(grid, i, j, NULL, grid[i][j]->sides[k], k, NULL, false, false) == 1)
          || (grid[i][j]->sides[k] == 'p' && grid[i][j]->special == 1 && k == 4) )
            grid[i][j]->meeple[k] = 6;
        else
            grid[i][j]->meeple[k] = 0;
        
        clear_tmpmark(grid);
    }
    
    bool no_meeple_placement_possible = 1;

    for(int k = 0; k < 5; k++)
        if(grid[i][j]->meeple[k] > 0)
            no_meeple_placement_possible = 0;

    if(no_meeple_placement_possible == 1)
        sdlCustom_PlaceMeeple(RV, EV, grid);

    if(meeple != -1 && grid[i][j]->meeple[meeple] == 6)
        grid[i][j]->meeple[meeple] = 7;
}

void sdlCustom_MainMenu_init(RenderVar *RV, EventVar *EV, grid *grid)
{
    *grid = gen_rand_grid(&EV->deck);

    if(EV->current_tile != 0)
        for(int i = 0; i < EV->player_number; i++)
            free(EV->player_list[i]);

    EV->current_tile = 0;

    empty_tmpmark(*grid);

    sdlCustom_EventLog_free_All(&EV->EventLog);

    RV->TAR_h = round(RV->WIN_h/2.0);
    RV->TAR_w = round(RV->WIN_w/2.0);
    RV->scale = 2150;

    EV->GameState = GameState_MAIN_MENU;
}

void sdlCustom_Game_Event(RenderVar *RV, EventVar *EV, grid grid)
{
    // https://wiki.libsdl.org/SDL_Keycode
    while (SDL_PollEvent(&(EV->event))) 
    {
    
        if((EV->event).key.type == SDL_KEYDOWN)
        {
            switch((EV->event).key.keysym.sym)
            {
                case SDLK_ESCAPE :
                    EV->GameState = GameState_MAIN_MENU_INIT;
                    break;
                
                case SDLK_KP_PLUS :
                case SDLK_p :
                    Mix_VolumeMusic(Mix_VolumeMusic(-1)+10);
                    break;

                case SDLK_KP_MINUS :
                case SDLK_m :
                    if(Mix_VolumeMusic(-1)-10 <= 0)
                        Mix_VolumeMusic(0);
                    else
                        Mix_VolumeMusic(Mix_VolumeMusic(-1)-10);
                    break;

                case SDLK_s :
                    if(RV->scale > EV->min_zoom)
                        RV->scale -= EV->zoom_val;
                    break;

                case SDLK_z :
                    if(RV->scale < EV->max_zoom)
                        RV->scale += EV->zoom_val;
                    break;

                case SDLK_F4 :
                    if (((EV->event).key.keysym.mod && KMOD_LALT) && (EV->event).type == SDL_KEYDOWN)
                        secret = (secret+1)%2;
                    break;

                default :
                    break;
            }
        }

        if(EV->event.type == SDL_MOUSEWHEEL)
        {
            EV->new_scale = RV->scale+(EV->zoom_val * EV->event.wheel.y);

            if(EV->new_scale > EV->max_zoom)
                RV->scale = EV->max_zoom;
                
            else if(EV->new_scale < EV->min_zoom)
                RV->scale = EV->min_zoom;
            
            else
                RV->scale = EV->new_scale;
        }

        // clique gauche enfoncé
        if(EV->event.motion.state == SDL_BUTTON_RMASK)
        {
            RV->TAR_h += EV->event.motion.yrel;
            RV->TAR_w += EV->event.motion.xrel;
        }

        if(EV->GameState == GameState_PLACE_TILE)
        {
            sdlCustom_GetPos_TileOnCursor(RV, EV->event, &(EV->i_TileHL), &(EV->j_TileHL));

            if((EV->event).key.type == SDL_KEYDOWN && (EV->event).key.repeat == 0)
                switch((EV->event).key.keysym.sym)
                {
                    case SDLK_RIGHT :
                        rotate_tile_clockwise(EV->deck[EV->current_tile]);
                        EV->GameState = GameState_CHECK_TILE;
                        break;
                    
                    case SDLK_LEFT :
                        rotate_tile_counterclockwise(EV->deck[EV->current_tile]); 
                        EV->GameState = GameState_CHECK_TILE;
                        break;

                    default :
                        break;
                }

            // clique droit simple
            if(EV->event.button.type == SDL_MOUSEBUTTONDOWN && EV->event.button.button == SDL_BUTTON_LEFT)
                sdlCustom_PlaceTile(RV, EV, grid);
        }

        else if(EV->GameState == GameState_PLACE_MEEPLE)
        {
            clear_HL_TempMepple(grid);
            sdlCustom_GetPos_TileOnCursor(RV, EV->event, &(EV->i_TileHL), &(EV->j_TileHL));

            if(grid[EV->i_TileHL][EV->j_TileHL] != NULL && grid[EV->i_TileHL][EV->j_TileHL] == EV->deck[EV->current_tile])
                sdlCustom_HL_MeppleOnCursor(RV, EV, grid, EV->i_TileHL, EV->j_TileHL);

            if(EV->event.button.type == SDL_MOUSEBUTTONDOWN && EV->event.button.button == SDL_BUTTON_LEFT)
                sdlCustom_PlaceMeeple(RV, EV, grid);
        }

    }
}

void sdlCustom_LoadPlayerBotsNumbers(RenderVar *RV, EventVar *EV)
{
    if(RV->Texture_MainMenu.PlayerNumb != NULL) {
        SDL_DestroyTexture(RV->Texture_MainMenu.PlayerNumb);
    }
    if(RV->Texture_MainMenu.BotNumb != NULL) {
        SDL_DestroyTexture(RV->Texture_MainMenu.BotNumb);
    }


    char PN[2] = "0";
    char BN[2] = "0";
    
    PN[0] += EV->player_number;
    BN[0] += EV->bot_number;

    SDL_Color colorP = {200, 200, 200, 255};
    SDL_Color colorB = {200, 200, 200, 255};
    SDL_Surface *surface = NULL;

    if(EV->MainMenu_NumbSelected == 0)
    {
        colorP.r = 200;
        colorP.g = 200;
        colorP.b = 25;
    }
    else
    {
        colorB.r = 200;
        colorB.g = 200;
        colorB.b = 25;
    }
    
    surface = TTF_RenderUTF8_Solid(RV->font, PN, colorP);
    RV->Texture_MainMenu.PlayerNumb = SDL_CreateTextureFromSurface(RV->renderer, surface);

    surface = TTF_RenderUTF8_Solid(RV->font, BN, colorB);
    RV->Texture_MainMenu.BotNumb = SDL_CreateTextureFromSurface(RV->renderer, surface);

    SDL_FreeSurface(surface);
}

void sdlCustom_RenderPlayerBotNumbers(RenderVar *RV, EventVar *EV)
{
    SDL_Rect dst = {round(RV->WIN_w/2.0), round(RV->WIN_h/2.0), 0, 0};
    
    SDL_QueryTexture(RV->Texture_MainMenu.PlayerNumb, NULL, NULL, &dst.w, &dst.h);

    dst.w = round(dst.w*RV->WIN_h*0.0032);
    dst.h = round(dst.h*RV->WIN_h*0.0032);

    int gap = round(RV->WIN_h*0.15);

    dst.x -= round(dst.w/2.0) + gap;
    dst.y -= round(dst.h/2.0);

    SDL_RenderCopy(RV->renderer, RV->Texture_MainMenu.PlayerNumb, NULL, &dst);

    dst.x += gap*2 + round(RV->WIN_h*0.014);
    SDL_RenderCopy(RV->renderer, RV->Texture_MainMenu.BotNumb, NULL, &dst);
}

void sdlCustom_RenderMainMenuBackground(RenderVar *RV)
{
    SDL_Rect dst = {round(RV->WIN_w/2.0), 0, 0, 0};

    SDL_QueryTexture(RV->Texture_MainMenu.background, NULL, NULL, &dst.w, &dst.h);

    float scale = (1.0*RV->WIN_h)/(1.0*dst.h);

    dst.w = round(scale*dst.w);
    dst.h = round(scale*dst.h);

    dst.x -= round(dst.w/2.0);

    SDL_RenderCopy(RV->renderer, RV->Texture_MainMenu.background, NULL, &dst);
}

void sdlCustom_RenderMainMenuFrame(RenderVar *RV, EventVar *EV, grid grid)
{
    sdlCustom_RenderGrid(RV, grid, 0, 0);
    sdlCustom_RenderMainMenuBackground(RV);
    sdlCustom_RenderPlayerBotNumbers(RV, EV);
}

void sdlCustom_MainMenu_Event(RenderVar *RV, EventVar *EV)
{
    while (SDL_PollEvent(&(EV->event)))
    {
        if((EV->event).key.type == SDL_KEYDOWN)
        {
            switch((EV->event).key.keysym.sym)
            {
                case SDLK_ESCAPE :
                    EV->GameState = GameState_QUIT;
                    break;

                case SDLK_RETURN :
                    EV->GameState = GameState_START;
                    break;

                case SDLK_RIGHT :
                    EV->MainMenu_NumbSelected = 1;
                    sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    break;
                
                case SDLK_LEFT :
                    EV->MainMenu_NumbSelected = 0;
                    sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    break;
                
                case SDLK_UP :
                    if(EV->MainMenu_NumbSelected == 0 && EV->player_number < 5)
                    {
                        EV->player_number ++;
                        Mix_PlayChannel(-1, RV->Game_Sounds.main_menu_numb, 0);
                        sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    }
                    if(EV->MainMenu_NumbSelected == 1 && EV->bot_number < EV->player_number-1)
                    {
                        EV->bot_number ++;
                        Mix_PlayChannel(-1, RV->Game_Sounds.main_menu_numb, 0);
                        sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    }
                    
                    break;

                case SDLK_DOWN :
                    if(EV->MainMenu_NumbSelected == 0 && EV->player_number > 2)
                    {
                        EV->player_number --;
                        if(EV->bot_number == EV->player_number)
                            EV->bot_number --;
                        Mix_PlayChannel(-1, RV->Game_Sounds.main_menu_numb, 0);
                        sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    }
                    if(EV->MainMenu_NumbSelected == 1 && EV->bot_number > 0)
                    {
                        EV->bot_number --;
                        Mix_PlayChannel(-1, RV->Game_Sounds.main_menu_numb, 0);
                        sdlCustom_LoadPlayerBotsNumbers(RV, EV);
                    }
                    break;

                case SDLK_KP_PLUS :
                case SDLK_p :
                    Mix_VolumeMusic(Mix_VolumeMusic(-1)+10);
                    break;

                case SDLK_KP_MINUS :
                case SDLK_m :
                    if(Mix_VolumeMusic(-1)-10 <= 0)
                        Mix_VolumeMusic(0);
                    else
                        Mix_VolumeMusic(Mix_VolumeMusic(-1)-10);
                    break;
                    break;
                
                default :
                    break;
            }
        }
    }

}

void sdlCustom_RenderEventLog(RenderVar *RV, EventLog *EL)
{
    EventLog *tmp = EL;
    SDL_Rect dst  = {round(RV->WIN_w*0.05), round(RV->WIN_h*0.05), 0, 0};
    SDL_Rect rect = {round(dst.x-RV->WIN_w*0.02), round(dst.y-RV->WIN_h*0.02), round(RV->WIN_h*0.87), 0};
    SDL_SetRenderDrawColor(RV->renderer, 10, 10, 10, 95);

    while(tmp != NULL)
    {
        if(tmp->Time_On_Screen != 0)
        {
            SDL_QueryTexture(tmp->TextureText, NULL, NULL, &dst.w, &dst.h);

            dst.w  = round(dst.w*RV->WIN_h*0.0003);
            dst.h  = round(dst.h*RV->WIN_h*0.0003);

            rect.h = dst.h + round(RV->WIN_h*0.045);

            SDL_RenderFillRect(RV->renderer, &rect);
            SDL_RenderCopy(RV->renderer, tmp->TextureNumb, NULL, &dst);
            SDL_RenderCopy(RV->renderer, tmp->TextureText, NULL, &dst);

            dst.y += dst.h + round(RV->WIN_h*0.045);
            rect.y += rect.h;

            tmp->Time_On_Screen --;
        }
        tmp = tmp->next;
    }
}

void sdlCustom_RenderPlayerInfo(RenderVar *RV, EventVar *EV)
{
    float ui_scale = 0.00325;

    int wtext  = Tile_texture_size*EV->player_number, 
        htext  = Tile_texture_size,
        wscale = round(wtext*RV->WIN_h*ui_scale),
        hscale = round(htext*RV->WIN_h*ui_scale);
    
    SDL_Rect src = {0, 0, wtext, htext};
    SDL_Rect dst = {round((RV->WIN_w-wscale)/2.0), RV->WIN_h-hscale, wscale, hscale};
    
    SDL_Rect dstMepple = {  dst.x + round(hscale*0.10*5), 
                            dst.y + round(hscale*0.245), 
                            wscale, 
                            hscale};
    
    SDL_Rect dstScore  = {  dst.x + round(hscale*0.10*5), 
                            dst.y + round(hscale*0.74),
                            wscale, 
                            hscale};

    SDL_RenderCopy(RV->renderer, RV->Texture_UI.PlayersInfo, &src, &dst);

    if(EV->GameState != GameState_END)
    {        
        src.w = Tile_texture_size;
        src.x = Tile_texture_size*(EV->player_turn-1);
        dst.w = dst.h;
        dst.x += hscale*(EV->player_turn-1);

        SDL_RenderCopy(RV->renderer, RV->Texture_UI.PlayersTurn, &src, &dst);
    }
    // Affichage du score et du nombre de meeples

    // Meeples
    for(int i = 0; i < EV->player_number; i++)
    {
        SDL_QueryTexture(*((&RV->Texture_UI.Player1_Meeples)+i), NULL, NULL, &(dstMepple.w), &(dstMepple.h));
        
        dstMepple.w  = round(dstMepple.w*RV->WIN_h*0.00075);
        dstMepple.h  = round(dstMepple.h*RV->WIN_h*0.00075);

        if(i == 0)
            dstMepple.x -= round(dstMepple.w/2);

        SDL_RenderCopy(RV->renderer, *((&RV->Texture_UI.Player1_Meeples)+i), NULL, &dstMepple);

        dstMepple.x  += hscale + round(RV->WIN_h*0.0009);
    }

    // Score
    for(int i = 0; i < EV->player_number; i++)
    {   
        SDL_QueryTexture(*((&RV->Texture_UI.Player1_Points)+i), NULL, NULL, &(dstScore.w), &(dstScore.h));
        
        dstScore.w  = round(dstScore.w*RV->WIN_h*0.0005);
        dstScore.h  = round(dstScore.h*RV->WIN_h*0.0005);

        if(i == 0)
            dstScore.x -= round(dstScore.w/2);

        SDL_RenderCopy(RV->renderer, *((&RV->Texture_UI.Player1_Points)+i), NULL, &dstScore);

        dstScore.x  += hscale + round(RV->WIN_h*0.0009);
    }

    if(secret == 1)
        sdlCustom_RenderBeautifulCat(RV);
}

void sdlCustom_RenderCurrentTile(RenderVar *RV, EventVar *EV, grid grid)
{
    int scale = RV->scale;
    RV->scale = 3400;
    int ipos = RV->WIN_h/2 - Tile_texture_size*RV->scale/2000;
    int jpos = round(RV->WIN_w*0.87);
    bool istemp = EV->deck[EV->current_tile]->istemp;
     EV->deck[EV->current_tile]->istemp = 1;

    sdlCustom_RenderTile(RV, ipos, jpos, EV->deck[EV->current_tile], 0);

    ipos += Tile_texture_size*RV->scale/16000;
    jpos += Tile_texture_size*RV->scale/16000;

    RV->scale = 3000;
    EV->deck[EV->current_tile]->istemp = 0;

    sdlCustom_RenderTile(RV, ipos, jpos, EV->deck[EV->current_tile], 0);


    if(EV->GameState == GameState_PLACE_TILE && EV->current_tile != 0)
    {
        bool no_tile_placement_found = 1;


        for(int i = 0; i < N_CARDS*2; i++)
            for(int j = 0; j < N_CARDS*2; j++)
                if(grid[i][j] != NULL && grid[i][j] == EV->deck[EV->current_tile])
                {
                    no_tile_placement_found = 0;
                    break;
                }

        if(no_tile_placement_found == 1)
        {
            int texturesize = Tile_texture_size*RV->scale/1000;
            SDL_Rect dst = {jpos, ipos, texturesize, texturesize};
            SDL_RenderCopy(RV->renderer, RV->Texture_UI.no_position, NULL, &dst);
        }
    }

    EV->deck[EV->current_tile]->istemp = istemp;
    RV->scale = scale;
}

void sdlCustom_RenderUI(RenderVar *RV, EventVar *EV, grid grid)
{
    sdlCustom_RenderPlayerInfo(RV, EV);
    sdlCustom_RenderEventLog(RV, EV->EventLog);
    sdlCustom_RenderCurrentTile(RV, EV, grid);
}

void sdlCustom_RenderWinMessage(RenderVar *RV)
{
    SDL_Rect dst = {RV->WIN_w/2, RV->WIN_h/2, 0, 0};
    
    SDL_QueryTexture(RV->Texture_UI.win_message, NULL, NULL, &dst.w, &dst.h);

    dst.w = round(dst.w*RV->WIN_h*0.0008);
    dst.h = round(dst.h*RV->WIN_h*0.0008);

    dst.x -= dst.w/2;
    dst.y -= dst.h/2;

    int rectdiff = round(dst.w/20);

    SDL_Rect rect = {dst.x-rectdiff, dst.y-rectdiff, dst.w+2*rectdiff, dst.h+2*rectdiff};

    SDL_RenderFillRect(RV->renderer, &rect);
    SDL_RenderCopy(RV->renderer, RV->Texture_UI.win_message, NULL, &dst);
}

void sdlCustom_CountIncompleteStructuresPoints(RenderVar *RV, EventVar *EV, grid grid)
{
    int score[5] = {0};
    uint scoretmp = 0;
    bool playerIDtmp[5] = {0};

    for(int i = 0; i < N_CARDS*2; i++)
        for(int j = 0; j < N_CARDS*2; j++)
        {
            for(int k = 0; k < 4; k ++)
            {
                scoretmp = 0;

                for(int l = 0; l < 5; l ++)
                    playerIDtmp[l] = 0;

                if(grid[i][j] != NULL && grid[i][j]->sides[k] != 'p')
                {
                    is_struct_unoccupied(grid, i, j, &scoretmp, grid[i][j]->sides[k], k, playerIDtmp, false, true);
                    clear_tmpmark(grid);

                    for(int l = 0; l < 5; l ++)
                        if(playerIDtmp[l] != 0)
                            score[l] += scoretmp;
                }

            }
        }

    for(int k = 0; k < 5; k ++)
    {
        if(score[k] != 0)
        {
            EV->player_list[k]->points += score[k];
            sdlCustom_EventLog_ADD(RV, &EV->EventLog, GameEvent_Points_IncompleteStruct, k+1, score[k], 0);
            sdlCustom_LoadPointsCounterTexture(RV, EV, k+1);
        }
    }
}

bool sdlCustom_init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("\nErreur SDL_Init : %s", SDL_GetError());
        return 0;
    }
    if(TTF_Init() == -1)
    {
        printf("\nErreur TTF_Init :");
        return 0;
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    return 1;
}

int **sdlCustom_AIturn_fitness(grid grid, coord* coord_list, int size, tile* t, joueur *Agent)
{
    int **score = malloc(sizeof(int*)*size);
    if (score == NULL) {
        perror("error while malloc score in fitness");
    }
    for(int i = 0; i < size; i++) {
        score[i] = malloc(sizeof(int)*3);
        if (score[i] == NULL) {
            perror("error while malloc score[i] in fitness");
            exit(EXIT_FAILURE);
        } 
        score[i][0] = 0;
        score[i][1] = -1;
        score[i][2] = INT_MIN;
    }
    for (int i = 0; i < size; i++) {
        coord c = coord_list[i];
        grid[c.y][c.x] = t;
        int max_points = INT_MIN;
        int max_pos = -1;
        for (int j = 0; j < 4; j++) {
            int meeple_score_tmp = 0;
            bool meeple_placed = false;
            int disincentivize_new_struct = 1;
            if ((t->sides[j] == 'V') || (t->sides[j] == 'r')) {
                // place meeple if possible
                if (Agent->meeples > 0) {
                    if (is_struct_unoccupied(grid, c.y, c.x, NULL, t->sides[j], j, NULL, true, false)) {
                        grid[c.y][c.x]->meeple[j] = Agent->id;
                        Agent->meeples--;
                        meeple_placed = true;
                    }
                    clear_tmpmark(grid);
                }
                else {
                    // check if agent owns the structure
                    bool playerIDtmp[5] = {false};
                    uint scoretmp = 0;
                    is_struct_unoccupied(grid, c.y, c.x, &scoretmp, t->sides[j], j, playerIDtmp, true, false);
                    clear_tmpmark(grid);

                    if (playerIDtmp[Agent->id-1] == false) {
                        disincentivize_new_struct = DISINCENTIVIZE_NEW_STRUCT;
                    }
                }
                uint n = 0;
                // if placing a tile here completes a structure add points relative to the points the structure gives
                if (is_struct_closed(grid, c.y, c.x, &n, t->sides[j], j)) {
                    //printf("closes structure at %d %d and gives %u points\n", c.y, c.x, n);
                    clear_tmpmark(grid);
                    bool playerIDtmp[5] = {false};
                    uint scoretmp = 0;
                    is_struct_unoccupied(grid, c.y, c.x, &scoretmp, t->sides[j], j, playerIDtmp, true, false);
                    clear_tmpmark(grid);

                    

                    for (int k = 0; k < 5; k++) {
                        if (playerIDtmp[k] == true) {
                            if (k == (Agent->id-1)) {
                                int pts;
                                if ((t->sides[j] == 'V') && (scoretmp <= SELF_CITY_MIN_POINTS)) {
                                    pts = scoretmp / disincentivize_new_struct;
                                }
                                else {
                                    pts = scoretmp * SELF_STRUCT_COMPLETE_MULT / disincentivize_new_struct;
                                }
                                score[i][0] += pts;
                                //printf("the agent gets %d points for this tile\n", pts);
                                if (meeple_placed) {
                                    meeple_score_tmp += pts;
                                }
                            } 
                            else {
                                int pts;
                                if ((t->sides[j] == 'V') && (scoretmp <= SELF_CITY_MIN_POINTS)) {
                                    pts = scoretmp / disincentivize_new_struct;
                                }
                                else {
                                    pts = scoretmp * OPP_STRUCT_COMPLETE_MULT / disincentivize_new_struct;
                                }
                                score[i][0] -= pts;
                                //printf("the agent loses %d points for this tile since player %d owns it\n", pts, k+1);
                                if (meeple_placed) {
                                    meeple_score_tmp -= pts;
                                }
                            }
                        }
                    }
                }
                else {  // if placing a tile here enlarges a structure
                    // check if structure belongs to the player
                    clear_tmpmark(grid);
                    bool playerIDtmp[5] = {false};
                    uint scoretmp = 0;
                    is_struct_unoccupied(grid, c.y, c.x, &scoretmp, t->sides[j], j, playerIDtmp, false, false);
                    //printf("doesnt complete structure at %d %d and gives %u points\n", c.y, c.x, scoretmp);
                    clear_tmpmark(grid);

                    for (int k = 0; k < 5; k++) {
                        if (playerIDtmp[k] != false) {
                            if (k == (Agent->id-1)) {
                                int pts = scoretmp * SELF_STRUCT_INCOMPLETE_MULT / disincentivize_new_struct;
                                score[i][0] += pts;
                                //printf("the agent gets %d points for this tile\n", pts);
                                if (meeple_placed) {
                                    meeple_score_tmp += pts;
                                }
                            } else {
                                int pts = scoretmp * OPP_STRUCT_INCOMPLETE_MULT / disincentivize_new_struct;
                                score[i][0] -= pts;
                                //printf("the agent loses %d points for this tile since player %d owns it\n", pts, k+1);
                                if (meeple_placed) {
                                    meeple_score_tmp += pts;
                                }
                            }
                        }
                    }

                }

                if ((meeple_placed) && (meeple_score_tmp > max_points)) {
                    max_points = meeple_score_tmp;
                    max_pos = j;
                }

                
                if (grid[c.y][c.x]->meeple[j] != 0) {
                    grid[c.y][c.x]->meeple[j] = 0;
                    Agent->meeples++;
                }
            }
        }

        int meeple_score_tmp = 0;

        // check if tile is an abbey
        if ((grid[c.y][c.x]->sides[4] == 'p') && (grid[c.y][c.x]->special)) {
            score[i][0] += (9 * SELF_ABBEY_ADD) / 2; // add half the max points for the abbey
            meeple_score_tmp += SELF_ABBEY_ADD;
        }

        // check all adjacent tiles for abbey
        for (int k = -1; k <= 1; k++) {
            for (int l = -1; l <= 1; l++) {
                if ((k == 0) && (l == 0)) {
                    continue;
                }
                if (grid[c.y+k][c.x+l] != NULL) {
                    if ((grid[c.y+k][c.x+l]->sides[4] == 'p') && (grid[c.y+k][c.x+l]->special)) {
                        // if the abbey belongs to the agent add points
                        if (grid[c.y+k][c.x+l]->meeple[4] == Agent->id) {
                            score[i][0] += SELF_ABBEY_ADD;
                            meeple_score_tmp += SELF_ABBEY_ADD;
                        }
                        // otherwise subtract points
                        else {
                            score[i][0] -= OTHER_ABBEY_SUB;
                            meeple_score_tmp -= OTHER_ABBEY_SUB;
                        }
                    }
                }
            }
        }

        if (meeple_score_tmp > max_points) {
            max_points = meeple_score_tmp;
            if ((grid[c.y][c.x]->sides[4] == 'p') && (grid[c.y][c.x]->special)) // the center of the tile is an abbey
                max_pos = 4;
        }

        if (max_pos != -1) {
            score[i][1] = max_pos;
            score[i][2] = max_points;
        }
        grid[c.y][c.x] = NULL;
    }

    return score;
} 

void sdlCustom_AIturn_HandleAgent(RenderVar *RV, EventVar *EV, grid grid)
{
    tile *new_tile = EV->deck[EV->current_tile];

    uint v1 = 0;
    uint v2 = 0;
    uint v3 = 0;
    uint v4 = 0;

    coord* c1 = get_all_possible_positions(grid, new_tile, &v1);
    int ** f1 = sdlCustom_AIturn_fitness(grid, c1, v1, new_tile, EV->player_list[EV->player_turn-1]);
   
    rotate_tile_clockwise(new_tile);
    coord* c2 = get_all_possible_positions(grid, new_tile, &v2);
    int ** f2 = sdlCustom_AIturn_fitness(grid, c2, v2, new_tile, EV->player_list[EV->player_turn-1]);

    rotate_tile_clockwise(new_tile);
    coord* c3 = get_all_possible_positions(grid, new_tile, &v3);
    int ** f3 = sdlCustom_AIturn_fitness(grid, c3, v3, new_tile, EV->player_list[EV->player_turn-1]);

    rotate_tile_clockwise(new_tile);
    coord* c4 = get_all_possible_positions(grid, new_tile, &v4);
    int ** f4 = sdlCustom_AIturn_fitness(grid, c4, v4, new_tile, EV->player_list[EV->player_turn-1]);

    // get highest score and corresponding coordinates from all 4 rotations
    int max[] = {INT_MIN, -1, INT_MIN};
    coord c = {1, 1}; // if it places something at 1 1 it means that it cant place anything anywhere
    int index = 1;

    for (int i = 0; i < v1; i++) {
        //printf("f1[%d][0] = %d\n", i, f1[i][0]);
        if (f1[i][0] > max[0]) {
            //printf("this is the new max\n");
            max[0] = f1[i][0];
            max[1] = f1[i][1];
            max[2] = f1[i][2];
            c = c1[i];
        }
    }

    for (int i = 0; i < v2; i++) {
        //printf("f2[%d][0] = %d\n", i, f2[i][0]);
        if (f2[i][0] > max[0]) {
            //printf("this is the new max\n");
            max[0] = f2[i][0];
            max[1] = f2[i][1];
            max[2] = f2[i][2];
            c = c2[i];
            index = 2;
        }
    }

    for (int i = 0; i < v3; i++) {
        //printf("f3[%d][0] = %d\n", i, f3[i][0]);
        if (f3[i][0] > max[0]) {
            //printf("this is the new max\n");
            max[0] = f3[i][0];
            max[1] = f3[i][1];
            max[2] = f3[i][2];
            c = c3[i];
            index = 3;
        }
    }

    for (int i = 0; i < v4; i++) {
        //printf("f4[%d][0] = %d\n", i, f4[i][0]);
        if (f4[i][0] > max[0]) {
            //printf("this is the new max\n");
            max[0] = f4[i][0];
            max[1] = f4[i][1];
            max[2] = f4[i][2];
            c = c4[i];
            index = 4;
        }
    }

    switch (index) {
        case 1:
            rotate_tile_clockwise(new_tile);
            break;
        case 2:
            rotate_tile_clockwise(new_tile);
            rotate_tile_clockwise(new_tile);
            break;
        case 3:
            rotate_tile_counterclockwise(new_tile); // big brain
            break;
        case 4:
            break;
    }

    
    // place tile
    grid[c.y][c.x] = new_tile;

    // place meeple if possible
    if ((max[1] != -1) && EV->player_list[EV->player_turn-1]->meeples > 0) {
        if (max[2] > MINIMUM_MEEPLE_SCORE_FOR_PLACEMENT) {
            grid[c.y][c.x]->meeple[max[1]] = EV->player_turn;
            EV->player_list[EV->player_turn-1]->meeples--;
            sdlCustom_LoadMeepleCounterTexture(RV, EV, EV->player_turn);
        }
    }

    EV->i_TileHL = c.y;
    EV->j_TileHL = c.x;

    sdlCustom_CheckStruct(RV, EV, grid);

    sdlCustom_give_points_for_abbey(RV, EV, grid);

    if( new_tile->sides[4] == 'p' &&
        new_tile->special == 1    &&
        new_tile->meeple[4] > 0   &&
        new_tile->meeple[4] < 6)
    {
        int score = 0;

        for(int k = -1; k <= 1; k++)
            for(int l = -1; l <= 1; l++)
                if(grid[c.y + k][c.x +l] != NULL)
                    score ++;

        (EV->player_list[EV->player_turn-1]->points) += score;

        sdlCustom_EventLog_ADD(RV, &(EV->EventLog), GameEvent_Points_abbey, EV->player_turn, score, 0);
        sdlCustom_LoadPointsCounterTexture(RV, EV, EV->player_turn);
    }


    free(c1);
    free(c2);
    free(c3);
    free(c4);
    for (int i = 0; i < v1; i++) {
        free(f1[i]);
    }
    for (int i = 0; i < v2; i++) {
        free(f2[i]);
    }
    for (int i = 0; i < v3; i++) {
        free(f3[i]);
    }
    for (int i = 0; i < v4; i++) {
        free(f4[i]);
    }

    free(f1);
    free(f2);
    free(f3);
    free(f4);
}

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int statut = EXIT_FAILURE;

    if(sdlCustom_init() == 0)
        goto Quit;

    window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              1920, 1080, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        printf("\nErreur SDL_CreateWindow : %s", SDL_GetError());
        goto Quit;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL)
    {
        printf("\nErreur SDL_CreateRenderer : %s", SDL_GetError());
        goto Quit;
    }

    SDL_SetWindowTitle(window, "CARCASUM - Tile Game");

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    sdlCustom_ColorBackground(renderer, 100, 100, 100);
    SDL_RenderPresent(renderer);

    // Setting up Render Variables
/////////////////////////////////////////////////////////////////////////////////////////////
    RenderVar RV;
    RV.FPS_Lock  = 144;
    RV.FrameTime = 1.0/(float)(RV.FPS_Lock);
    RV.renderer = renderer;
    SDL_GetWindowSize(window, &RV.WIN_w, &RV.WIN_h);
    RV.TAR_h = round(RV.WIN_h/2.0);
    RV.TAR_w = round(RV.WIN_w/2.0);
    RV.scale = 2150;
    RV.font  = TTF_OpenFont("ressources/7-12-serif-bold.ttf", 80);

    // To prevent crash if the game quit on main menu, without loading UI Textures
    for(int i = 0; i < sizeof(Texture_UI)/sizeof(SDL_Texture*); i++)
        *(&(RV.Texture_UI.PlayersInfo)+i) = NULL;
/////////////////////////////////////////////////////////////////////////////////////////////

    // Setting up Event Variables
/////////////////////////////////////////////////////////////////////////////////////////////
    EventVar EV;
    EV.max_zoom  = 8000;
    EV.min_zoom  = 1000;
    EV.zoom_val  = 150;
    EV.new_scale = 0;
    EV.i_TileHL  = 0;
    EV.j_TileHL  = 0;
    EV.EventLog  = NULL;
    EV.deck = NULL;
    EV.current_tile  = 0;
    EV.player_number = 5;
    EV.bot_number = 4;
    EV.player_turn   = 1;
    EV.GameState = GameState_MAIN_MENU_INIT;
    EV.MainMenu_NumbSelected = 0;
/////////////////////////////////////////////////////////////////////////////////////////////

    int timer_until_nextFrameTime;
    srand(time(NULL));
    grid grid = NULL;

    // Load Texture for the MainMenu
    sdlCustom_LoadTilesTextures(renderer, &RV.Texture_Tiles);
    sdlCustom_LoadMainMenuTextures(renderer, &RV.Texture_MainMenu);
    RV.Texture_MainMenu.BotNumb = NULL;
    RV.Texture_MainMenu.PlayerNumb = NULL;
    sdlCustom_LoadPlayerBotsNumbers(&RV, &EV);

    // Load sounds and music
    sdlCustom_LoadGameSounds(&RV);

    // Start Playing the background music
    Mix_PlayMusic(RV.Game_Sounds.BackgroundMusic, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);

    // Setting up the number of tile
    N_CARDS = get_line_number("tuiles_base_simplifiees.csv");



    while(EV.GameState != GameState_QUIT)
    {
        // Start te main loop timer
        Uint64 start = SDL_GetPerformanceCounter();

        if(EV.GameState == GameState_MAIN_MENU_INIT)
            sdlCustom_MainMenu_init(&RV, &EV, &grid);

        // State switching, input event and rendering for the main menu
        if(EV.GameState == GameState_MAIN_MENU)
        {
            sdlCustom_MainMenu_Event(&RV, &EV);
            sdlCustom_RenderMainMenuFrame(&RV, &EV, grid);
        }

        // State switching, input event and rendering for the game
        if(EV.GameState >= GameState_START)
        {
            if(EV.GameState == GameState_START)
            {
                sdlCustom_LoadTextureUI(renderer, &RV.Texture_UI);
                free_main_menu_grid(grid, EV.deck);
                free_grid(grid);
                free_deck(EV.deck);
                EV.deck = readcsv("tuiles_base_simplifiees.csv");
                grid = Grid();
                
                for(int i = 0; i < EV.player_number; i++)
                {
                    if(i < EV.player_number-EV.bot_number)
                        EV.player_list[i] = Joueur(i+1);
                    else
                        EV.player_list[i] = Bot(i+1);
                }

                for(int i = 0; i < EV.player_number; i++)
                {
                    sdlCustom_LoadMeepleCounterTexture(&RV, &EV, i+1);
                    sdlCustom_LoadPointsCounterTexture(&RV, &EV, i+1);
                }

                grid[N_CARDS][N_CARDS] = EV.deck[0];
                EV.current_tile = 1;
                EV.player_turn  = 1; 

                EV.GameState = GameState_CHECK_TILE;
            }

            if(EV.GameState == GameState_CHECK_TILE)
            {
                if(EV.current_tile < N_CARDS)
                {
                    empty_tmpmark(grid);
                    if (EV.player_list[EV.player_turn-1]->is_ai)
                        EV.GameState = GameState_AI_TURN;
                    else {
                        place_temp_tile(grid, EV.deck[EV.current_tile]);
                        EV.GameState = GameState_PLACE_TILE;
                    }
                }
                else
                    EV.GameState = GameState_INIT_END;
            }

            if(EV.GameState == GameState_AI_TURN)
            {

                if(AI_frame_wait == 0)
                {
                    sdlCustom_AIturn_HandleAgent(&RV,  &EV, grid);
                    AI_frame_wait = round(RV.FPS_Lock*2.5);

                    EV.deck[EV.current_tile]->istemp = 1;

                    for(int i = 0; i < N_CARDS*2; i++)
                        for(int j = 0; j < N_CARDS*2; j++)
                            if(EV.deck[EV.current_tile] == grid[i][j])
                            {
                                EV.i_TileHL = i;
                                EV.j_TileHL = j;
                            }
                }

                if(AI_frame_wait == 1)
                {
                    EV.deck[EV.current_tile]->istemp = 0;
                    EV.player_turn = EV.player_turn%EV.player_number +1;
                    EV.deck[EV.current_tile] = NULL;
                    (EV.current_tile) ++;
                    EV.GameState = GameState_CHECK_TILE;
                }

                AI_frame_wait --;
            }

            sdlCustom_Game_Event(&RV, &EV, grid); 
            sdlCustom_RenderGameFrame(&RV, grid, EV.i_TileHL, EV.j_TileHL);
            sdlCustom_RenderUI(&RV, &EV, grid);
            sdlCustom_EventLog_freeLast(&EV.EventLog);

            if(EV.GameState == GameState_INIT_END)
            {
                Mix_PlayChannel(-1, RV.Game_Sounds.end_game, 0);

                sdlCustom_CountIncompleteStructuresPoints(&RV, &EV, grid);

                int winner = 0, maxpoints = 0;

                for(int i = 0; i < EV.player_number; i++)
                    if(EV.player_list[i]->points > maxpoints)
                    {
                        maxpoints = EV.player_list[i]->points;
                        winner = i+1;
                    }
                
                for(int i = 0; i < EV.player_number; i++)
                    if(EV.player_list[i]->points == maxpoints && i != winner-1)
                        winner = 0;

                sdlCustom_LoadWinMessageTexture(&RV, winner);

                EV.GameState = GameState_END;
            }

            if(EV.GameState == GameState_END)
                sdlCustom_RenderWinMessage(&RV);
        }

        SDL_RenderPresent(RV.renderer);
        
        // end the main loop timer
        Uint64 end = SDL_GetPerformanceCounter();

        // compute the remaining time to wait until the next frame
        float elapsedMS = (end-start)/(float)SDL_GetPerformanceFrequency()*1000.f;
        timer_until_nextFrameTime = floor(RV.FrameTime*1000.f - elapsedMS);
        timer_until_nextFrameTime = timer_until_nextFrameTime < 0 ? 0 : timer_until_nextFrameTime;
        
        SDL_Delay(timer_until_nextFrameTime);
    }

    if(EV.current_tile != 0)
        for(int i = 0; i < 5; i++)
            if(EV.player_list[i] != NULL)
                free(EV.player_list[i]);
    free_grid(grid);
    free_deck(EV.deck);
    sdlCustom_EventLog_free_All(&EV.EventLog);

    statut = EXIT_SUCCESS;
Quit:
    sdlCustom_UnloadTilesTexture(&(RV.Texture_Tiles));
    sdlCustom_UnloadUITextures(&(RV.Texture_UI));
    sdlCustom_UnloadMainMenuTextures(&(RV.Texture_MainMenu));
    sdlCustom_UnloadSounds(&(RV.Game_Sounds));

    if(RV.font != NULL)
        TTF_CloseFont(RV.font);
    if(renderer != NULL)
        SDL_DestroyRenderer(renderer);
    if(window != NULL)
        SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return statut;
}