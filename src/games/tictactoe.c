#include "tictactoe.h"
#include "../game.h"
#include "../../lib/termbox2/termbox2.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Static buffer for status text
static char status_buffer[256];

// Core game logic functions
void tictactoe_init_game_state(TicTacToeGameState* game) {
    game->cursor_x = 1;
    game->cursor_y = 1;
    game->current_player = TICTACTOE_CELL_X;
    game->game_active = true;
    game->game_mode = TICTACTOE_MODE_TWO_PLAYER;
    game->ai_difficulty = TICTACTOE_DIFFICULTY_MEDIUM;
    game->human_player = TICTACTOE_CELL_X;
    game->ai_player = TICTACTOE_CELL_O;
    game->ai_thinking = false;
    game->hovered_cell_x = -1;
    game->hovered_cell_y = -1;
    game->winner = TICTACTOE_CELL_EMPTY;
    game->is_draw = false;
    
    tictactoe_init_ai_state(&game->ai_state);
    tictactoe_reset_board(game);
}

void tictactoe_reset_board(TicTacToeGameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            game->board[i][j] = TICTACTOE_CELL_EMPTY;
        }
    }
    game->current_player = TICTACTOE_CELL_X;
    game->game_active = true;
    game->winner = TICTACTOE_CELL_EMPTY;
    game->is_draw = false;
}

bool tictactoe_is_valid_move(const TicTacToeGameState* game, int x, int y) {
    if (x < 0 || x >= 3 || y < 0 || y >= 3) {
        return false;
    }
    if (!game->game_active) {
        return false;
    }
    return game->board[y][x] == TICTACTOE_CELL_EMPTY;
}

bool tictactoe_make_move(TicTacToeGameState* game, int x, int y) {
    if (!tictactoe_is_valid_move(game, x, y)) {
        return false;
    }
    
    game->board[y][x] = game->current_player;
    
    TicTacToeCellState winner = tictactoe_check_winner(game);
    if (winner != TICTACTOE_CELL_EMPTY || tictactoe_is_board_full(game)) {
        game->game_active = false;
        game->winner = winner;
        game->is_draw = (winner == TICTACTOE_CELL_EMPTY);
    } else {
        tictactoe_switch_player(game);
    }
    
    return true;
}

TicTacToeCellState tictactoe_check_winner(const TicTacToeGameState* game) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (game->board[i][0] != TICTACTOE_CELL_EMPTY &&
            game->board[i][0] == game->board[i][1] &&
            game->board[i][1] == game->board[i][2]) {
            return game->board[i][0];
        }
    }
    
    // Check columns
    for (int j = 0; j < 3; j++) {
        if (game->board[0][j] != TICTACTOE_CELL_EMPTY &&
            game->board[0][j] == game->board[1][j] &&
            game->board[1][j] == game->board[2][j]) {
            return game->board[0][j];
        }
    }
    
    // Check diagonals
    if (game->board[0][0] != TICTACTOE_CELL_EMPTY &&
        game->board[0][0] == game->board[1][1] &&
        game->board[1][1] == game->board[2][2]) {
        return game->board[0][0];
    }
    
    if (game->board[0][2] != TICTACTOE_CELL_EMPTY &&
        game->board[0][2] == game->board[1][1] &&
        game->board[1][1] == game->board[2][0]) {
        return game->board[0][2];
    }
    
    return TICTACTOE_CELL_EMPTY;
}

bool tictactoe_is_board_full(const TicTacToeGameState* game) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game->board[i][j] == TICTACTOE_CELL_EMPTY) {
                return false;
            }
        }
    }
    return true;
}

void tictactoe_switch_player(TicTacToeGameState* game) {
    game->current_player = (game->current_player == TICTACTOE_CELL_X) ? TICTACTOE_CELL_O : TICTACTOE_CELL_X;
}

// AI implementation (adapted from original game.cpp)
void tictactoe_init_ai_state(TicTacToeAIState* ai_state) {
    ai_state->ai_move_in_progress = false;
    ai_state->ai_last_move_x = -1;
    ai_state->ai_last_move_y = -1;
    ai_state->ai_thinking_start_time = 0.0;
    ai_state->ai_evaluation_calls = 0;
}

void tictactoe_get_available_moves(const TicTacToeGameState* game, int* moves, int* move_count) {
    *move_count = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (game->board[y][x] == TICTACTOE_CELL_EMPTY) {
                moves[(*move_count) * 2] = x;
                moves[(*move_count) * 2 + 1] = y;
                (*move_count)++;
            }
        }
    }
}

bool tictactoe_simulate_move(TicTacToeGameState* game, int x, int y, TicTacToeCellState player) {
    if (x < 0 || x >= 3 || y < 0 || y >= 3 || game->board[y][x] != TICTACTOE_CELL_EMPTY) {
        return false;
    }
    game->board[y][x] = player;
    return true;
}

void tictactoe_undo_move(TicTacToeGameState* game, int x, int y) {
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
        game->board[y][x] = TICTACTOE_CELL_EMPTY;
    }
}

int tictactoe_evaluate_game_state(const TicTacToeGameState* game, TicTacToeCellState ai_player, TicTacToeCellState human_player) {
    TicTacToeCellState winner = tictactoe_check_winner(game);
    
    if (winner == ai_player) {
        return 10;
    } else if (winner == human_player) {
        return -10;
    } else {
        return 0;
    }
}

int tictactoe_apply_positional_bonus(int x, int y) {
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

int tictactoe_evaluate_strategic_positions(const TicTacToeGameState* game, TicTacToeCellState player) {
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

int tictactoe_get_opening_move(const TicTacToeGameState* game, TicTacToeCellState ai_player) {
    // Suppress unused parameter warning
    (void)ai_player;
    
    int move_count = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (game->board[y][x] != TICTACTOE_CELL_EMPTY) {
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
        if (game->board[1][1] != TICTACTOE_CELL_EMPTY) {
            return 0 * 3 + 0; // Top-left corner
        } else {
            // If opponent took a corner, take center
            return 1 * 3 + 1; // Center
        }
    }
    
    return -1; // No opening move available
}

int tictactoe_minimax_alpha_beta(TicTacToeGameState* game, int depth, int alpha, int beta, 
                                bool maximizing, TicTacToeCellState ai_player, TicTacToeCellState human_player) {
    // Check if game is terminal
    TicTacToeCellState winner = tictactoe_check_winner(game);
    if (winner != TICTACTOE_CELL_EMPTY) {
        int score = tictactoe_evaluate_game_state(game, ai_player, human_player);
        return score - depth; // Prefer immediate wins
    }
    
    if (tictactoe_is_board_full(game)) {
        return 0; // Draw
    }
    
    if (depth >= 9) { // Maximum search depth
        return tictactoe_evaluate_strategic_positions(game, ai_player) - 
               tictactoe_evaluate_strategic_positions(game, human_player);
    }
    
    int moves[18]; // Max 9 moves * 2 coordinates
    int move_count;
    tictactoe_get_available_moves(game, moves, &move_count);
    
    if (maximizing) {
        int max_eval = -1000;
        for (int i = 0; i < move_count; i++) {
            int x = moves[i * 2];
            int y = moves[i * 2 + 1];
            
            tictactoe_simulate_move(game, x, y, ai_player);
            int eval = tictactoe_minimax_alpha_beta(game, depth + 1, alpha, beta, false, ai_player, human_player);
            tictactoe_undo_move(game, x, y);
            
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
            
            tictactoe_simulate_move(game, x, y, human_player);
            int eval = tictactoe_minimax_alpha_beta(game, depth + 1, alpha, beta, true, ai_player, human_player);
            tictactoe_undo_move(game, x, y);
            
            min_eval = (eval < min_eval) ? eval : min_eval;
            beta = (beta < eval) ? beta : eval;
            
            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        return min_eval;
    }
}

int tictactoe_apply_difficulty_filter(int best_move, int* move_scores, int move_count, TicTacToeAIDifficulty difficulty) {
    if (difficulty == TICTACTOE_DIFFICULTY_HARD) {
        return best_move; // Always play optimally
    }
    
    if (difficulty == TICTACTOE_DIFFICULTY_EASY) {
        // 30% chance to play randomly
        if ((rand() % 100) < 30) {
            return rand() % move_count;
        }
    } else if (difficulty == TICTACTOE_DIFFICULTY_MEDIUM) {
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

int tictactoe_get_ai_move(const TicTacToeGameState* game, TicTacToeCellState ai_player, TicTacToeAIDifficulty difficulty) {
    TicTacToeCellState human_player = (ai_player == TICTACTOE_CELL_X) ? TICTACTOE_CELL_O : TICTACTOE_CELL_X;
    
    // Check for opening moves first
    int opening_move = tictactoe_get_opening_move(game, ai_player);
    if (opening_move >= 0 && difficulty == TICTACTOE_DIFFICULTY_HARD) {
        int x = opening_move % 3;
        int y = opening_move / 3;
        if (tictactoe_is_valid_move(game, x, y)) {
            return opening_move;
        }
    }
    
    // Get available moves
    int moves[18];
    int move_count;
    tictactoe_get_available_moves(game, moves, &move_count);
    
    if (move_count == 0) {
        return -1; // No moves available
    }
    
    // Easy difficulty - sometimes play randomly
    if (difficulty == TICTACTOE_DIFFICULTY_EASY && (rand() % 100) < 40) {
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
        
        TicTacToeGameState temp_game = *game;
        tictactoe_simulate_move(&temp_game, x, y, ai_player);
        
        int score = tictactoe_minimax_alpha_beta(&temp_game, 0, -1000, 1000, false, ai_player, human_player);
        move_scores[i] = score;
        
        if (score > best_score) {
            best_score = score;
            best_move = i;
        }
    }
    
    // Apply difficulty filter
    best_move = tictactoe_apply_difficulty_filter(best_move, move_scores, move_count, difficulty);
    
    int x = moves[best_move * 2];
    int y = moves[best_move * 2 + 1];
    
    return y * 3 + x; // Convert to single index
}

void tictactoe_trigger_ai_move(TicTacToeGameState* game) {
    if (game->game_mode != TICTACTOE_MODE_SINGLE_PLAYER || game->current_player != game->ai_player) {
        return;
    }
    
    game->ai_thinking = true;
    game->ai_state.ai_move_in_progress = true;
}

void tictactoe_process_ai_turn(TicTacToeGameState* game) {
    if (!game->ai_thinking || game->game_mode != TICTACTOE_MODE_SINGLE_PLAYER) {
        return;
    }
    
    int move_index = tictactoe_get_ai_move(game, game->ai_player, game->ai_difficulty);
    
    if (move_index >= 0) {
        int x = move_index % 3;
        int y = move_index / 3;
        
        if (tictactoe_make_move(game, x, y)) {
            game->ai_state.ai_last_move_x = x;
            game->ai_state.ai_last_move_y = y;
        }
    }
    
    game->ai_thinking = false;
    game->ai_state.ai_move_in_progress = false;
}

// GameInterface implementation functions
void tictactoe_init(void* state) {
    TicTacToeGameState* game = (TicTacToeGameState*)state;
    tictactoe_init_game_state(game);
}

void tictactoe_reset(void* state) {
    TicTacToeGameState* game = (TicTacToeGameState*)state;
    tictactoe_reset_board(game);
}

void tictactoe_update(void* state, double delta_time) {
    TicTacToeGameState* game = (TicTacToeGameState*)state;
    
    // Process AI turn if needed
    if (game->game_mode == TICTACTOE_MODE_SINGLE_PLAYER) {
        if (game->current_player == game->ai_player && game->game_active && !game->ai_thinking) {
            tictactoe_trigger_ai_move(game);
        }
        
        if (game->ai_thinking) {
            tictactoe_process_ai_turn(game);
        }
    }
    
    // Suppress unused parameter warning
    (void)delta_time;
}

bool tictactoe_is_active(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    return game->game_active;
}

bool tictactoe_is_over(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    return !game->game_active;
}

bool tictactoe_handle_input(void* state, const struct tb_event* event, const void* cursor) {
    // Input handling will be implemented in generic input pipeline
    // For now, return false to indicate input not handled
    (void)state;
    (void)event;
    (void)cursor;
    return false;
}

bool tictactoe_handle_cursor_click(void* state, int x, int y) {
    TicTacToeGameState* game = (TicTacToeGameState*)state;
    
    // Convert screen coordinates to board coordinates
    // This is a simplified conversion - actual implementation would need
    // to match the rendering logic
    int start_x = x - 10; // Approximate board start position
    int start_y = y - 8;
    
    if (start_x >= 0 && start_x < 12 && start_y >= 0 && start_y < 6) {
        int cell_x = start_x / 4;
        int cell_y = start_y / 2;
        
        if (cell_x >= 0 && cell_x < 3 && cell_y >= 0 && cell_y < 3) {
            // Skip move if it's AI's turn in single player mode
            if (game->game_mode == TICTACTOE_MODE_SINGLE_PLAYER && 
                game->current_player == game->ai_player) {
                return false;
            }
            
            return tictactoe_make_move(game, cell_x, cell_y);
        }
    }
    
    return false;
}

void tictactoe_render(const void* state, int screen_width, int screen_height) {
    // Rendering will be handled by the generic render pipeline
    // This is a placeholder
    (void)state;
    (void)screen_width;
    (void)screen_height;
}

void tictactoe_render_ui(const void* state) {
    // UI rendering will be handled by the generic render pipeline
    // This is a placeholder
    (void)state;
}

bool tictactoe_update_hover_state(void* state, const void* cursor) {
    TicTacToeGameState* game = (TicTacToeGameState*)state;
    
    // Cast cursor back to GlobalCursor for now
    // In a full implementation, we'd use a generic cursor interface
    const GlobalCursor* global_cursor = (const GlobalCursor*)cursor;
    
    // Reset hover state
    game->hovered_cell_x = -1;
    game->hovered_cell_y = -1;
    
    // Calculate game board area (this needs to match rendering logic)
    int start_x = 10; // Approximate board start position
    int start_y = 8;
    
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cell_x = start_x + 2 + col * 4;
            int cell_y = start_y + 1 + row * 2;
            
            // Check if cursor is over this cell (with some tolerance)
            if (global_cursor->screen_x >= cell_x - 1 && global_cursor->screen_x <= cell_x + 1 && 
                global_cursor->screen_y == cell_y) {
                game->hovered_cell_x = col;
                game->hovered_cell_y = row;
                return true;
            }
        }
    }
    
    return false;
}

bool tictactoe_has_winner(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    return game->winner != TICTACTOE_CELL_EMPTY;
}

bool tictactoe_is_draw(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    return game->is_draw;
}

const char* tictactoe_get_status_text(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    
    if (!game->game_active) {
        if (game->winner != TICTACTOE_CELL_EMPTY) {
            snprintf(status_buffer, sizeof(status_buffer), "Player %c wins!", 
                    (game->winner == TICTACTOE_CELL_X) ? 'X' : 'O');
        } else {
            snprintf(status_buffer, sizeof(status_buffer), "It's a draw!");
        }
    } else {
        snprintf(status_buffer, sizeof(status_buffer), "Player %c's turn", 
                (game->current_player == TICTACTOE_CELL_X) ? 'X' : 'O');
    }
    
    return status_buffer;
}

const char* tictactoe_get_winner_text(const void* state) {
    const TicTacToeGameState* game = (const TicTacToeGameState*)state;
    
    if (game->winner != TICTACTOE_CELL_EMPTY) {
        snprintf(status_buffer, sizeof(status_buffer), "Player %c", 
                (game->winner == TICTACTOE_CELL_X) ? 'X' : 'O');
        return status_buffer;
    } else if (game->is_draw) {
        return "Draw";
    }
    
    return "None";
}

void tictactoe_cleanup(void* state) {
    // No special cleanup needed for TicTacToe
    (void)state;
}

// Utility functions for game setup
void tictactoe_setup_two_player_game(TicTacToeGameState* game) {
    game->game_mode = TICTACTOE_MODE_TWO_PLAYER;
    tictactoe_reset_board(game);
}

void tictactoe_setup_single_player_game(TicTacToeGameState* game, TicTacToeAIDifficulty difficulty) {
    game->game_mode = TICTACTOE_MODE_SINGLE_PLAYER;
    game->ai_difficulty = difficulty;
    game->human_player = TICTACTOE_CELL_X;
    game->ai_player = TICTACTOE_CELL_O;
    game->ai_thinking = false;
    tictactoe_init_ai_state(&game->ai_state);
    tictactoe_reset_board(game);
}

// Static GameInterface instance
static const GameInterface tictactoe_interface = {
    .game_name = "Tic-Tac-Toe",
    .game_description = "Classic 3x3 grid game with X and O",
    .init_game = tictactoe_init,
    .reset_game = tictactoe_reset,
    .update_game = tictactoe_update,
    .is_game_active = tictactoe_is_active,
    .is_game_over = tictactoe_is_over,
    .handle_input = tictactoe_handle_input,
    .handle_cursor_click = tictactoe_handle_cursor_click,
    .render_game = tictactoe_render,
    .render_game_ui = tictactoe_render_ui,
    .update_hover_state = tictactoe_update_hover_state,
    .has_winner = tictactoe_has_winner,
    .is_draw = tictactoe_is_draw,
    .get_status_text = tictactoe_get_status_text,
    .get_winner_text = tictactoe_get_winner_text,
    .game_state_size = sizeof(TicTacToeGameState),
    .cleanup_game = tictactoe_cleanup
};

// Get the TicTacToe game interface
const GameInterface* get_tictactoe_interface(void) {
    return &tictactoe_interface;
}