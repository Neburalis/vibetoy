#include "menu.h"
#include "game.h"
#include "game_manager.h"
#include "games/tictactoe.h"

void init_application(ApplicationState* app) {
    app->current_state = STATE_MAIN_MENU;
    app->menu_selection = 0;
    app->mode_selection = 0;
    app->difficulty_selection = 0;
    app->game_mode = MODE_TWO_PLAYER;
    app->ai_difficulty = DIFFICULTY_MEDIUM;
    app->human_player = CELL_X;
    app->ai_player = CELL_O;
    app->ai_thinking = false;
    app->winner = CELL_EMPTY;
    app->is_draw = false;
    app->has_active_game = false;
    
    // Initialize game structure but don't activate it
    app->game.cursor_x = 1;
    app->game.cursor_y = 1;
    app->game.current_player = CELL_X;
    app->game.game_active = false;  // Keep inactive until game starts
    reset_board(&app->game);
    
    // Initialize global cursor
    init_global_cursor(&app->cursor);
    
    // Initialize AI state
    init_ai_state(&app->ai_state);
    
    // Initialize random seed for AI
    srand((unsigned int)time(NULL));
}

void handle_menu_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            handle_cursor_click(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '\n':
            handle_cursor_click(app);
            break;
            
        case 'n':
        case 'N':
            app->current_state = STATE_MODE_SELECTION;
            app->mode_selection = 0;
            break;
            
        case 'c':
        case 'C':
            if (app->has_active_game) {
                // Check if game is actually finished
                if (!app->game.game_active && (app->winner != CELL_EMPTY || app->is_draw)) {
                    // Game was finished, go to game over state
                    app->current_state = STATE_GAME_OVER;
                } else {
                    // Game is still active, continue playing
                    app->current_state = STATE_PLAYING;
                }
            }
            break;
            
        case 'q':
        case 'Q':
            app->current_state = STATE_QUIT;
            break;
    }
}

void handle_game_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    // Skip input processing during AI turn in single player mode
    if (app->game_mode == MODE_SINGLE_PLAYER && 
        app->game.current_player == app->ai_player && 
        app->game.game_active) {
        return;
    }
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            handle_cursor_click(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '\n':
            handle_cursor_click(app);
            break;
            
        case 'r':
        case 'R':
            if (app->game_mode == MODE_SINGLE_PLAYER) {
                start_single_player_game(app);
            } else {
                reset_board(&app->game);
                app->game.game_active = true;
            }
            break;
            
        case 'm':
        case 'M':
            app->current_state = STATE_MAIN_MENU;
            app->menu_selection = 0;
            break;
            
        case 'q':
        case 'Q':
            app->current_state = STATE_QUIT;
            break;
    }
}

void handle_game_over_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            handle_cursor_click(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '\n':
            handle_cursor_click(app);
            break;
            
        case 'r':
        case 'R':
            // Restart the game
            reset_board(&app->game);
            app->game.game_active = true;
            app->current_state = STATE_PLAYING;
            app->winner = CELL_EMPTY;
            app->is_draw = false;
            app->has_active_game = true;
            break;
            
        case 'm':
        case 'M':
            // Return to main menu
            app->current_state = STATE_MAIN_MENU;
            app->menu_selection = 0;
            break;
            
        case 'q':
        case 'Q':
            // Quit the game
            app->current_state = STATE_QUIT;
            break;
    }
}

void handle_mode_selection_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            handle_cursor_click(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '\n':
            handle_cursor_click(app);
            break;
            
        case '1':
            app->game_mode = MODE_TWO_PLAYER;
            reset_board(&app->game);
            app->game.game_active = true;
            app->current_state = STATE_PLAYING;
            app->has_active_game = true;
            app->winner = CELL_EMPTY;
            app->is_draw = false;
            break;
            
        case '2':
            app->game_mode = MODE_SINGLE_PLAYER;
            app->current_state = STATE_DIFFICULTY_SELECTION;
            app->difficulty_selection = 0;
            break;
            
        case 'b':
        case 'B':
            app->current_state = STATE_MAIN_MENU;
            app->menu_selection = 0;
            break;
            
        case 'q':
        case 'Q':
            app->current_state = STATE_QUIT;
            break;
    }
}

void handle_difficulty_selection_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            handle_cursor_click(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '\n':
            handle_cursor_click(app);
            break;
            
        case '1':
            app->ai_difficulty = DIFFICULTY_EASY;
            start_single_player_game(app);
            break;
            
        case '2':
            app->ai_difficulty = DIFFICULTY_MEDIUM;
            start_single_player_game(app);
            break;
            
        case '3':
            app->ai_difficulty = DIFFICULTY_HARD;
            start_single_player_game(app);
            break;
            
        case 'b':
        case 'B':
            app->current_state = STATE_MODE_SELECTION;
            app->mode_selection = 0;
            break;
            
        case 'q':
        case 'Q':
            app->current_state = STATE_QUIT;
            break;
    }
}

void start_single_player_game(ApplicationState* app) {
    // Let human choose to be X or O (X goes first)
    app->human_player = CELL_X;  // Human starts as X
    app->ai_player = CELL_O;     // AI plays as O
    
    reset_board(&app->game);
    app->game.game_active = true;
    app->current_state = STATE_PLAYING;
    app->has_active_game = true;
    app->winner = CELL_EMPTY;
    app->is_draw = false;
    app->ai_thinking = false;
    
    // Reset AI state
    init_ai_state(&app->ai_state);
}

// New architecture integration functions
void handle_game_selection_input(ApplicationState* app, const struct tb_event* event) {
    if (event->type != TB_EVENT_KEY) return;
    
    switch (event->key) {
        case TB_KEY_ARROW_UP:
            move_global_cursor(app, 0, -1);
            break;
            
        case TB_KEY_ARROW_DOWN:
            move_global_cursor(app, 0, 1);
            break;
            
        case TB_KEY_ARROW_LEFT:
            move_global_cursor(app, -1, 0);
            break;
            
        case TB_KEY_ARROW_RIGHT:
            move_global_cursor(app, 1, 0);
            break;
            
        case TB_KEY_ENTER:
            setup_game_from_selection(app);
            break;
            
        case TB_KEY_ESC:
            transition_to_main_menu(app);
            break;
    }
    
    // Handle character input
    switch (event->ch) {
        case 'w':
        case 'W':
            move_global_cursor(app, 0, -1);
            break;
            
        case 's':
        case 'S':
            move_global_cursor(app, 0, 1);
            break;
            
        case 'a':
        case 'A':
            move_global_cursor(app, -1, 0);
            break;
            
        case 'd':
        case 'D':
            move_global_cursor(app, 1, 0);
            break;
            
        case '1':
            app->game_selection = 0; // TicTacToe
            setup_game_from_selection(app);
            break;
            
        case '2':
            app->game_selection = 1; // Future: Tetris
            setup_game_from_selection(app);
            break;
            
        case '3':
            app->game_selection = 2; // Future: Snake
            setup_game_from_selection(app);
            break;
            
        case 'q':
        case 'Q':
            app->current_state = STATE_QUIT;
            break;
    }
}

void setup_game_from_selection(ApplicationState* app) {
    GameType selected_game = (GameType)app->game_selection;
    
    // Load the selected game
    if (load_selected_game(app, selected_game)) {
        // For TicTacToe, go to mode selection
        if (selected_game == GAME_TYPE_TICTACTOE) {
            app->current_state = STATE_MODE_SELECTION;
            app->mode_selection = 0;
        } else {
            // For other games, go directly to playing
            transition_to_playing(app);
        }
    } else {
        // Failed to load game, go back to main menu
        transition_to_main_menu(app);
    }
}