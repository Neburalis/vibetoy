#include "game.h"
#include "menu.h"
#include "../lib/termbox2/termbox2.h"
#include <stdlib.h>
#include <time.h>

void init_game(GameState* game) {
    game->cursor_x = 1;
    game->cursor_y = 1;
    game->current_player = CELL_X;
    game->game_active = true;
    reset_board(game);
}

void reset_board(GameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            game->board[i][j] = CELL_EMPTY;
        }
    }
    game->current_player = CELL_X;
    // Don't automatically set game_active here - let caller decide
}

bool is_valid_move(const GameState* game, int x, int y) {
    if (x < 0 || x >= 3 || y < 0 || y >= 3) {
        return false;
    }
    if (!game->game_active) {
        return false;
    }
    return game->board[y][x] == CELL_EMPTY;
}

bool make_move(GameState* game, int x, int y) {
    if (!is_valid_move(game, x, y)) {
        return false;
    }
    
    game->board[y][x] = game->current_player;
    
    CellState winner = check_winner(game);
    if (winner != CELL_EMPTY || is_board_full(game)) {
        game->game_active = false;
    } else {
        switch_player(game);
    }
    
    return true;
}

CellState check_winner(const GameState* game) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] != CELL_EMPTY &&
            game->board[i][0] == game->board[i][1] &&
            game->board[i][1] == game->board[i][2]) {
            return game->board[i][0];
        }
    }
    
    // Check columns
    for (int j = 0; j < 3; j++) {
        if (game->board[0][j] != CELL_EMPTY &&
            game->board[0][j] == game->board[1][j] &&
            game->board[1][j] == game->board[2][j]) {
            return game->board[0][j];
        }
    }
    
    // Check diagonals
    if (game->board[0][0] != CELL_EMPTY &&
        game->board[0][0] == game->board[1][1] &&
        game->board[1][1] == game->board[2][2]) {
        return game->board[0][0];
    }
    
    if (game->board[0][2] != CELL_EMPTY &&
        game->board[0][2] == game->board[1][1] &&
        game->board[1][1] == game->board[2][0]) {
        return game->board[0][2];
    }
    
    return CELL_EMPTY;
}

bool is_board_full(const GameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == CELL_EMPTY) {
                return false;
            }
        }
    }
    return true;
}

void switch_player(GameState* game) {
    game->current_player = (game->current_player == CELL_X) ? CELL_O : CELL_X;
}

void move_cursor(GameState* game, int dx, int dy) {
    int new_x = game->cursor_x + dx;
    int new_y = game->cursor_y + dy;
    
    // Keep cursor within bounds
    if (new_x >= 0 && new_x < 3) {
        game->cursor_x = new_x;
    }
    if (new_y >= 0 && new_y < 3) {
        game->cursor_y = new_y;
    }
}

// Global cursor functions
void init_global_cursor(GlobalCursor* cursor) {
    cursor->screen_x = tb_width() / 2;
    cursor->screen_y = tb_height() / 2;
    cursor->hovered_menu_item = -1;
    cursor->hovered_game_cell_x = -1;
    cursor->hovered_game_cell_y = -1;
    cursor->hovered_game_over_option = false;
    cursor->game_over_option_index = -1;
}

void move_global_cursor(ApplicationState* app, int dx, int dy) {
    int new_x = app->cursor.screen_x + dx;
    int new_y = app->cursor.screen_y + dy;
    
    // Keep cursor within screen bounds
    if (new_x >= 0 && new_x < tb_width()) {
        app->cursor.screen_x = new_x;
    }
    if (new_y >= 0 && new_y < tb_height()) {
        app->cursor.screen_y = new_y;
    }
    
    // Update hover state after moving
    update_hover_state(app);
}

void set_cursor_position(ApplicationState* app, int x, int y) {
    // Set cursor to absolute position (for mouse input)
    if (x >= 0 && x < tb_width()) {
        app->cursor.screen_x = x;
    }
    if (y >= 0 && y < tb_height()) {
        app->cursor.screen_y = y;
    }
    
    // Update hover state after positioning
    update_hover_state(app);
}

void update_hover_state(ApplicationState* app) {
    // Reset all hover states
    app->cursor.hovered_menu_item = -1;
    app->cursor.hovered_game_cell_x = -1;
    app->cursor.hovered_game_cell_y = -1;
    app->cursor.hovered_game_over_option = false;
    app->cursor.game_over_option_index = -1;
    app->cursor.hovered_mode_selection = -1;
    app->cursor.hovered_difficulty_selection = -1;
    
    int x = app->cursor.screen_x;
    int y = app->cursor.screen_y;
    
    if (app->current_state == STATE_MAIN_MENU) {
        // Check menu items (positioned at center of screen)
        int x_center = tb_width() / 2;
        int menu_start_y = 9; // Adjusted for menu position
        
        // Menu item areas (approximate)
        if (x >= x_center - 10 && x <= x_center + 10) {
            if (y == menu_start_y) {
                app->cursor.hovered_menu_item = 0; // New Game
            } else if (app->has_active_game && y == menu_start_y + 1) {
                app->cursor.hovered_menu_item = 1; // Continue
            } else if (y == menu_start_y + (app->has_active_game ? 2 : 1)) {
                app->cursor.hovered_menu_item = app->has_active_game ? 2 : 1; // Quit
            }
        }
    } else if (app->current_state == STATE_MODE_SELECTION) {
        // Check mode selection items
        int x_center = tb_width() / 2;
        int menu_start_y = 7; // Adjusted for mode selection position
        
        if (x >= x_center - 15 && x <= x_center + 15) {
            if (y == menu_start_y) {
                app->cursor.hovered_mode_selection = 0; // Two Player
            } else if (y == menu_start_y + 1) {
                app->cursor.hovered_mode_selection = 1; // Single Player
            } else if (y == menu_start_y + 2) {
                app->cursor.hovered_mode_selection = 2; // Back
            }
        }
    } else if (app->current_state == STATE_DIFFICULTY_SELECTION) {
        // Check difficulty selection items
        int x_center = tb_width() / 2;
        int menu_start_y = 7; // Adjusted for difficulty selection position
        
        if (x >= x_center - 15 && x <= x_center + 15) {
            if (y == menu_start_y) {
                app->cursor.hovered_difficulty_selection = 0; // Easy
            } else if (y == menu_start_y + 1) {
                app->cursor.hovered_difficulty_selection = 1; // Medium
            } else if (y == menu_start_y + 2) {
                app->cursor.hovered_difficulty_selection = 2; // Hard
            } else if (y == menu_start_y + 3) {
                app->cursor.hovered_difficulty_selection = 3; // Back
            }
        }
    } else if (app->current_state == STATE_PLAYING) {
        // Check game board cells
        int start_x = tb_width() / 2 - 6;
        int start_y = 8;
        
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                int cell_x = start_x + 2 + col * 4;
                int cell_y = start_y + 1 + row * 2;
                
                // Check if cursor is over this cell (with some tolerance)
                if (x >= cell_x - 1 && x <= cell_x + 1 && y == cell_y) {
                    app->cursor.hovered_game_cell_x = col;
                    app->cursor.hovered_game_cell_y = row;
                    break;
                }
            }
        }
    } else if (app->current_state == STATE_GAME_OVER) {
        // Check game over options
        int x_center = tb_width() / 2;
        int options_start_y = 19; // Approximate position of game over options
        
        if (x >= x_center - 10 && x <= x_center + 10) {
            if (y == options_start_y) {
                app->cursor.hovered_game_over_option = true;
                app->cursor.game_over_option_index = 0; // Restart
            } else if (y == options_start_y + 1) {
                app->cursor.hovered_game_over_option = true;
                app->cursor.game_over_option_index = 1; // Main Menu
            } else if (y == options_start_y + 2) {
                app->cursor.hovered_game_over_option = true;
                app->cursor.game_over_option_index = 2; // Quit
            }
        }
    }
}

// AI Implementation Functions

void init_ai_state(AIState* ai_state) {
    ai_state->ai_move_in_progress = false;
    ai_state->ai_last_move_x = -1;
    ai_state->ai_last_move_y = -1;
    ai_state->ai_thinking_start_time = 0.0;
    ai_state->ai_evaluation_calls = 0;
}

void get_available_moves(const GameState* game, int* moves, int* move_count) {
    *move_count = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (game->board[y][x] == CELL_EMPTY) {
                moves[(*move_count) * 2] = x;
                moves[(*move_count) * 2 + 1] = y;
                (*move_count)++;
            }
        }
    }
}

bool simulate_move(GameState* game, int x, int y, CellState player) {
    if (x < 0 || x >= 3 || y < 0 || y >= 3 || game->board[y][x] != CELL_EMPTY) {
        return false;
    }
    game->board[y][x] = player;
    return true;
}

void undo_move(GameState* game, int x, int y) {
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
        game->board[y][x] = CELL_EMPTY;
    }
}

int evaluate_game_state(const GameState* game, CellState ai_player, CellState human_player) {
    CellState winner = check_winner(game);
    
    if (winner == ai_player) {
        return 10;
    } else if (winner == human_player) {
        return -10;
    } else {
        return 0;
    }
}

int apply_positional_bonus(int x, int y) {
    // Center position gets highest bonus
    if (x == 1 && y == 1) {
        return 3;
    }
    // Corners get medium bonus
    if ((x == 0 || x == 2) && (y == 0 || y == 2)) {
        return 2;
    }
    // Edges get lowest bonus
    return 1;
}

int evaluate_strategic_positions(const GameState* game, CellState player) {
    int score = 0;
    
    // Bonus for center control
    if (game->board[1][1] == player) {
        score += 3;
    }
    
    // Bonus for corners
    if (game->board[0][0] == player) score += 2;
    if (game->board[0][2] == player) score += 2;
    if (game->board[2][0] == player) score += 2;
    if (game->board[2][2] == player) score += 2;
    
    return score;
}

int get_opening_move(const GameState* game, CellState ai_player) {
    // Suppress unused parameter warning
    (void)ai_player;
    
    int move_count = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (game->board[y][x] != CELL_EMPTY) {
                move_count++;
            }
        }
    }
    
    // First move - take center if available, otherwise corner
    if (move_count == 0) {
        return 1 * 3 + 1; // Center (1,1)
    }
    
    // Second move logic
    if (move_count == 1) {
        // If center is taken by opponent, take a corner
        if (game->board[1][1] != CELL_EMPTY) {
            return 0 * 3 + 0; // Top-left corner
        } else {
            // If opponent took a corner, take center
            return 1 * 3 + 1; // Center
        }
    }
    
    return -1; // No opening move available
}

int minimax_alpha_beta(GameState* game, int depth, int alpha, int beta, 
                      bool maximizing, CellState ai_player, CellState human_player) {
    // Check if game is terminal
    CellState winner = check_winner(game);
    if (winner != CELL_EMPTY) {
        int score = evaluate_game_state(game, ai_player, human_player);
        return score - depth; // Prefer immediate wins
    }
    
    if (is_board_full(game)) {
        return 0; // Draw
    }
    
    if (depth >= 9) { // Maximum search depth
        return evaluate_strategic_positions(game, ai_player) - 
               evaluate_strategic_positions(game, human_player);
    }
    
    int moves[18]; // Max 9 moves * 2 coordinates
    int move_count;
    get_available_moves(game, moves, &move_count);
    
    if (maximizing) {
        int max_eval = -1000;
        for (int i = 0; i < move_count; i++) {
            int x = moves[i * 2];
            int y = moves[i * 2 + 1];
            
            simulate_move(game, x, y, ai_player);
            int eval = minimax_alpha_beta(game, depth + 1, alpha, beta, false, ai_player, human_player);
            undo_move(game, x, y);
            
            max_eval = (eval > max_eval) ? eval : max_eval;
            alpha = (alpha > eval) ? alpha : eval;
            
            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        return max_eval;
    } else {
        int min_eval = 1000;
        for (int i = 0; i < move_count; i++) {
            int x = moves[i * 2];
            int y = moves[i * 2 + 1];
            
            simulate_move(game, x, y, human_player);
            int eval = minimax_alpha_beta(game, depth + 1, alpha, beta, true, ai_player, human_player);
            undo_move(game, x, y);
            
            min_eval = (eval < min_eval) ? eval : min_eval;
            beta = (beta < eval) ? beta : eval;
            
            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        return min_eval;
    }
}

int apply_difficulty_filter(int best_move, int* move_scores, int move_count, AIDifficulty difficulty) {
    if (difficulty == DIFFICULTY_HARD) {
        return best_move; // Always play optimally
    }
    
    if (difficulty == DIFFICULTY_EASY) {
        // 30% chance to play randomly
        if ((rand() % 100) < 30) {
            return rand() % move_count;
        }
    } else if (difficulty == DIFFICULTY_MEDIUM) {
        // 10% chance to play suboptimally
        if ((rand() % 100) < 10) {
            // Find second best move
            int second_best = best_move;
            int best_score = move_scores[best_move];
            for (int i = 0; i < move_count; i++) {
                if (i != best_move && move_scores[i] < best_score) {
                    second_best = i;
                    break;
                }
            }
            return second_best;
        }
    }
    
    return best_move;
}

int get_ai_move(const GameState* game, CellState ai_player, AIDifficulty difficulty) {
    CellState human_player = (ai_player == CELL_X) ? CELL_O : CELL_X;
    
    // Check for opening moves first
    int opening_move = get_opening_move(game, ai_player);
    if (opening_move >= 0 && difficulty == DIFFICULTY_HARD) {
        int x = opening_move % 3;
        int y = opening_move / 3;
        if (is_valid_move(game, x, y)) {
            return opening_move;
        }
    }
    
    // Get available moves
    int moves[18];
    int move_count;
    get_available_moves(game, moves, &move_count);
    
    if (move_count == 0) {
        return -1; // No moves available
    }
    
    // Easy difficulty - sometimes play randomly
    if (difficulty == DIFFICULTY_EASY && (rand() % 100) < 40) {
        int random_index = rand() % move_count;
        return moves[random_index * 2 + 1] * 3 + moves[random_index * 2];
    }
    
    // Evaluate all moves using minimax
    int best_move = 0;
    int best_score = -1000;
    int move_scores[9];
    
    for (int i = 0; i < move_count; i++) {
        int x = moves[i * 2];
        int y = moves[i * 2 + 1];
        
        GameState temp_game = *game;
        simulate_move(&temp_game, x, y, ai_player);
        
        int score = minimax_alpha_beta(&temp_game, 0, -1000, 1000, false, ai_player, human_player);
        move_scores[i] = score;
        
        if (score > best_score) {
            best_score = score;
            best_move = i;
        }
    }
    
    // Apply difficulty filter
    best_move = apply_difficulty_filter(best_move, move_scores, move_count, difficulty);
    
    int x = moves[best_move * 2];
    int y = moves[best_move * 2 + 1];
    
    return y * 3 + x; // Convert to single index
}

void trigger_ai_move(ApplicationState* app) {
    if (app->game_mode != MODE_SINGLE_PLAYER || app->game.current_player != app->ai_player) {
        return;
    }
    
    app->ai_thinking = true;
    app->ai_state.ai_move_in_progress = true;
}

void process_ai_turn(ApplicationState* app) {
    if (!app->ai_thinking || app->game_mode != MODE_SINGLE_PLAYER) {
        return;
    }
    
    int move_index = get_ai_move(&app->game, app->ai_player, app->ai_difficulty);
    
    if (move_index >= 0) {
        int x = move_index % 3;
        int y = move_index / 3;
        
        if (make_move(&app->game, x, y)) {
            app->ai_state.ai_last_move_x = x;
            app->ai_state.ai_last_move_y = y;
            
            if (!app->game.game_active) {
                app->winner = check_winner(&app->game);
                app->is_draw = (app->winner == CELL_EMPTY);
                app->current_state = STATE_GAME_OVER;
            }
        }
    }
    
    app->ai_thinking = false;
    app->ai_state.ai_move_in_progress = false;
}

void handle_cursor_click(ApplicationState* app) {
    if (app->current_state == STATE_MAIN_MENU && app->cursor.hovered_menu_item >= 0) {
        app->menu_selection = app->cursor.hovered_menu_item;
        // Navigate to mode selection for New Game
        if (app->has_active_game) {
            switch (app->cursor.hovered_menu_item) {
                case 0: // New Game
                    app->current_state = STATE_MODE_SELECTION;
                    app->mode_selection = 0;
                    break;
                case 1: // Continue
                    if (!app->game.game_active && (app->winner != CELL_EMPTY || app->is_draw)) {
                        app->current_state = STATE_GAME_OVER;
                    } else {
                        app->current_state = STATE_PLAYING;
                    }
                    break;
                case 2: // Quit
                    app->current_state = STATE_QUIT;
                    break;
            }
        } else {
            switch (app->cursor.hovered_menu_item) {
                case 0: // New Game
                    app->current_state = STATE_MODE_SELECTION;
                    app->mode_selection = 0;
                    break;
                case 1: // Quit
                    app->current_state = STATE_QUIT;
                    break;
            }
        }
    } else if (app->current_state == STATE_MODE_SELECTION && app->cursor.hovered_mode_selection >= 0) {
        switch (app->cursor.hovered_mode_selection) {
            case 0: // Two Player
                app->game_mode = MODE_TWO_PLAYER;
                reset_board(&app->game);
                app->game.game_active = true;
                app->current_state = STATE_PLAYING;
                app->has_active_game = true;
                app->winner = CELL_EMPTY;
                app->is_draw = false;
                break;
            case 1: // Single Player
                app->game_mode = MODE_SINGLE_PLAYER;
                app->current_state = STATE_DIFFICULTY_SELECTION;
                app->difficulty_selection = 0;
                break;
            case 2: // Back
                app->current_state = STATE_MAIN_MENU;
                app->menu_selection = 0;
                break;
        }
    } else if (app->current_state == STATE_DIFFICULTY_SELECTION && app->cursor.hovered_difficulty_selection >= 0) {
        switch (app->cursor.hovered_difficulty_selection) {
            case 0: // Easy
                app->ai_difficulty = DIFFICULTY_EASY;
                start_single_player_game(app);
                break;
            case 1: // Medium
                app->ai_difficulty = DIFFICULTY_MEDIUM;
                start_single_player_game(app);
                break;
            case 2: // Hard
                app->ai_difficulty = DIFFICULTY_HARD;
                start_single_player_game(app);
                break;
            case 3: // Back
                app->current_state = STATE_MODE_SELECTION;
                app->mode_selection = 0;
                break;
        }
    } else if (app->current_state == STATE_PLAYING && 
               app->cursor.hovered_game_cell_x >= 0 && 
               app->cursor.hovered_game_cell_y >= 0) {
        // Skip move if it's AI's turn in single player mode
        if (app->game_mode == MODE_SINGLE_PLAYER && 
            app->game.current_player == app->ai_player) {
            return;
        }
        
        // Try to make a move
        if (app->game.game_active) {
            if (make_move(&app->game, app->cursor.hovered_game_cell_x, app->cursor.hovered_game_cell_y)) {
                if (!app->game.game_active) {
                    app->winner = check_winner(&app->game);
                    app->is_draw = (app->winner == CELL_EMPTY);
                    app->current_state = STATE_GAME_OVER;
                }
            }
        }
    } else if (app->current_state == STATE_GAME_OVER && app->cursor.hovered_game_over_option) {
        switch (app->cursor.game_over_option_index) {
            case 0: // Restart
                if (app->game_mode == MODE_SINGLE_PLAYER) {
                    start_single_player_game(app);
                } else {
                    reset_board(&app->game);
                    app->game.game_active = true;
                    app->current_state = STATE_PLAYING;
                    app->winner = CELL_EMPTY;
                    app->is_draw = false;
                    app->has_active_game = true;
                }
                break;
            case 1: // Main Menu
                app->current_state = STATE_MAIN_MENU;
                app->menu_selection = 0;
                break;
            case 2: // Quit
                app->current_state = STATE_QUIT;
                break;
        }
    }
}