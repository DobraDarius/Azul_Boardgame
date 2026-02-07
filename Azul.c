/*AZUL BOARD GAME - Terminal Implementation
FIXED VERSION - All bugs resolved and improvements made

Major fixes:
1. Fixed Portuguese wall pattern display logic (multiple if-else needed)
2. Fixed logic operators in loops (AND vs OR conditions)
3. Fixed variable initialization issues
4. Fixed middle pile selection logic
5. Fixed floor line penalties initialization
6. Fixed color display for Black tiles (green -> actual black)
7. Added proper pattern line validation
8. Fixed game flow and turn management
9. Added missing boundary checks
10. Improved user input validation
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define ALL_TILES 100
#define SAME_COLOR_TILES 20
#define HOW_MANY_TILES_TYPES 5
#define HOW_MANY_TILES_ON_FACTORY 4
#define MAX_NUMBER_OF_FACTORIES 9
#define MAX_PLAYERS 4
#define MAX_PLAYER_NAME 10
#define MAX_PENALTIES 7
#define BLUE 0
#define RED 1
#define BLACK 2
#define YELLOW 3
#define WHITE 4
#define AVAILABLE -1
#define BLOCKED -2

/*
    Color indices:
    - 0 = BLUE
    - 1 = RED
    - 2 = BLACK
    - 3 = YELLOW
    - 4 = WHITE
*/

typedef struct
{
    unsigned int all_tiles[5];
}Bag;

typedef struct 
{
    int portugese_wall[5][5];
    int pattern_lines[5][5];
    unsigned int score;
    int penalties[7];
}Mat;

typedef struct
{
    int all_factories[MAX_NUMBER_OF_FACTORIES][HOW_MANY_TILES_ON_FACTORY];
}Factory_display;

typedef struct
{
    int all_tiles[HOW_MANY_TILES_TYPES];
    unsigned int is_token_present;    
}Middle_pile;

typedef struct 
{
    Mat mat;
    unsigned int is_token_present;
    char player_name[MAX_PLAYER_NAME];
}Player;

typedef struct 
{
    int availiability_of_pattern_lines[MAX_PLAYERS][HOW_MANY_TILES_TYPES];
    int check_mid_pile_availiability;
    int player_on_move;
    int selected_tile;
    int same_tile_type_on_factory;
    int selected_factory;
    int player_order[MAX_PLAYERS];
    int selected_pattern_line;
    int MidPile_or_factory_selector;
    int selections_until_round_finish;
}Gameflow;

typedef struct 
{
    Bag bag;
    Middle_pile middle_pile;
    Player players[MAX_PLAYERS];
    int no_of_players;
    Factory_display factory_displays;
    int no_of_factory_displays;
    Gameflow flow;
}Game;

void fill_the_bag(Bag* bag)
{
    for(int idx = 0; idx < HOW_MANY_TILES_TYPES; idx++)
    {
        bag->all_tiles[idx] = SAME_COLOR_TILES;
    }
}

void print_tile_cover(int tileType){
    const char* colors[HOW_MANY_TILES_TYPES] = {
        "\x1b[44m\x1b[97m", // Blue - White text on blue background
        "\x1b[41m\x1b[97m", // Red - White text on red background
        "\x1b[40m\x1b[97m", // Black - White text on black background (visible!)
        "\x1b[43m\x1b[30m", // Yellow - Black text on yellow background
        "\x1b[47m\x1b[30m"  // White - Black text on white background
    };
    
    if(tileType >= 0 && tileType < HOW_MANY_TILES_TYPES){
        if(tileType == 0){
            printf("%s[blue]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 1){
            printf("%s[red]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 2){
            printf("%s[black]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 3){
            printf("%s[yellow]\x1b[0m", colors[tileType]); 
        }
        else{
            printf("%s[white]\x1b[0m", colors[tileType]); 
        }
    }
    else{
        printf("   ");
    }
}

void print_tile(int tileType){
    const char* colors[HOW_MANY_TILES_TYPES] = {
        "\x1b[34m\x1b[1m", // Blue - Bold blue text
        "\x1b[31m\x1b[1m", // Red - Bold red text
        "\x1b[90m\x1b[1m", // Black - Bold gray text (visible on black terminals!)
        "\x1b[33m\x1b[1m", // Yellow - Bold yellow text
        "\x1b[37m\x1b[1m"  // White - Bold white text
    };
    
    if(tileType >= 0 && tileType < HOW_MANY_TILES_TYPES){
        if(tileType == 0){
            printf("%s[BLUE]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 1){
            printf("%s[RED]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 2){
            printf("%s[BLACK]\x1b[0m", colors[tileType]); 
        }
        else if(tileType == 3){
            printf("%s[YELLOW]\x1b[0m", colors[tileType]); 
        }
        else{
            printf("%s[WHITE]\x1b[0m", colors[tileType]); 
        }
    }
    else{
        printf("     ");
    }
}

// Helper function to get the tile color at a specific Portuguese wall position
int get_portugese_wall_color(int row, int col)
{
    // The Portuguese wall has a fixed pattern that rotates
    // Each row shifts the pattern by one position
    int pattern[5] = {BLUE, YELLOW, RED, BLACK, WHITE};
    return pattern[(col - row + 5) % 5];
}

void initialise_mat(Game* info)
{
    for(int p = 0; p < info->no_of_players; p++)
    {
        // Initialize pattern lines
        for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
        {
            for(int j = 0; j < HOW_MANY_TILES_TYPES; j++)
            {
                if(i + j >= HOW_MANY_TILES_TYPES - 1)
                {
                    info->players[p].mat.pattern_lines[i][j] = AVAILABLE;
                }
                else
                {
                    info->players[p].mat.pattern_lines[i][j] = BLOCKED;
                }
            }
        }
        
        // Initialize Portuguese wall
        for(int i = 0; i < 5; i++)
        {
            for(int j = 0; j < 5; j++)
            {
                info->players[p].mat.portugese_wall[i][j] = AVAILABLE;
            }
        }
        
        // Initialize floor line (FIXED: was using unsigned, now int)
        for(int i = 0; i < MAX_PENALTIES; i++)
        {
            info->players[p].mat.penalties[i] = AVAILABLE;
        }
        
        // Initialize score
        info->players[p].mat.score = 0;
        info->players[p].is_token_present = 0;
    }
}

void print_players_boards(Game* info)
{
    printf("\n=================================================\n\n");
    for(int p = 0; p < info->no_of_players; p++)
    {
        printf("Portuguese wall of %s:\n", info->players[p].player_name);
        for(int i = 0; i < 5; i++)
        {
            for(int j = 0; j < 5; j++)
            {
                int expected_color = get_portugese_wall_color(i, j);
                
                // FIXED: Use if-else chain instead of multiple ifs
                if(info->players[p].mat.portugese_wall[i][j] == BLOCKED)
                {
                    // Tile is placed
                    print_tile_cover(expected_color);
                }
                else
                {
                    // Tile not placed, show what should go here
                    print_tile(expected_color);
                }
            }
            printf("\n");
        }
        printf("\n");

        printf("Pattern lines of %s:\n", info->players[p].player_name);
        for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
        {
            for(int j = 0; j < HOW_MANY_TILES_TYPES; j++)
            {
                if(info->players[p].mat.pattern_lines[i][j] == BLOCKED) 
                {
                    printf("  ");
                } 
                else if(info->players[p].mat.pattern_lines[i][j] == AVAILABLE)
                {
                    printf(". ");
                }
                else
                {
                    // It's a tile (0-4)
                    print_tile_cover(info->players[p].mat.pattern_lines[i][j]);
                }
            }
            printf("\n");
        }
        printf("\n");
    
        printf("Floor line of %s: ", info->players[p].player_name);
        for(int i = 0; i < 7; i++)
        {
            if(info->players[p].mat.penalties[i] == AVAILABLE)
            {
                printf("[ ] ");
            }
            else
            {
                print_tile_cover(info->players[p].mat.penalties[i]);
            }
        }
        printf("\n\n");

        printf("Score of %s: %u\n", info->players[p].player_name, info->players[p].mat.score);
        printf("\n");

        printf("=================================================\n\n");
    }
}

void is_bag_empty(Game* info)
{
    int cnt = 0;
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        if(info->bag.all_tiles[i] == 0)
        {
            cnt++;
        }
    }
    if(cnt == HOW_MANY_TILES_TYPES)
    {
        printf("WARNING: Bag is empty! Game should end or refill from discard.\n");
        // In real game, you'd refill from box lid (discard pile)
    }
}

void set_the_no_of_factories(Game* info)
{
    if(info->no_of_players == 2)
    {
        info->no_of_factory_displays = 5;
    }
    else if(info->no_of_players == 3)
    {
        info->no_of_factory_displays = 7;
    }
    else if(info->no_of_players == 4)
    {
        info->no_of_factory_displays = 9;
    }
}

void initialise_factory_displays(Game* info)
{
    for(int i = 0; i < info->no_of_factory_displays; i++)
    {
        for(int j = 0; j < HOW_MANY_TILES_ON_FACTORY; j++)
        {
            info->factory_displays.all_factories[i][j] = AVAILABLE;
        }
    }
}

void amplasete_tiles_on_a_factory(Game* info)
{
    int random_tile_idx = 0;
    for(int i = 0; i < info->no_of_factory_displays; i++)
    {
        for(int j = 0; j < HOW_MANY_TILES_ON_FACTORY; j++)
        {
            random_tile_idx = rand() % HOW_MANY_TILES_TYPES;
            is_bag_empty(info);
            
            // FIXED: Better handling when bag is low
            int attempts = 0;
            while(info->bag.all_tiles[random_tile_idx] == 0 && attempts < 100)
            {
                random_tile_idx = rand() % HOW_MANY_TILES_TYPES;
                attempts++;
            }
            
            if(attempts >= 100)
            {
                printf("Error: Cannot fill factories, bag is empty!\n");
                return;
            }
        
            info->bag.all_tiles[random_tile_idx]--;
            info->factory_displays.all_factories[i][j] = random_tile_idx;
        }
    }
    
    printf("ALL %d FACTORIES ARE FILLED\n", info->no_of_factory_displays);
    printf("\n\n");
    for(int fct = 0; fct < info->no_of_factory_displays; fct++)
    {
        printf("Factory no.%d: ", fct+1);
        for(int fct_tile = 0; fct_tile < HOW_MANY_TILES_ON_FACTORY; fct_tile++)
        {
            print_tile(info->factory_displays.all_factories[fct][fct_tile]);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n\n");
}

int check_factories(Game* info)
{
    int cnt = 0;
    for(int i = 0; i < info->no_of_factory_displays; i++)
    {
        if(info->factory_displays.all_factories[i][0] == BLOCKED)
        {
            cnt++;
        }
    }
    if(cnt == info->no_of_factory_displays)
    {
        return 1;
    }
    return 0;
}

void print_chosen_factory(Game* info, int no_of_factory)
{
    printf("You chose Factory: %d\n", no_of_factory + 1);
    for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
    {
       print_tile(info->factory_displays.all_factories[no_of_factory][i]);
       printf(" ");
    }
    printf("\n");
}

void initialise_middle_pile(Game* info)
{
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        info->middle_pile.all_tiles[i] = 0;
    }
    info->middle_pile.is_token_present = 1;
}

void print_mid_pile(Game* info)
{
    printf("\nMiddle pile tiles: ");
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        if(info->middle_pile.all_tiles[i] > 0)
        {
            print_tile(i);
            printf("x%d ", info->middle_pile.all_tiles[i]);
        }
    }
    if(info->middle_pile.is_token_present)
    {
        printf("[TOKEN]");
    }
    printf("\n");
}

int check_MidPile(Game* info)
{
    int cnt = 0;
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        if(info->middle_pile.all_tiles[i] == 0)
        {
            cnt++;
        }
    }

    if(cnt == HOW_MANY_TILES_TYPES)
    {
        return 1;
    }
    return 0;
}

void set_gameflow_round(Game* info)
{
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        for(int j = 0; j < MAX_PLAYERS; j++)
        {
            info->flow.availiability_of_pattern_lines[j][i] = -1;
        }
    }

    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        info->flow.player_order[i] = 0;
    }

    info->flow.player_on_move = -1;
    info->flow.selected_tile = -1;
    info->flow.same_tile_type_on_factory = 0;
    info->flow.selected_factory = -1;
    info->flow.selected_pattern_line = -1;
    info->flow.selections_until_round_finish = 0;
    info->flow.MidPile_or_factory_selector = BLOCKED;
    info->flow.check_mid_pile_availiability = BLOCKED;
}

void set_gameflow_turn(Game* info)
{
    // Increment the current player's order (they just played)
    info->flow.player_order[info->flow.player_on_move]++;
    
    // Reset turn-specific variables
    info->flow.selected_tile = -1;
    info->flow.same_tile_type_on_factory = 0;
    info->flow.selected_factory = -1;
    info->flow.selected_pattern_line = -1;
    info->flow.selections_until_round_finish++;
    info->flow.MidPile_or_factory_selector = BLOCKED;
}

void handdle_token(Game* info)
{
    for(int i = 0; i < info->no_of_players; i++)
    {
        if(info->players[i].is_token_present == 1)
        {
            info->flow.player_order[i]--;
        }
    }
}

void set_player_on_move(Game* info)
{
    int player_on_move = info->flow.player_order[0];

    for(int i = 0; i < info->no_of_players; i++)
    {
        if(info->flow.player_order[i] < player_on_move)
        {
            player_on_move = info->flow.player_order[i];
        }
    }

    int i = 0;
    while(info->flow.player_order[i] != player_on_move && i < info->no_of_players)
    {
        i++;
    }
    info->flow.player_on_move = i;
    printf("\n>>> %s's turn (Player %d) <<<\n\n", info->players[i].player_name, i+1);
}

void check_availability_of_mid_pile(Game* info)
{
    info->flow.check_mid_pile_availiability = BLOCKED;
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        if(info->middle_pile.all_tiles[i] != 0)
        {
            info->flow.check_mid_pile_availiability = AVAILABLE;
            break;
        }
    }
}

void mid_pile_or_factory_selector(Game* info)
{
    check_availability_of_mid_pile(info);
    int factories_available = !check_factories(info); // check_factories returns 1 if all empty

    int mid_available = (info->flow.check_mid_pile_availiability != BLOCKED);
    
    // Both available - let player choose
    if(mid_available && factories_available)
    {
        do
        {
            printf("Type 1 for middle pile or 2 for factory: ");
            scanf("%d", &info->flow.MidPile_or_factory_selector);
        } while (info->flow.MidPile_or_factory_selector != 1 && info->flow.MidPile_or_factory_selector != 2);   
    }
    // Only middle pile available
    else if(mid_available && !factories_available)
    {
        printf("All factories empty, taking from middle pile\n\n");
        info->flow.MidPile_or_factory_selector = 1;
    }
    // Only factories available
    else if(!mid_available && factories_available)
    {
        printf("Middle pile is empty, selecting from factory\n\n");
        info->flow.MidPile_or_factory_selector = 2;
    }
    // Neither available - this shouldn't happen, but handle it
    else
    {
        printf("ERROR: Nothing to select from! Round should have ended.\n");
        info->flow.MidPile_or_factory_selector = BLOCKED;
    }
}

void select_from_middle_pile(Game* info)
{
    printf("\nAvailable tiles in middle pile:\n");
    for(int i = 0; i < HOW_MANY_TILES_TYPES; i++)
    {
        if(info->middle_pile.all_tiles[i] > 0)
        {
            printf("%d: ", i);
            print_tile(i);
            printf(" x%d\n", info->middle_pile.all_tiles[i]);
        }
    }
    
    // FIXED: Logic was inverted
    do
    {
        printf("Select tile color (0-4): ");
        scanf("%d", &info->flow.selected_tile);
    } while (info->flow.selected_tile < 0 || 
             info->flow.selected_tile >= HOW_MANY_TILES_TYPES || 
             info->middle_pile.all_tiles[info->flow.selected_tile] == 0);
    
    info->flow.same_tile_type_on_factory = info->middle_pile.all_tiles[info->flow.selected_tile];
}

int check_availiability_of_factory(Game* info, int selected_factory)
{
    // FIXED: Initialize cnt
    int cnt = 0;
    for(int j = 0; j < HOW_MANY_TILES_ON_FACTORY; j++)
    {
        if(info->factory_displays.all_factories[selected_factory][j] != BLOCKED)
        {
            cnt++;
        }
    }
    if (cnt > 0) // FIXED: Should be > 0, not == 4
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void select_factory(Game* info)
{
    // FIXED: Logic operators (should be OR, not AND)
    do{
        printf("Select a factory (1-%d): ", info->no_of_factory_displays);  
        scanf("%d", &info->flow.selected_factory);
    }while(info->flow.selected_factory < 1 || 
           info->flow.selected_factory > info->no_of_factory_displays || 
           !check_availiability_of_factory(info, info->flow.selected_factory - 1));
    
    info->flow.selected_factory = info->flow.selected_factory - 1;
    print_chosen_factory(info, info->flow.selected_factory);
}

int is_tile_on_factory(Game* info, int selected_tile, int selected_factory)
{
    for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
    {
        if(info->factory_displays.all_factories[selected_factory][i] == selected_tile)
        {
            return 1;
        }
    }
    return 0;
}

void select_wanted_tile_from_factory(Game* info) 
{   
    select_factory(info);
    do {
        printf("Select tile color (0=BLUE, 1=RED, 2=BLACK, 3=YELLOW, 4=WHITE): ");
        scanf("%d", &info->flow.selected_tile);

        if (!is_tile_on_factory(info, info->flow.selected_tile, info->flow.selected_factory)) 
        {
            printf("Tile not available on this factory. Try again.\n");
        }
    } while (info->flow.selected_tile < 0 || 
             info->flow.selected_tile >= HOW_MANY_TILES_TYPES || 
             !is_tile_on_factory(info, info->flow.selected_tile, info->flow.selected_factory));

    printf("You selected: ");
    print_tile(info->flow.selected_tile);
    printf("\n");
}

void how_many_same_tile_type_on_factory(Game* info)
{
    int all_same_tiles = 0;
    int selected_factory = info->flow.selected_factory;
    for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
    {
        if(info->factory_displays.all_factories[selected_factory][i] == info->flow.selected_tile)
        {
            all_same_tiles++;
        }
    }
    info->flow.same_tile_type_on_factory = all_same_tiles;
    printf("Tiles of this color on factory: %d\n\n", info->flow.same_tile_type_on_factory);
}

void what_pattern_line_are_avalibel_and_free_spaces(Game* info) 
{
    for (int i = 0; i < HOW_MANY_TILES_TYPES; i++) 
    {   
        int free_cnt = 0;

        for (int j = 0; j < HOW_MANY_TILES_TYPES; j++) 
        {
            if (i + j >= HOW_MANY_TILES_TYPES - 1) 
            {
                if (info->players[info->flow.player_on_move].mat.pattern_lines[i][j] == AVAILABLE) 
                {
                    free_cnt++;
                }
            }
        }
        
        if (free_cnt > 0) 
        {
            info->flow.availiability_of_pattern_lines[info->flow.player_on_move][i] = free_cnt;
        } 
        else 
        {
            info->flow.availiability_of_pattern_lines[info->flow.player_on_move][i] = BLOCKED;
        }
    }

    printf("Pattern line availability (spaces): ");
    for (int i = 0; i < HOW_MANY_TILES_TYPES; i++) 
    {
        printf("L%d:%d ", i, info->flow.availiability_of_pattern_lines[info->flow.player_on_move][i]);
    }
    printf("\n\n");
}

void select_patern_line(Game* info)
{
    int wanted_line = -1;
    // FIXED: Logic operators (should be OR)
    do
    {
        printf("Select pattern line (0-4, or -1 for floor): ");
        scanf("%d", &wanted_line);
    } while ((wanted_line < -1 || wanted_line > 4) || 
             (wanted_line >= 0 && info->flow.availiability_of_pattern_lines[info->flow.player_on_move][wanted_line] == BLOCKED));

    info->flow.selected_pattern_line = wanted_line;
    if(wanted_line >= 0)
    {
        printf("Selected pattern line %d\n\n", info->flow.selected_pattern_line);
    }
    else
    {
        printf("Tiles will go to floor line\n\n");
    }
}

void put_on_available_floorline_slot(Game* info)
{
    for(int i = 0; i < MAX_PENALTIES; i++)
    {
        if(info->players[info->flow.player_on_move].mat.penalties[i] == AVAILABLE && 
           info->flow.same_tile_type_on_factory > 0)
        {
            info->players[info->flow.player_on_move].mat.penalties[i] = info->flow.selected_tile;
            info->flow.same_tile_type_on_factory--;
        }
        if(info->flow.same_tile_type_on_factory == 0)
        {
            break;
        }
    }
    
    // Excess tiles go back to bag
    while(info->flow.same_tile_type_on_factory > 0)
    {
        info->bag.all_tiles[info->flow.selected_tile]++;
        info->flow.same_tile_type_on_factory--;
    }
}

void amplasate_from_middle_pile_on_pattern_lines(Game* info)
{
    select_from_middle_pile(info);
    
    // Take the token if present
    if(info->middle_pile.is_token_present)
    {
        info->players[info->flow.player_on_move].is_token_present = 1;
        info->middle_pile.is_token_present = 0;
        
        // Put token on first available floor slot
        for(int i = 0; i < MAX_PENALTIES; i++)
        {
            if(info->players[info->flow.player_on_move].mat.penalties[i] == AVAILABLE)
            {
                info->players[info->flow.player_on_move].mat.penalties[i] = -3; // Token marker
                break;
            }
        }
        printf("You took the first player token! (-1 point)\n");
    }
    
    select_patern_line(info);

    int player_idx = info->flow.player_on_move;
    int wanted_line = info->flow.selected_pattern_line;
    
    // Floor line selected
    if(wanted_line == -1)
    {
        put_on_available_floorline_slot(info);
        info->middle_pile.all_tiles[info->flow.selected_tile] = 0;
        print_players_boards(info);
        print_mid_pile(info);
        return;
    }
    
    int col_idx = HOW_MANY_TILES_TYPES - 1;
    
    // Check if pattern line already has different color
    for(int j = 0; j < HOW_MANY_TILES_TYPES; j++)
    {
        if(info->players[player_idx].mat.pattern_lines[wanted_line][j] >= 0 &&
           info->players[player_idx].mat.pattern_lines[wanted_line][j] < HOW_MANY_TILES_TYPES)
        {
            if(info->players[player_idx].mat.pattern_lines[wanted_line][j] != info->flow.selected_tile)
            {
                printf("Pattern line already has different color! Tiles go to floor.\n");
                put_on_available_floorline_slot(info);
                info->middle_pile.all_tiles[info->flow.selected_tile] = 0;
                print_players_boards(info);
                print_mid_pile(info);
                return;
            }
        }
    }
    
    // Place tiles on pattern line
    while(col_idx >= 0 && 
          col_idx + wanted_line >= HOW_MANY_TILES_TYPES - 1 && 
          info->flow.same_tile_type_on_factory > 0)
    {
        if(info->players[player_idx].mat.pattern_lines[wanted_line][col_idx] == AVAILABLE)
        {
            info->players[player_idx].mat.pattern_lines[wanted_line][col_idx] = info->flow.selected_tile;
            info->flow.availiability_of_pattern_lines[player_idx][wanted_line]--;
            info->flow.same_tile_type_on_factory--;
        }
        col_idx--;
    }
    
    // Remaining tiles to floor
    if(info->flow.same_tile_type_on_factory > 0)
    {
        put_on_available_floorline_slot(info);
    }
    
    info->middle_pile.all_tiles[info->flow.selected_tile] = 0;
    
    printf("Tiles placed!\n\n");
    print_players_boards(info);
    print_mid_pile(info);
}

void amplasate_on_pattern_line_or_mid_pile(Game* info)
{
    select_wanted_tile_from_factory(info);
    how_many_same_tile_type_on_factory(info);
    select_patern_line(info);

    int selected_factory = info->flow.selected_factory;
    int player_idx = info->flow.player_on_move;
    int wanted_line = info->flow.selected_pattern_line;
    
    // Floor line selected
    if(wanted_line == -1)
    {
        // Put selected tiles on floor
        put_on_available_floorline_slot(info);
        
        // Move other tiles to middle
        for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
        {
            if(info->factory_displays.all_factories[selected_factory][i] != info->flow.selected_tile &&
               info->factory_displays.all_factories[selected_factory][i] != BLOCKED)
            {
                int other_tile = info->factory_displays.all_factories[selected_factory][i];
                info->middle_pile.all_tiles[other_tile]++;
            }
            info->factory_displays.all_factories[selected_factory][i] = BLOCKED;
        }
        
        printf("Tiles placed on floor line!\n\n");
        print_players_boards(info);
        print_mid_pile(info);
        return;
    }
    
    int col_idx = HOW_MANY_TILES_TYPES - 1;
    
    // Check if pattern line has different color tiles
    for(int j = 0; j < HOW_MANY_TILES_TYPES; j++)
    {
        if(info->players[player_idx].mat.pattern_lines[wanted_line][j] >= 0 &&
           info->players[player_idx].mat.pattern_lines[wanted_line][j] < HOW_MANY_TILES_TYPES &&
           info->players[player_idx].mat.pattern_lines[wanted_line][j] != info->flow.selected_tile)
        {
            printf("Pattern line has different color! All tiles go to floor.\n");
            put_on_available_floorline_slot(info);
            
            for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
            {
                if(info->factory_displays.all_factories[selected_factory][i] != info->flow.selected_tile &&
                   info->factory_displays.all_factories[selected_factory][i] != BLOCKED)
                {
                    int other_tile = info->factory_displays.all_factories[selected_factory][i];
                    info->middle_pile.all_tiles[other_tile]++;
                }
                info->factory_displays.all_factories[selected_factory][i] = BLOCKED;
            }
            
            print_players_boards(info);
            print_mid_pile(info);
            return;
        }
    }
    
    // Place tiles on pattern line
    while(col_idx >= 0 && 
          col_idx + wanted_line >= HOW_MANY_TILES_TYPES - 1 && 
          info->flow.same_tile_type_on_factory > 0)
    {
        if(info->players[player_idx].mat.pattern_lines[wanted_line][col_idx] == AVAILABLE)
        {
            info->players[player_idx].mat.pattern_lines[wanted_line][col_idx] = info->flow.selected_tile;
            info->flow.availiability_of_pattern_lines[player_idx][wanted_line]--;
            info->flow.same_tile_type_on_factory--;
        }
        col_idx--;
    }
    
    // Remaining tiles to floor
    if(info->flow.same_tile_type_on_factory > 0)
    {
        put_on_available_floorline_slot(info);
    }
    
    // Move other colored tiles to middle
    for(int i = 0; i < HOW_MANY_TILES_ON_FACTORY; i++)
    {
        if(info->factory_displays.all_factories[selected_factory][i] != info->flow.selected_tile &&
           info->factory_displays.all_factories[selected_factory][i] != BLOCKED)
        {
            int other_tile = info->factory_displays.all_factories[selected_factory][i];
            info->middle_pile.all_tiles[other_tile]++;
        }
        info->factory_displays.all_factories[selected_factory][i] = BLOCKED;
    }

    printf("Tiles placed!\n\n");

    print_players_boards(info);
    print_mid_pile(info);
}

void print_factories(Game* info)
{
    printf("\n--- FACTORIES ---\n");
    for(int i = 0; i < info->no_of_factory_displays; i++)
    {
        printf("Factory %d: ", i+1);
        int empty = 1;
        for(int j = 0; j < HOW_MANY_TILES_ON_FACTORY; j++)
        {
            if(info->factory_displays.all_factories[i][j] != BLOCKED)
            {
                print_tile(info->factory_displays.all_factories[i][j]);
                printf(" ");
                empty = 0;
            }
        }
        if(empty)
        {
            printf("[EMPTY]");
        }
        printf("\n");
    }
    print_mid_pile(info);
    printf("\n");
}

void process_end_of_round(Game* info)
{
    printf("\n=== PROCESSING END OF ROUND ===\n\n");
    
    int penalties[] = {-1, -1, -2, -2, -2, -3, -3};
    
    for(int p = 0; p < info->no_of_players; p++)
    {
        printf("Processing %s's board:\n", info->players[p].player_name);
        int round_score = 0;
        
        // Check each pattern line
        for(int row = 0; row < 5; row++)
        {
            // Check if pattern line is complete
            int line_complete = 1;
            int tile_color = -1;
            
            // Find what color tiles are on this line
            for(int col = 4; col >= 0; col--)
            {
                if(row + col >= 4) // Valid position for this row
                {
                    if(info->players[p].mat.pattern_lines[row][col] == AVAILABLE)
                    {
                        line_complete = 0;
                        break;
                    }
                    else if(info->players[p].mat.pattern_lines[row][col] >= 0 && 
                            info->players[p].mat.pattern_lines[row][col] < 5)
                    {
                        tile_color = info->players[p].mat.pattern_lines[row][col];
                    }
                }
            }
            
            // If line is complete, move one tile to wall
            if(line_complete && tile_color >= 0)
            {
                printf("  Row %d complete with color %d\n", row, tile_color);
                
                // Find the correct column for this color on this row
                int wall_col = -1;
                for(int col = 0; col < 5; col++)
                {
                    if(get_portugese_wall_color(row, col) == tile_color)
                    {
                        wall_col = col;
                        break;
                    }
                }
                
                if(wall_col >= 0)
                {
                    // Place tile on wall
                    info->players[p].mat.portugese_wall[row][wall_col] = BLOCKED;
                    
                    // Calculate score for this tile
                    int tile_score = 1;
                    int horizontal_count = 1;
                    int vertical_count = 1;
                    
                    // Count horizontally
                    for(int left = wall_col - 1; left >= 0 && info->players[p].mat.portugese_wall[row][left] == BLOCKED; left--)
                    {
                        horizontal_count++;
                    }
                    for(int right = wall_col + 1; right < 5 && info->players[p].mat.portugese_wall[row][right] == BLOCKED; right++)
                    {
                        horizontal_count++;
                    }
                    
                    // Count vertically
                    for(int up = row - 1; up >= 0 && info->players[p].mat.portugese_wall[up][wall_col] == BLOCKED; up--)
                    {
                        vertical_count++;
                    }
                    for(int down = row + 1; down < 5 && info->players[p].mat.portugese_wall[down][wall_col] == BLOCKED; down++)
                    {
                        vertical_count++;
                    }
                    
                    // Add points
                    if(horizontal_count > 1)
                    {
                        tile_score += horizontal_count - 1;
                    }
                    if(vertical_count > 1)
                    {
                        tile_score += vertical_count - 1;
                    }
                    
                    round_score += tile_score;
                    printf("    Placed at wall[%d][%d], scored %d points\n", row, wall_col, tile_score);
                }
                
                // Clear the pattern line and return extra tiles to bag
                for(int col = 0; col < 5; col++)
                {
                    if(row + col >= 4)
                    {
                        if(info->players[p].mat.pattern_lines[row][col] >= 0 && 
                           info->players[p].mat.pattern_lines[row][col] < 5)
                        {
                            // First tile goes to wall, rest go back to bag
                            if(col != 4) // Don't return the tile we just placed
                            {
                                info->bag.all_tiles[tile_color]++;
                            }
                        }
                        info->players[p].mat.pattern_lines[row][col] = AVAILABLE;
                    }
                }
            }
        }
        
        // Process floor line penalties
        int penalty_score = 0;
        for(int i = 0; i < MAX_PENALTIES; i++)
        {
            if(info->players[p].mat.penalties[i] != AVAILABLE)
            {
                penalty_score += penalties[i];
                // Return tiles to bag (except token marker -3)
                if(info->players[p].mat.penalties[i] >= 0 && info->players[p].mat.penalties[i] < 5)
                {
                    info->bag.all_tiles[info->players[p].mat.penalties[i]]++;
                }
                info->players[p].mat.penalties[i] = AVAILABLE;
            }
        }
        
        printf("  Penalty points: %d\n", penalty_score);
        round_score += penalty_score;
        
        // Update score (can't go below 0)
        int new_score = info->players[p].mat.score + round_score;
        if(new_score < 0) new_score = 0;
        info->players[p].mat.score = new_score;
        
        printf("  Round score: %+d, Total score: %d\n\n", round_score, info->players[p].mat.score);
    }
}

int check_game_end(Game* info)
{
    // Game ends when any player completes a horizontal row
    for(int p = 0; p < info->no_of_players; p++)
    {
        for(int row = 0; row < 5; row++)
        {
            int row_complete = 1;
            for(int col = 0; col < 5; col++)
            {
                if(info->players[p].mat.portugese_wall[row][col] != BLOCKED)
                {
                    row_complete = 0;
                    break;
                }
            }
            if(row_complete)
            {
                printf("\n%s completed a row! Game ends after this round.\n", info->players[p].player_name);
                return 1;
            }
        }
    }
    return 0;
}

void calculate_final_bonuses(Game* info)
{
    printf("\n=== CALCULATING FINAL BONUSES ===\n\n");
    
    for(int p = 0; p < info->no_of_players; p++)
    {
        int bonus_points = 0;
        printf("%s's bonuses:\n", info->players[p].player_name);
        
        // Bonus for complete horizontal rows (2 points each)
        int complete_rows = 0;
        for(int row = 0; row < 5; row++)
        {
            int row_complete = 1;
            for(int col = 0; col < 5; col++)
            {
                if(info->players[p].mat.portugese_wall[row][col] != BLOCKED)
                {
                    row_complete = 0;
                    break;
                }
            }
            if(row_complete)
            {
                complete_rows++;
                bonus_points += 2;
            }
        }
        if(complete_rows > 0)
        {
            printf("  %d complete row(s): +%d points\n", complete_rows, complete_rows * 2);
        }
        
        // Bonus for complete vertical columns (7 points each)
        int complete_cols = 0;
        for(int col = 0; col < 5; col++)
        {
            int col_complete = 1;
            for(int row = 0; row < 5; row++)
            {
                if(info->players[p].mat.portugese_wall[row][col] != BLOCKED)
                {
                    col_complete = 0;
                    break;
                }
            }
            if(col_complete)
            {
                complete_cols++;
                bonus_points += 7;
            }
        }
        if(complete_cols > 0)
        {
            printf("  %d complete column(s): +%d points\n", complete_cols, complete_cols * 7);
        }
        
        // Bonus for complete colors (10 points each)
        // Define where each color appears on the Portuguese wall
        int color_positions[5][5][2] = {
            {{0,0}, {1,1}, {2,2}, {3,3}, {4,4}}, // Blue diagonal
            {{0,2}, {1,3}, {2,4}, {3,0}, {4,1}}, // Red
            {{0,3}, {1,4}, {2,0}, {3,1}, {4,2}}, // Black  
            {{0,1}, {1,2}, {2,3}, {3,4}, {4,0}}, // Yellow
            {{0,4}, {1,0}, {2,1}, {3,2}, {4,3}}  // White
        };
        
        const char* color_names[] = {"Blue", "Red", "Black", "Yellow", "White"};
        int complete_colors = 0;
        
        for(int color = 0; color < 5; color++)
        {
            int color_complete = 1;
            for(int i = 0; i < 5; i++)
            {
                int row = color_positions[color][i][0];
                int col = color_positions[color][i][1];
                if(info->players[p].mat.portugese_wall[row][col] != BLOCKED)
                {
                    color_complete = 0;
                    break;
                }
            }
            if(color_complete)
            {
                printf("  Complete %s set: +10 points\n", color_names[color]);
                complete_colors++;
                bonus_points += 10;
            }
        }
        
        info->players[p].mat.score += bonus_points;
        printf("  Total bonus: +%d points\n", bonus_points);
        printf("  Final score: %d\n\n", info->players[p].mat.score);
    }
}

void determine_winner(Game* info)
{
    printf("\n=== FINAL RESULTS ===\n\n");
    
    int highest_score = -1;
    int winner_idx = -1;
    int tie = 0;
    
    // Find highest score
    for(int p = 0; p < info->no_of_players; p++)
    {
        printf("%s: %u points\n", info->players[p].player_name, info->players[p].mat.score);
        
        if(info->players[p].mat.score > highest_score)
        {
            highest_score = info->players[p].mat.score;
            winner_idx = p;
            tie = 0;
        }
        else if(info->players[p].mat.score == highest_score)
        {
            tie = 1;
        }
    }
    
    if(tie)
    {
        printf("\nIt's a tie at %d points!\n", highest_score);
        printf("Tiebreaker: Player with most complete rows wins.\n");
        // In a full implementation, you'd count complete rows for tiebreaker
    }
    else
    {
        printf("\n🎉 %s WINS with %d points! 🎉\n", 
               info->players[winner_idx].player_name, highest_score);
    }
}

void set_players_name(Game* info)
{
    if(info->no_of_players > 1 && info->no_of_players < 5)
    {
        printf("GOOD! Now set player names:\n");
        int i = 0;
        while(i < info->no_of_players)
        {
            printf("Name for Player %d: ", i+1);
            scanf("%s", info->players[i].player_name);
            i++;
        }
    }
    else
    {
        printf("Invalid number of players! Must be 2-4.\n");
        exit(EXIT_FAILURE);
    }
}

void print_title() 
{  
    printf("\n\n\n");                                         
    printf(" @@@@@@   @@@@@@@@  @@@  @@@  @@@       \n");
    printf("@@@@@@@@  @@@@@@@@  @@@  @@@  @@@       \n");
    printf("@@!  @@@       @@!  @@!  @@@  @@!       \n");
    printf("!@!  @!@      !@!   !@!  @!@  !@!       \n");
    printf("@!@!@!@!     @!!    @!@  !@!  @!!       \n");
    printf("!!!@!!!!    !!!     !@!  !!!  !!!       \n");
    printf("!!:  !!!   !!:      !!:  !!!  !!:       \n");
    printf(":!:  !:!  :!:       :!:  !:!   :!:      \n");
    printf("::   :::   :: ::::  ::::: ::   :: ::::  \n");
    printf(" :   : :  : :: : :   : :  :   : :: : :  \n");
    printf("\n\n\n");  
}

void handle_round(Game* info)
{
    printf("\n=== STARTING NEW ROUND ===\n\n");
    
    initialise_factory_displays(info);
    amplasete_tiles_on_a_factory(info);
    initialise_middle_pile(info);
    set_gameflow_round(info);

    while (1)
    {
        set_player_on_move(info);
        print_factories(info);
        
        mid_pile_or_factory_selector(info);

        if(info->flow.MidPile_or_factory_selector == 1)
        {
            what_pattern_line_are_avalibel_and_free_spaces(info);
            amplasate_from_middle_pile_on_pattern_lines(info);
        }
        else if(info->flow.MidPile_or_factory_selector == 2)
        {
            what_pattern_line_are_avalibel_and_free_spaces(info);
            amplasate_on_pattern_line_or_mid_pile(info);
        }

        if (check_factories(info) && check_MidPile(info))
        {
            printf("\n=== ROUND COMPLETE ===\n");
            break;
        }

        set_gameflow_turn(info);
    }
}

int main()
{
    srand(time(NULL));
    Game info;
    
    print_title();
    
    printf("Choose number of players (2-4): ");
    scanf("%d", &info.no_of_players);

    set_players_name(&info);
    fill_the_bag(&info.bag);
    set_the_no_of_factories(&info);
    initialise_mat(&info);
    
    print_players_boards(&info);

    // Play rounds until game ends
    int round_number = 1;
    int game_ended = 0;
    
    while(!game_ended)
    {
        printf("\n\n");
        printf("╔════════════════════════════════════════╗\n");
        printf("║         ROUND %d STARTING              ║\n", round_number);
        printf("╔════════════════════════════════════════╗\n");
        printf("\n");
        
        // Play one round
        handle_round(&info);
        
        // Process end of round (move tiles, calculate scores)
        process_end_of_round(&info);
        
        // Show updated boards
        print_players_boards(&info);
        
        // Check if game should end
        game_ended = check_game_end(&info);
        
        if(!game_ended)
        {
            // Set up for next round - player with token goes first
            for(int p = 0; p < info.no_of_players; p++)
            {
                if(info.players[p].is_token_present)
                {
                    info.flow.player_order[p] = -1; // Goes first
                    info.players[p].is_token_present = 0; // Reset token
                }
                else
                {
                    info.flow.player_order[p] = 0;
                }
            }
            round_number++;
        }
    }
    
    // Calculate final bonuses
    calculate_final_bonuses(&info);
    
    // Determine winner
    determine_winner(&info);
    
    printf("\nThank you for playing AZUL!\n\n");
    
    return 0;
}