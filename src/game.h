#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "game_manager.h"

// Legacy types for backward compatibility (will be removed gradually)
typedef enum {
    CELL_EMPTY = 0,
    CELL_X = 1,
    CELL_O = 2
} CellState;

typedef enum {
    MODE_TWO_PLAYER,
    MODE_SINGLE_PLAYER
} GameMode;

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} AIDifficulty;

// Legacy GameState structure (for compatibility during transition)
typedef struct {
    CellState board[3][3];
    int cursor_x;  // Keep for backward compatibility
    int cursor_y;  // Keep for backward compatibility
    CellState current_player;
    bool game_active;
} GameState;

typedef enum {
    STATE_MAIN_MENU,
    STATE_GAME_SELECTION,  // New state for game selection
    STATE_MODE_SELECTION,
    STATE_DIFFICULTY_SELECTION,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_QUIT
} AppState;

// Global cursor position and hover state
typedef struct {
    int screen_x;  // Absolute screen position
    int screen_y;
    int hovered_menu_item;  // -1 if no menu item hovered, otherwise menu index
    int hovered_game_cell_x; // -1 if no game cell hovered
    int hovered_game_cell_y;
    bool hovered_game_over_option; // For game over screen options
    int game_over_option_index;    // Which game over option is hovered
    int hovered_mode_selection;    // For mode selection screen
    int hovered_difficulty_selection; // For difficulty selection screen
    int hovered_game_selection;    // For game selection screen
} GlobalCursor;

// AI state tracking
typedef struct {
    bool ai_move_in_progress;
    int ai_last_move_x;
    int ai_last_move_y;
    double ai_thinking_start_time;
    int ai_evaluation_calls;
} AIState;

typedef struct {
    AppState current_state;
    AppState previous_state;  // For returning from pause/options
    
    // Game management (new architecture)
    GameManager game_manager;
    
    // Legacy fields (for backward compatibility during transition)
    GameState game;
    GameMode game_mode;
    AIDifficulty ai_difficulty;
    CellState human_player;    // CELL_X or CELL_O
    CellState ai_player;       // CELL_X or CELL_O
    bool ai_thinking;          // Visual feedback during AI turn
    
    // UI state
    int menu_selection;
    int game_selection;        // Which game is selected in game selection menu
    int mode_selection;
    int difficulty_selection;
    CellState winner;
    bool is_draw;
    bool has_active_game;
    
    // Global cursor system (unchanged)
    GlobalCursor cursor;
    
    // Legacy AI state (for compatibility)
    AIState ai_state;
    
    // Timing for games that need it
    double last_update_time;
    double frame_delta;
} ApplicationState;

// Legacy game logic functions (for backward compatibility)
void init_game(GameState* game);
void reset_board(GameState* game);
bool is_valid_move(const GameState* game, int x, int y);
bool make_move(GameState* game, int x, int y);
CellState check_winner(const GameState* game);
bool is_board_full(const GameState* game);
void switch_player(GameState* game);
void move_cursor(GameState* game, int dx, int dy);

// Global cursor functions
void init_global_cursor(GlobalCursor* cursor);
void move_global_cursor(ApplicationState* app, int dx, int dy);
void set_cursor_position(ApplicationState* app, int x, int y);
void update_hover_state(ApplicationState* app);
void handle_cursor_click(ApplicationState* app);

// Legacy AI functions (for backward compatibility)
int get_ai_move(const GameState* game, CellState ai_player, AIDifficulty difficulty);
int minimax_alpha_beta(GameState* game, int depth, int alpha, int beta, 
                      bool maximizing, CellState ai_player, CellState human_player);
int evaluate_game_state(const GameState* game, CellState ai_player, CellState human_player);
int apply_difficulty_filter(int best_move, int* move_scores, int move_count, AIDifficulty difficulty);
void get_available_moves(const GameState* game, int* moves, int* move_count);
bool simulate_move(GameState* game, int x, int y, CellState player);
void undo_move(GameState* game, int x, int y);
int evaluate_strategic_positions(const GameState* game, CellState player);
int get_opening_move(const GameState* game, CellState ai_player);
int apply_positional_bonus(int x, int y);
void trigger_ai_move(ApplicationState* app);
void init_ai_state(AIState* ai_state);
void process_ai_turn(ApplicationState* app);

// New game manager integration functions
void init_application_state(ApplicationState* app);
bool load_selected_game(ApplicationState* app, GameType game_type);
void unload_current_game(ApplicationState* app);
bool has_active_game_session(const ApplicationState* app);
void update_game_state(ApplicationState* app, double delta_time);

// State transition helpers
void transition_to_game_selection(ApplicationState* app);
void transition_to_playing(ApplicationState* app);
void transition_to_game_over(ApplicationState* app);
void transition_to_main_menu(ApplicationState* app);

#endif