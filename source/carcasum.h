#include <stdio.h>                                                      // Standard Input / output library
#include <stdlib.h>                                                     // Standard library
#include <stdbool.h>                                                    // Boolean data type
#include <time.h>                                                       // Time library (for random number generation)
#include <math.h>                                                       // Math library
#include <string.h>                                                     // String library
#include <ctype.h>                                                      // Character library
#include <limits.h>                                                     // Integer limits
#include <SDL2/SDL.h>                                                   // Simple DirectMedia Layer (SDL) library for cross-platform video output
#include <SDL2/SDL_image.h>                                             // Simple DirectMedia Layer (SDL) library to interact with images (png, jpg, etc.)
#include <SDL2/SDL_ttf.h>                                               // Simple DirectMedia Layer (SDL) library to create textures from fonts and strings
#include <SDL2/SDL_mixer.h>                                             // Simple DirectMedia Layer (SDL) library to play audio
    
// Game State               
#define GameState_QUIT           0                                      // state where the game is quiting, end of the main loop
#define GameState_MAIN_MENU      1                                      // state where the game is in main menu
#define GameState_MAIN_MENU_INIT 2                                      // state where the game initializes all the game struct for the main menu
#define GameState_START          8                                      // state where the game initializes all the game struct for the start of a game
#define GameState_CHECK_TILE     9                                      // state where the game check all the available position for the current tile
#define GameState_PLACE_TILE     10                                     // state where the player select a position for the current tile or rotate it
#define GameState_PLACE_MEEPLE   11                                     // state where the player choose to place a meeple or not in the newly placed tile
#define GameState_AI_TURN        12                                     // state where the AI plays
#define GameState_INIT_END       13                                     // state where the game check the winner and reload the win message texture
#define GameState_END            14                                     // state where the game render the win message
// Game Event               
#define GameEvent_Points_abbey              0                           // event triggered when a player wins points with an abbey
#define GameEvent_Complete_abbey            1                           // event triggered when a player completes an abbey
#define GameEvent_Complete_city             2                           // event triggered when a player completes a city
#define GameEvent_Complete_roads            3                           // event triggered when a player completes a road
#define GameEvent_MeepleBack_roads          4                           // event triggered when a player completes a city without winning any points
#define GameEvent_MeepleBack_city           5                           // event triggered when a player completes a road without winning any points
#define GameEvent_Points_IncompleteStruct   6                           // event triggered when a player wins points from all his incomplete structures
// AI               
#define SELF_STRUCT_COMPLETE_MULT   30                                  // multiplier for completing structures belonging to the Agent
#define SELF_STRUCT_INCOMPLETE_MULT 15                                  // multiplier for incomplete structures belonging to the Agent
#define OPP_STRUCT_COMPLETE_MULT    8                                   // multiplier for completing structures belonging to the Opponent
#define OPP_STRUCT_INCOMPLETE_MULT  4                                   // multiplier for incomplete structures belonging to the Opponent
#define SELF_ABBEY_ADD              10                                  // points to add if the tile is adding points for the agent via an abbey
#define OTHER_ABBEY_SUB             10                                  // points to subtract if the tile is subtracting points for the player via an abbey
#define DISINCENTIVIZE_NEW_STRUCT   10                                  // diviser for placing a new structure when the agent has no meeples left
// Others Game Const                
#define TimeBeforeEventLogDisapear  6                                   // time in seconds before an event log disapears
#define Tile_texture_size 64                                            // height (and width since tiles are square) in pixels of the texture of a tile (ressources/tile)
    
uint N_CARDS = 72;                                                      // the total nomber of tile in the deck, see readcsv for more info
    
typedef unsigned int uint;                                              // unsigned integer type
    
//////// Custom Struct              
    typedef struct tile_t                                               // struct that represents a tile
    {               
        char sides[5];                                                  // the sides of the tile (up, down, left, right, center)
        int meeple[5];                                                  // the meeple on the tile (0 if no meeple, 1-5 for the side it is on)
        bool special;                                                   // true if the tile is a special tile (abbey, castle)
        bool closed[4];                                                 // true if the side is closed 
        bool istemp;                                                    // true if the tile is a temporary tile
    } tile;             
    
    
    typedef struct joueur_t                                             // struct that represents a player 
    {               
        uint points;                                                    // the points of the player
        uint meeples;                                                   // the number of meeples of the player
        uint id;                                                        // the id of the player
        bool is_ai;                                                     // true if the player is controlled by the AI
    } joueur;               
    
    
    typedef struct coord_t                                              // struct that represents a coordinate
    {               
        int x;                                                          // the x coordinate
        int y;                                                          // the y coordinate
    } coord;                
    
    typedef tile*** grid;                                               // 2D grid of tiles
    
    
    
//////// Custom Struct using SDL                
    
    
    typedef struct Texture_Tiles                                        // struct containing the complete list of textures used to render tiles
    {               
        SDL_Texture             
                    *abbey,                                             // 0 
                    *blazon,                                            // 1 
                    *prairies,                                          // 2 
                    *roads,                                             // 3 
                    *town,                                              // 4 
                    *city,                                              // 5 
                    *roads_perpendicular,                               // 6 
                    *wall,                                              // 7
                    *wall_gate,                                         // 8
                    *wall_diagonal_left,                                // 9
                    *wall_diagonal_right,                               // 10
                    *alt_wall,                                          // 11
                    *patch_wall,                                        // 12
                    *alt_wall_gate,                                     // 13
                    *roads_intersection,                                // 14
                    *blazon_castle,                                     // 15 
                    *p1,                                                // 16
                    *p2,                                                // 17
                    *p3,                                                // 18
                    *p4,                                                // 19
                    *p5,                                                // 20
                    *p_select,                                          // 21
                    *p_highlight,                                       // 22
                    *tempmark,                                          // 23
                    *tempmark_highlight,                                // 24
                    *empty;                                             // 25
    } Texture_Tiles;                
    
    
    typedef struct Texture_MainMenu                                     // struct containing the complete list of textures used to render the main menu
    {               
        SDL_Texture             
                    *background,                
                    *PlayerNumb,                
                    *BotNumb;               
    } Texture_MainMenu;             
    
    
    typedef struct Texture_UI                                           // struct containing the complete list of textures used to render the game UI
    {               
        SDL_Texture             
                    *PlayersInfo,                       
                    *PlayersTurn,                   
                    *Player1_Meeples,                   
                    *Player2_Meeples,                   
                    *Player3_Meeples,                   
                    *Player4_Meeples,               
                    *Player5_Meeples,               
                    *Player1_Points,                
                    *Player2_Points,                
                    *Player3_Points,                
                    *Player4_Points,                
                    *Player5_Points,                
                    *no_position,               
                    *win_message,               
                    *cat;               
    } Texture_UI;               
    
    
    typedef struct Game_Sounds                                          // struct containing the complete list of sounds and music used in the game
    {               
        Mix_Music *BackgroundMusic;             
        Mix_Chunk *place_tile,              
                  *place_meeple,                
                  *end_game,                
                  *complete_struct,             
                  *main_menu_numb;              
    } Game_Sounds;              
    
    
    typedef struct EventLog                                             // struct containing all the information of an event log
    {               
        SDL_Texture *TextureText,                                       // Texture of the text of the event log without number
                    *TextureNumb;                                       // Texture of the text of the event with number and colored with the FindPlayerColor function
    
        uint Time_On_Screen,                                            // Time remaining in seconds before the log disapears
             EventID,                                                   // ID of the event (see GameEvent_...)
             playerID,                                                  // ID of the player who triggered the event from 1 to 5
             score,                                                     // Points gaigned by the player
             meeple;                                                    // Meeple regained by the player 
    
        struct EventLog *next;                                          // pointer to the next event log
    } EventLog;             
    
    
    typedef struct Render_variables                                     // struct containing all the ressources needed to render the whole game
    {               
        uint FPS_Lock;                                                  // frame per second lock (144 in the base game)
        float FrameTime;                                                // time in ms between each game frame
        SDL_Renderer *renderer;                                         // renderer used to render the game
        Texture_Tiles Texture_Tiles;                                    // see Texture_Tiles
        Texture_MainMenu Texture_MainMenu;                              // see Texture_MainMenu
        Texture_UI Texture_UI;                                          // see Texture_UI
        TTF_Font *font;                                                 // pointer to the font used in the game 
    
        int WIN_w;                                                      // with of the game windows
        int WIN_h;                                                      // height of the game windows
        int TAR_w;                                                      // x coordonate of the player deplacement target
        int TAR_h;                                                      // y coordonate of the player deplacement target
        int scale;                                                      // scale of the grid
    
        Game_Sounds Game_Sounds;                                        // see Game_Sounds
    } RenderVar;                        


    typedef struct Event_variables                      
    {                       
        SDL_Event event;                                                // input event, see https://wiki.libsdl.org/SDL_Event

        int max_zoom,                                                   // max zoom value for the game scale, see RenderVar
            min_zoom,                                                   // min zoom value for the game scale, see RenderVar
            zoom_val,                                                   // value wich will be added to the game scale, see RenderVar
            new_scale,                                                  // new value of the game scale after mousewheel computing, see RenderVar
            i_TileHL,                                                   // y positiion of the highlighted tile on the grid
            j_TileHL,                                                   // x positiion of the highlighted tile on the grid
            player_number,                                              // number of total player in the current game
            bot_number,                                                 // number of bot player in the current game
            player_turn,                                                // ID of the player who play from 1 to 5
            current_tile,                                               // ID of the current tile in deck, from 0 to N_CARDS
            GameState;                                                  // current state of the game, see GameState

        EventLog *EventLog;                                             // pointer to the first event of the game event log, see EventLog

        joueur* player_list[5];                                         // list of all the player, see joueur
        tile**  deck;                                                   // deck of the current game, see deck

        bool MainMenu_NumbSelected;                                     // tell wich option is selected in the main menu
                                                                        // 0 : the player number is selected to be modified
                                                                        // 1 : the bot number is selected to be modifier

    } EventVar;         



//////// Custom Functions for the game strcut           

    ////// Functions to load the deck           

        // return the number of line in the file
        uint get_line_number(char* filename);                       

        // convert a string to lowercase
        void lowercase(char* str);                                  

        // read the file and return a deck of tiles
        tile** readcsv(char* filename);                             

        // shuffle the deck
        void deck_shuffle(tile** deck);                             

        // free the deck
        void free_deck(tile** deck);                                


    ////// Functions to alloc and free the grid, tiles and player struct

        // alloc and return a grid struct
        grid Grid();                                                

        // free the grid struct
        void free_grid(grid g);                                         

        // alloc and return a tile struct
        tile* Tile();                                               

        // alloc and return a player struct
        joueur* Joueur(uint id);                                    

        // alloc and return a player struct that is designated as a bot
        joueur* Bot(uint id);                                       


    ////// Functions to rotate tile         

        // rotate a tile counterclockwise
        void rotate_tile_counterclockwise(tile* t);                 

        // rotate a tile clockwise
        void rotate_tile_clockwise(tile* t);                        


    ////// Functions to check a struct or a tile emplacement

        // check if a tile can be placed on the grid
        bool is_valid_pos(grid grid,                                
                          tile* tile,           
                          coord coord);         

        // return all the available positions on the grid (ie. emtpy spaces that are adjacent to a tile)
        coord*  get_all_available_positions(grid grid,              
                                            uint* size);            

        // return all the possible positions on the grid (ie. all empty spaces where you can place the tile)
        coord* get_all_possible_possitions(grid g,                  
                                           tile* t,             
                                           uint* size);         

        // place a tile marked as temporary on the grid
        void place_temp_tile(grid grid, tile* tile);                

        // check if a struct is closed
        bool is_struct_closed(grid G,                               
                              int i,            
                              int j,            
                              uint *score,          
                              char c,           
                              int side);            

        // check if a struct is unoccupied (ie. no meeple on it)
        bool is_struct_unoccupied(grid G,                           
                                  int i,            
                                  int j,            
                                  uint* score,          
                                  char c,           
                                  int side,             
                                  bool* playerID,           
                                  bool cities_count_double,
                                  bool close_struct);           

        // close a structure
        void close_stuct(grid G,                                    
                         int i,             
                         int j,             
                         uint score,            
                         char c,            
                         int side,          
                         joueur* p[5],          
                         uint winners[5]);          

        // check if the Abbey is complete 
        bool is_abbey_complete(grid G,                              
                               int i,           
                               int j);          

    ////// Functions to do basic manipulation of grid           

        void clear_tmpmark(grid G);                                     // sets the isTemp field of all the tiles on the grid to false


        void empty_tmpmark(grid G);                                     // removes every tile with isTemp == true from the grid


        void clear_HL_TempMepple(grid G);                               // Replace all the highlighted meeple (meeple = 7) by normal temp meeeple (meeple = 6)


        void clear_ALL_TempMeeple(grid G);                              // Replace all the temp meeeple (meeple = 6) by empty meeple (meeple = 0)



    grid gen_rand_grid(tile *** pdeck);                                 // generate a random grid with the given deck



//////// Custom Functions Using SDL

    // return the SDL_color value corresponding to each player
    void sdlCustom_FindPlayerColor(                                 
                                SDL_Color *color,                       // color where the value will be stored
                                uint playerID);                         // ID of the player from 1 to 5

    // color the screen of the selected color with an alpha value of 255
    bool sdlCustom_ColorBackground(                                 
                                SDL_Renderer *renderer,                 //renderer to fill
                                int r,                                  // r component of the desired color  
                                int g,                                  // g component of the desired color 
                                int b);                                 // b component of the desired color 


    ////// Functions to load Textures and Sounds

        // Load the game's sound in the Game_Sounds struct stored in RV.Game_Sounds
        void sdlCustom_LoadGameSounds(                              
                                                RenderVar *RV);         // RenderVar.GameSounds need to be empty

        // Reload the texture of the meeple counter of the selected player stored in RV.Texture_UI
        void sdlCustom_LoadMeepleCounterTexture(                    
                                                RenderVar *RV,          // RenderVar.GameSounds need to be empty
                                                EventVar *EV,           // EventVar struct must be initialized
                                                int playerID);          // ID of the player selected from 1 to 5

        // Reload the texture of the points counter of the selected player stored in RV.Texture_UI
        void sdlCustom_LoadPointsCounterTexture(                    
                                                RenderVar *RV,          // RenderVar struct containing the Texture_UI struct
                                                EventVar *EV,           // EventVar struct must be initialized
                                                int playerID);          // ID of the player selected from 1 to 5

        // Reload the win message texture stores in RV.Texture_UI
        void sdlCustom_LoadWinMessageTexture(                       
                                                RenderVar *RV,          // RenderVar struct containing the Texture_UI struct
                                                uint playerID);         // ID of the player who win the game from 1 to 5

        // Load the textures of a new event log
        void sdlCustom_LoadEventLogTexture(                         
                                            RenderVar *RV,              // RenderVar struct containing the renderer
                                            EventLog *EL);              // Pointer to the bew event of the list

        // Reload the texture of the player and bots number in RV.Texture_MainMenu
        void sdlCustom_LoadPlayerBotsNumbers(                       
                                            RenderVar *RV,              // RenderVar struct containing the Texture_MainMenu struct
                                            EventVar *EV);              // EventVar struct must be initialized

        // Load all the UI texture from the ressource/ui and ressource/secret file using IMG_LoadTexture function
        void sdlCustom_LoadTextureUI(                               
                                    SDL_Renderer* renderer,             // renderer of the current window
                                    Texture_UI *T);                     // struct where the textures pointers will be loaded 

        // Load all the tiles texture from the ressource/tiles file using IMG_LoadTexture function
        void sdlCustom_LoadTilesTextures(                           
                                        SDL_Renderer* renderer,         // renderer of the current window
                                        Texture_Tiles *T);              // struct where the textures pointers will be loaded 

        // Load all the main menu texture from the ressource/menu file using IMG_LoadTexture function
        void sdlCustom_LoadMainMenuTextures(                        
                                            SDL_Renderer* renderer,     // renderer of the current window
                                            Texture_MainMenu *T);       // struct where the textures pointers will be loaded


    ////// Functions to Unload Textures and Sounds
        void sdlCustom_UnloadSounds(Game_Sounds *GS);

        void sdlCustom_UnloadTilesTexture(Texture_Tiles *T);

        void sdlCustom_UnloadUITextures(Texture_UI *T);

        void sdlCustom_UnloadMainMenuTextures(Texture_MainMenu *T);


    ////// Event Log Functions

        // Add a new EventLog
        void sdlCustom_EventLog_ADD(                                
                                    RenderVar *RV,                      // RenderVar struct must be initialized
                                    EventLog **EL,                      // Pointer to the first event of the list
                                    uint EventID,                       // ID of the event (see GameEvent_...)
                                    uint playerID,                      // ID of the player who made the event from 1 to 5
                                    uint score,                         // Point gaigned by the player
                                    uint meeple);                       // Meeple regained by the player 

        // Free the first event of the list stored in an EventLog struct
        void sdlCustom_EventLog_Free(                               
                                    EventLog *EL);                      // Pointer to the first event of the list

        // Free all the events of the list stored in an EventLog struct
        void sdlCustom_EventLog_free_All(                           
                                    EventLog **EL);                     // Pointer to the first event of the list

        // Free the last event of the list stored in an Event log struct if Time_On_Screen element is equal to 0
        void sdlCustom_EventLog_freeLast(                           
                                    EventLog **EL);                     // Pointer to the first event of the list


    ////// Rendering Functions

        // Render the side of a tile texture on screen at selected position
        void sdlCustom_RenderSideTexturePNG(                        
                                            RenderVar *RV,              // RenderVar struct must be initialized
                                            int side,                   // Side of the Tile determine the part of the texture which will be used
                                            int ipos,                   // the y position of the texture
                                            int jpos,                   // the x position of the texture
                                            int id);                    // the ID of the texture in the struct Texture_Tiles

        // Render the base of the side of a tile on screen at selected position                             
        void sdlCustom_RenderSide_Base(
                                    RenderVar *RV,                      // RenderVar struct must be initialized
                                    int side,                           // Side of the Tile to render
                                    int ipos,                           // the y position of the tile
                                    int jpos,                           // the x position of the tile
                                    tile *t);                           // tile to render

        // Render the overlay of the side of a tile on screen at selected position             
        void sdlCustom_RenderSide_Overlay(                          
                                        RenderVar *RV,                  // RenderVar struct must be initialized
                                        int side,                       // Side of the Tile to render
                                        int ipos,                       // the y position of the tile
                                        int jpos,                       // the x position of the tile
                                        tile *t);                       // tile to render

        // Render the details of the side of a tile on screen at selected position
        void sdlCustom_RenderSide_Details(                          
                                        RenderVar *RV,                  // RenderVar struct must be initialized
                                        int side,                       // Side of the Tile to render
                                        int ipos,                       // the y position of the tile
                                        int jpos,                       // the x position of the tile
                                        tile *t);                       // tile to render

        // Render the base, the overlay and the details of all the side of a tile at selected position, NULL tiles are supported
        void sdlCustom_RenderTile(                                      
                                RenderVar *RV,                          // RenderVar struct must be initialized
                                int ipos,                               // the y position of the tile
                                int jpos,                               // the x position of the tile
                                tile *tile,                             // tile to render
                                bool highlight);                        // tell if the tile is hovered by the mouse cursor

        // RenderVar struct must be initialized 
        void sdlCustom_RenderGrid(  
            RenderVar *RV,              
                                grid G,                                 // the grid to render
                                int i_TileHL,                           // the y position in the grid of the highlighted tile
                                int j_TileHL);                          // the x position in the grid of the highlighted tile

        // Clear the rendering target and render a new frame of the current grid
        void sdlCustom_RenderGameFrame(RenderVar *RV,                   // RenderVar struct must be initialized
                                       grid grid,                       // the grid to render
                                       int i_TileHL,                    // the y position in the grid of the highlighted tile
                                       int j_TileHL);                   // the x position in the grid of the highlighted tile

        // Render a secret image on the button right corner of the screen
        void sdlCustom_RenderBeautifulCat(RenderVar *RV);               // RenderVar struct must be initialized

        // Render the texture of the player & bots number for the main menu
        void sdlCustom_RenderPlayerBotNumbers(RenderVar *RV,            // RenderVar struct must be initialized
                                              EventVar *EV);            // EventVar struct must be initialized

        // Render the texture of the main menu background   
        void sdlCustom_RenderMainMenuBackground(RenderVar *RV);         // RenderVar struct must be initialized

        // Render a frame of the main menu using RenderMainMenuBackground, RenderPlayerBotNumbers and RenderGrid
        void sdlCustom_RenderMainMenuFrame(RenderVar *RV,               // RenderVar struct must be initialized
                                           EventVar *EV,                // EventVar struct must be initialized
                                           grid grid);                  // the grid to render in background

        // Render the full list of event on the UI  
        void sdlCustom_RenderEventLog(RenderVar *RV,                    // RenderVar struct must be initialized
                                      EventLog *EL);                    // EventVar struct must be initialized

        // Render the number of meeple and points of each player on the UI
        void sdlCustom_RenderPlayerInfo(RenderVar *RV,                  // RenderVar struct must be initialized
                                        EventVar *EV);                  // EventVar struct must be initialized

        // Render a preview of the current tile on the UI   
        void sdlCustom_RenderCurrentTile(RenderVar *RV,                 // RenderVar struct must be initialized
                                         EventVar *EV,                  // EventVar struct must be initialized
                                         grid grid);                    // grid to check if a position is available

        // Render the complete UI using RenderCurrentTile, RenderPlayerInfo & RenderEventLog
        void sdlCustom_RenderUI(RenderVar *RV,                          // RenderVar struct must be initialized
                                EventVar *EV,                           // EventVar struct must be initialized
                                grid grid);                             // grid for the RenderCurrentTile function

        // Render the win message at the end of the game    
        void sdlCustom_RenderWinMessage(RenderVar *RV);                 // RenderVar struct must be initialized


    ////// Cursor Functions
        // Set the position of the tile highlighted by the mouse cursor
        // if the position is out of boud, the value 0 is atribued to both position 
        void sdlCustom_GetPos_TileOnCursor(RenderVar *RV,               // RenderVar struct must be initialized
                                           SDL_Event event,             // SDL_Event struct containing the cursor position on the window
                                           int *i_TileHL,               // the y position of the highlighted tile
                                           int *j_TileHL);              // the x position of the highlighted tile

        // Set the position of the meeple highlighted by the mouse cursor on a newly placed tile
        void sdlCustom_HL_MeppleOnCursor(RenderVar *RV,                 // RenderVar struct must be initialized
                                         EventVar *EV,                  // EventVar struct must be initialized
                                         grid grid,                     // grid containing the tile
                                         int i,                         // y position of the tile on the grid
                                         int j);                        // x position of the tile on the grid

    ////// Struct Checking Functions
    ////// These function use RenderVar and EventVar to generate EventLog using the EventLog_ADD function
        // give all the point for abbeys when a new tile is placed
        void sdlCustom_give_points_for_abbey(RenderVar *RV,             // RenderVar struct must be initialized
                                             EventVar *EV,              // EventVar struct must be initialized 
                                             grid G);                   // grid containing the structures

        // check if a city or a road is closed by the newly placed tile
        void sdlCustom_CheckStruct(RenderVar *RV,                       // RenderVar struct must be initialized
                                   EventVar *EV,                        // EventVar struct must be initialized 
                                   grid grid);                          // grid containing the structures

        // compute and give all the point for incomplete structure to all players
        void sdlCustom_CountIncompleteStructuresPoints(RenderVar *RV,   // RenderVar struct must be initialized
                                                       EventVar *EV,    // EventVar struct must be initialized 
                                                       grid grid);      // grid containing all the structures


    ////// Functions Placing Meeple and Tiles
    ////// These function are essential to the game because they change the GameState
        // Place a Meeple and check all the strctures on the newly placed tile
        void sdlCustom_PlaceMeeple(RenderVar *RV,                       // RenderVar struct must be initialized
                                   EventVar *EV,                        // EventVar struct must be initialized 
                                   grid grid);                          // grid containing all the placed tile

        // Place a tile and compute all the point for abbeys
        void sdlCustom_PlaceTile(RenderVar *RV,                         // RenderVar struct must be initialized
                                 EventVar *EV,                          // EventVar struct must be initialized 
                                 grid grid);                            // grid containing all the placed tile


    ////// Init Functions
        // initialize the game variable for the main menu
        void sdlCustom_MainMenu_init(RenderVar *RV,                     // RenderVar struct must be initialized
                                     EventVar *EV,                      // EventVar struct must be initialized 
                                     grid *grid);                       // grid who will contain a random game for the background

        // Initialize SDL, SDL_TTF and SDL_Mixer
        // return 0 if an error is encoutered
        // return 1 if everything is initalize without trouble
        bool sdlCustom_init();


    ////// Event Functions
        // manage player input for the game
        void sdlCustom_Game_Event(RenderVar *RV,                        // RenderVar struct must be initialized
                                  EventVar *EV,                         // EventVar struct must be initialized 
                                  grid grid);                           // grid containing all the placed tile

        // manage player input for the main menu
        void sdlCustom_MainMenu_Event(RenderVar *RV,                    // RenderVar struct must be initialized
                                      EventVar *EV);                    // EventVar struct must be initialized 


    ////// AI Functions
        // This function computes the score for each position for a given tile and a given player
        // it returns an array of int[3],
                    // the first element is the score for the position, 
                    // the second is the best spot to place a meeple, 
                    // the third is the expected ammount of points is the score for the meeple position
        int **sdlCustom_AIturn_fitness(grid grid,                       // grid containing all the placed tile
                                       coord* coord_list,               // list of all the possible position for the next tile
                                       int size,                        // size of returned int list
                                       tile* t,                         // tile to place
                                       joueur *Agent);                  // player who will play the turn

        // This function handles the turn of an agent, it computes the optimal position for the next tile for each rotation, and places it. 
        // It also places a meeple if the points it brings are above a certain threshold
        void sdlCustom_AIturn_HandleAgent(RenderVar *RV,                // RenderVar struct must be initialized
                                          EventVar *EV,                 // EventVar struct must be initialized
                                          grid grid);                   // grid containing all the placed tile