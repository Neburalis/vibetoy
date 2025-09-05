#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "../games/game_interface.h"
#include <stdbool.h>

// TicTacToe cell states
typedef enum {
    TICTACTOE_CELL_EMPTY = 0,
    TICTACTOE_CELL_X = 1,
    TICTACTOE_CELL_O = 2
} TicTacToeCellState;

// TicTacToe game modes
typedef enum {
    TICTACTOE_MODE_TWO_PLAYER,
    TICTACTOE_MODE_SINGLE_PLAYER
} TicTacToeGameMode;

// TicTacToe AI difficulty levels
typedef enum {
    TICTACTOE_DIFFICULTY_EASY,
    TICTACTOE_DIFFICULTY_MEDIUM,
    TICTACTOE_DIFFICULTY_HARD
} TicTacToeAIDifficulty;

// TicTacToe AI state tracking
typedef struct {
    bool ai_move_in_progress;
    int ai_last_move_x;
    int ai_last_move_y;
    double ai_thinking_start_time;
    int ai_evaluation_calls;
} TicTacToeAIState;

// TicTacToe game state (moved from global GameState)
typedef struct {
    TicTacToeCellState board[3][3];
    int cursor_x;  // Keep for backward compatibility
    int cursor_y;  // Keep for backward compatibility
    TicTacToeCellState current_player;
    bool game_active;
    
    // Game mode and AI settings
    TicTacToeGameMode game_mode;
    TicTacToeAIDifficulty ai_difficulty;
    TicTacToeCellState human_player;    // CELL_X or CELL_O
    TicTacToeCellState ai_player;       // CELL_X or CELL_O
    bool ai_thinking;                   // Visual feedback during AI turn
    TicTacToeAIState ai_state;          // AI state tracking
    
    // UI state specific to TicTacToe
    int hovered_cell_x;
    int hovered_cell_y;
    
    // Game result tracking
    TicTacToeCellState winner;
    bool is_draw;
} TicTacToeGameState;

// Core game logic functions (internal)
void tictactoe_init_game_state(TicTacToeGameState* game);
void tictactoe_reset_board(TicTacToeGameState* game);
bool tictactoe_is_valid_move(const TicTacToeGameState* game, int x, int y);
bool tictactoe_make_move(TicTacToeGameState* game, int x, int y);
TicTacToeCellState tictactoe_check_winner(const TicTacToeGameState* game);
bool tictactoe_is_board_full(const TicTacToeGameState* game);
void tictactoe_switch_player(TicTacToeGameState* game);

// AI functions
int tictactoe_get_ai_move(const TicTacToeGameState* game, TicTacToeCellState ai_player, TicTacToeAIDifficulty difficulty);
int tictactoe_minimax_alpha_beta(TicTacToeGameState* game, int depth, int alpha, int beta, 
                                bool maximizing, TicTacToeCellState ai_player, TicTacToeCellState human_player);
int tictactoe_evaluate_game_state(const TicTacToeGameState* game, TicTacToeCellState ai_player, TicTacToeCellState human_player);
void tictactoe_get_available_moves(const TicTacToeGameState* game, int* moves, int* move_count);
bool tictactoe_simulate_move(TicTacToeGameState* game, int x, int y, TicTacToeCellState player);
void tictactoe_undo_move(TicTacToeGameState* game, int x, int y);
void tictactoe_init_ai_state(TicTacToeAIState* ai_state);
void tictactoe_trigger_ai_move(TicTacToeGameState* game);
void tictactoe_process_ai_turn(TicTacToeGameState* game);

// GameInterface implementation functions
void tictactoe_init(void* state);
void tictactoe_reset(void* state);
void tictactoe_update(void* state, double delta_time);
bool tictactoe_is_active(const void* state);
bool tictactoe_is_over(const void* state);
bool tictactoe_handle_input(void* state, const struct tb_event* event, const void* cursor);
bool tictactoe_handle_cursor_click(void* state, int x, int y);
void tictactoe_render(const void* state, int screen_width, int screen_height);
void tictactoe_render_ui(const void* state);
bool tictactoe_update_hover_state(void* state, const void* cursor);
bool tictactoe_has_winner(const void* state);
bool tictactoe_is_draw(const void* state);
const char* tictactoe_get_status_text(const void* state);
const char* tictactoe_get_winner_text(const void* state);
void tictactoe_cleanup(void* state);

// Get the TicTacToe game interface
const GameInterface* get_tictactoe_interface(void);

// Utility functions for game setup
void tictactoe_setup_two_player_game(TicTacToeGameState* game);
void tictactoe_setup_single_player_game(TicTacToeGameState* game, TicTacToeAIDifficulty difficulty);

#endif