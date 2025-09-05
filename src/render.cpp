#include "render.h"
#include "../lib/termbox2/termbox2.h"

void clear_screen() {
    tb_clear();
}

void present_screen() {
    tb_present();
}

void render_main_menu(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "TIC-TAC-TOE GAME");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "================");
    y += 2;
    
    // Menu options
    const char* new_game_prefix = (app->menu_selection == 0) ? " > " : "   ";
    tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "%s[N] New Game", new_game_prefix);
    
    if (app->has_active_game) {
        const char* continue_prefix = (app->menu_selection == 1) ? " > " : "   ";
        tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "%s[C] Continue", continue_prefix);
        
        const char* quit_prefix = (app->menu_selection == 2) ? " > " : "   ";
        tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "%s[Q] Quit", quit_prefix);
    } else {
        const char* quit_prefix = (app->menu_selection == 1) ? " > " : "   ";
        tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "%s[Q] Quit", quit_prefix);
    }
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓ or W/S - Navigate");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

void render_game_board(const GameState* game) {
    int start_x = tb_width() / 2 - 6;
    int start_y = 8;
    
    // Draw board frame
    tb_printf(start_x, start_y, TB_DEFAULT, TB_DEFAULT, "┌───┬───┬───┐");
    tb_printf(start_x, start_y + 1, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 2, TB_DEFAULT, TB_DEFAULT, "├───┼───┼───┤");
    tb_printf(start_x, start_y + 3, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 4, TB_DEFAULT, TB_DEFAULT, "├───┼───┼───┤");
    tb_printf(start_x, start_y + 5, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 6, TB_DEFAULT, TB_DEFAULT, "└───┴───┴───┘");
    
    // Draw game pieces and cursor
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cell_x = start_x + 2 + col * 4;
            int cell_y = start_y + 1 + row * 2;
            
            char symbol = ' ';
            if (game->board[row][col] == CELL_X) {
                symbol = 'X';
            } else if (game->board[row][col] == CELL_O) {
                symbol = 'O';
            }
            
            // Highlight cursor position
            if (game->cursor_x == col && game->cursor_y == row) {
                tb_set_cell(cell_x - 1, cell_y, '[', TB_WHITE | TB_BOLD, TB_DEFAULT);
                tb_set_cell(cell_x, cell_y, symbol, TB_WHITE | TB_BOLD, TB_DEFAULT);
                tb_set_cell(cell_x + 1, cell_y, ']', TB_WHITE | TB_BOLD, TB_DEFAULT);
            } else {
                tb_set_cell(cell_x, cell_y, symbol, TB_DEFAULT, TB_DEFAULT);
            }
        }
    }
}

void render_game_ui(const GameState* game) {
    int x_center = tb_width() / 2;
    
    // Current player
    const char* player = (game->current_player == CELL_X) ? "X" : "O";
    tb_printf(x_center - 8, 5, TB_DEFAULT, TB_DEFAULT, "Current Player: %s", player);
}

void render_game_over(const ApplicationState* app) {
    int x_center = tb_width() / 2;
    int y = 16;
    
    tb_printf(x_center - 5, y++, TB_DEFAULT, TB_DEFAULT, "GAME OVER");
    tb_printf(x_center - 5, y++, TB_DEFAULT, TB_DEFAULT, "=========");
    y++;
    
    if (app->is_draw) {
        tb_printf(x_center - 4, y++, TB_DEFAULT, TB_DEFAULT, "It's a Draw!");
    } else {
        const char* winner = (app->winner == CELL_X) ? "X" : "O";
        tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "Player %s Wins!", winner);
    }
    
    y += 2;
    tb_printf(x_center - 7, y++, TB_DEFAULT, TB_DEFAULT, "[R] Restart Game");
    tb_printf(x_center - 7, y++, TB_DEFAULT, TB_DEFAULT, "[M] Main Menu");
    tb_printf(x_center - 7, y++, TB_DEFAULT, TB_DEFAULT, "[Q] Quit");
}

void render_controls(AppState state) {
    int y = tb_height() - 4;
    int x = 2;
    
    tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    
    if (state == STATE_PLAYING) {
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Place mark  [R] Restart  [M] Menu  [Q] Quit");
    } else if (state == STATE_MAIN_MENU) {
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
    } else if (state == STATE_GAME_OVER) {
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
        tb_printf(x, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select  [R] Restart  [M] Main Menu  [Q] Quit");
    }
}

void render_global_cursor(const ApplicationState* app) {
    // Get the current character at cursor position
    struct tb_cell* buffer = tb_cell_buffer();
    int width = tb_width();
    int cursor_pos = app->cursor.screen_y * width + app->cursor.screen_x;
    
    // Get current cell content
    struct tb_cell current_cell = buffer[cursor_pos];
    
    // Render cursor with inverted colors (Stone Story RPG style)
    // White background, black foreground to invert whatever is there
    char symbol = (current_cell.ch != 0) ? (char)current_cell.ch : ' ';
    tb_set_cell(app->cursor.screen_x, app->cursor.screen_y, symbol, TB_BLACK, TB_WHITE);
}

void render_main_menu_with_hover(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "TIC-TAC-TOE GAME");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "================");
    y += 2;
    
    // Menu options with hover highlighting
    uintattr_t new_game_bg = (app->cursor.hovered_menu_item == 0) ? TB_CYAN : TB_DEFAULT;
    uintattr_t new_game_fg = (app->cursor.hovered_menu_item == 0) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 6, y++, new_game_fg, new_game_bg, "[N] New Game");
    
    if (app->has_active_game) {
        uintattr_t continue_bg = (app->cursor.hovered_menu_item == 1) ? TB_CYAN : TB_DEFAULT;
        uintattr_t continue_fg = (app->cursor.hovered_menu_item == 1) ? TB_BLACK : TB_DEFAULT;
        tb_printf(x_center - 6, y++, continue_fg, continue_bg, "[C] Continue");
        
        uintattr_t quit_bg = (app->cursor.hovered_menu_item == 2) ? TB_CYAN : TB_DEFAULT;
        uintattr_t quit_fg = (app->cursor.hovered_menu_item == 2) ? TB_BLACK : TB_DEFAULT;
        tb_printf(x_center - 6, y++, quit_fg, quit_bg, "[Q] Quit");
    } else {
        uintattr_t quit_bg = (app->cursor.hovered_menu_item == 1) ? TB_CYAN : TB_DEFAULT;
        uintattr_t quit_fg = (app->cursor.hovered_menu_item == 1) ? TB_BLACK : TB_DEFAULT;
        tb_printf(x_center - 6, y++, quit_fg, quit_bg, "[Q] Quit");
    }
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

void render_game_board_with_hover(const ApplicationState* app) {
    int start_x = tb_width() / 2 - 6;
    int start_y = 8;
    
    // Draw board frame
    tb_printf(start_x, start_y, TB_DEFAULT, TB_DEFAULT, "┌───┬───┬───┐");
    tb_printf(start_x, start_y + 1, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 2, TB_DEFAULT, TB_DEFAULT, "├───┼───┼───┤");
    tb_printf(start_x, start_y + 3, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 4, TB_DEFAULT, TB_DEFAULT, "├───┼───┼───┤");
    tb_printf(start_x, start_y + 5, TB_DEFAULT, TB_DEFAULT, "│   │   │   │");
    tb_printf(start_x, start_y + 6, TB_DEFAULT, TB_DEFAULT, "└───┴───┴───┘");
    
    // Draw game pieces with hover highlighting
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cell_x = start_x + 2 + col * 4;
            int cell_y = start_y + 1 + row * 2;
            
            char symbol = ' ';
            if (app->game.board[row][col] == CELL_X) {
                symbol = 'X';
            } else if (app->game.board[row][col] == CELL_O) {
                symbol = 'O';
            }
            
            // Check if this cell is hovered
            uintattr_t bg = TB_DEFAULT;
            uintattr_t fg = TB_DEFAULT;
            
            if (app->cursor.hovered_game_cell_x == col && app->cursor.hovered_game_cell_y == row) {
                bg = TB_YELLOW;
                fg = TB_BLACK;
            }
            
            tb_set_cell(cell_x, cell_y, symbol, fg, bg);
        }
    }
}

void render_game_over_with_hover(const ApplicationState* app) {
    int x_center = tb_width() / 2;
    int y = 16;
    
    tb_printf(x_center - 5, y++, TB_DEFAULT, TB_DEFAULT, "GAME OVER");
    tb_printf(x_center - 5, y++, TB_DEFAULT, TB_DEFAULT, "=========");
    y++;
    
    if (app->is_draw) {
        tb_printf(x_center - 4, y++, TB_DEFAULT, TB_DEFAULT, "It's a Draw!");
    } else {
        const char* winner = (app->winner == CELL_X) ? "X" : "O";
        tb_printf(x_center - 6, y++, TB_DEFAULT, TB_DEFAULT, "Player %s Wins!", winner);
    }
    
    y += 2;
    
    // Game over options with hover highlighting
    uintattr_t restart_bg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 0) ? TB_CYAN : TB_DEFAULT;
    uintattr_t restart_fg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 0) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 7, y++, restart_fg, restart_bg, "[R] Restart Game");
    
    uintattr_t menu_bg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 1) ? TB_CYAN : TB_DEFAULT;
    uintattr_t menu_fg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 1) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 7, y++, menu_fg, menu_bg, "[M] Main Menu");
    
    uintattr_t quit_bg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 2) ? TB_CYAN : TB_DEFAULT;
    uintattr_t quit_fg = (app->cursor.hovered_game_over_option && app->cursor.game_over_option_index == 2) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 7, y++, quit_fg, quit_bg, "[Q] Quit");
}

// Mode Selection Screen
void render_mode_selection(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "SELECT GAME MODE");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "=================");
    y += 2;
    
    // Mode options
    const char* two_player_prefix = (app->mode_selection == 0) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[1] Two Player", two_player_prefix);
    
    const char* single_player_prefix = (app->mode_selection == 1) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[2] Single Player (vs AI)", single_player_prefix);
    
    const char* back_prefix = (app->mode_selection == 2) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[B] Back to Main Menu", back_prefix);
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓ or W/S - Navigate");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

void render_mode_selection_with_hover(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "SELECT GAME MODE");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "=================");
    y += 2;
    
    // Mode options with hover highlighting
    uintattr_t two_player_bg = (app->cursor.hovered_mode_selection == 0) ? TB_CYAN : TB_DEFAULT;
    uintattr_t two_player_fg = (app->cursor.hovered_mode_selection == 0) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, two_player_fg, two_player_bg, "[1] Two Player");
    
    uintattr_t single_player_bg = (app->cursor.hovered_mode_selection == 1) ? TB_CYAN : TB_DEFAULT;
    uintattr_t single_player_fg = (app->cursor.hovered_mode_selection == 1) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, single_player_fg, single_player_bg, "[2] Single Player (vs AI)");
    
    uintattr_t back_bg = (app->cursor.hovered_mode_selection == 2) ? TB_CYAN : TB_DEFAULT;
    uintattr_t back_fg = (app->cursor.hovered_mode_selection == 2) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, back_fg, back_bg, "[B] Back to Main Menu");
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

// Difficulty Selection Screen
void render_difficulty_selection(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "SELECT DIFFICULTY");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "=================");
    y += 2;
    
    // Difficulty options
    const char* easy_prefix = (app->difficulty_selection == 0) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[1] Easy (AI makes mistakes)", easy_prefix);
    
    const char* medium_prefix = (app->difficulty_selection == 1) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[2] Medium (Good AI)", medium_prefix);
    
    const char* hard_prefix = (app->difficulty_selection == 2) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[3] Hard (Unbeatable AI)", hard_prefix);
    
    const char* back_prefix = (app->difficulty_selection == 3) ? " > " : "   ";
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "%s[B] Back to Mode Selection", back_prefix);
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓ or W/S - Navigate");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

void render_difficulty_selection_with_hover(const ApplicationState* app) {
    int y = 5;
    int x_center = tb_width() / 2;
    
    // Title
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "SELECT DIFFICULTY");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "=================");
    y += 2;
    
    // Difficulty options with hover highlighting
    uintattr_t easy_bg = (app->cursor.hovered_difficulty_selection == 0) ? TB_CYAN : TB_DEFAULT;
    uintattr_t easy_fg = (app->cursor.hovered_difficulty_selection == 0) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, easy_fg, easy_bg, "[1] Easy (AI makes mistakes)");
    
    uintattr_t medium_bg = (app->cursor.hovered_difficulty_selection == 1) ? TB_CYAN : TB_DEFAULT;
    uintattr_t medium_fg = (app->cursor.hovered_difficulty_selection == 1) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, medium_fg, medium_bg, "[2] Medium (Good AI)");
    
    uintattr_t hard_bg = (app->cursor.hovered_difficulty_selection == 2) ? TB_CYAN : TB_DEFAULT;
    uintattr_t hard_fg = (app->cursor.hovered_difficulty_selection == 2) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, hard_fg, hard_bg, "[3] Hard (Unbeatable AI)");
    
    uintattr_t back_bg = (app->cursor.hovered_difficulty_selection == 3) ? TB_CYAN : TB_DEFAULT;
    uintattr_t back_fg = (app->cursor.hovered_difficulty_selection == 3) ? TB_BLACK : TB_DEFAULT;
    tb_printf(x_center - 8, y++, back_fg, back_bg, "[B] Back to Mode Selection");
    
    y += 3;
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Controls:");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "↑↓←→ or WASD - Move cursor");
    tb_printf(x_center - 8, y++, TB_DEFAULT, TB_DEFAULT, "Enter - Select");
}

// AI Visual Feedback Functions
void render_ai_thinking_animation(const ApplicationState* app) {
    if (!app->ai_thinking) return;
    
    int x_center = tb_width() / 2;
    int y = 6;
    
    // Simple thinking animation with dots
    static int animation_frame = 0;
    animation_frame = (animation_frame + 1) % 20; // Cycle every 20 frames
    
    const char* animation_states[] = {
        "AI is thinking   ",
        "AI is thinking.  ",
        "AI is thinking.. ",
        "AI is thinking..."
    };
    
    int state_index = animation_frame / 5; // Change state every 5 frames
    tb_printf(x_center - 8, y, TB_YELLOW | TB_BOLD, TB_DEFAULT, "%s", animation_states[state_index]);
}

void render_ai_turn_indicator(const ApplicationState* app) {
    if (app->game_mode != MODE_SINGLE_PLAYER) return;
    
    int x_center = tb_width() / 2;
    int y = 7;
    
    if (app->game.current_player == app->ai_player) {
        const char* ai_symbol = (app->ai_player == CELL_X) ? "X" : "O";
        tb_printf(x_center - 6, y, TB_GREEN | TB_BOLD, TB_DEFAULT, "AI Turn (%s)", ai_symbol);
    } else {
        const char* human_symbol = (app->human_player == CELL_X) ? "X" : "O";
        tb_printf(x_center - 7, y, TB_BLUE | TB_BOLD, TB_DEFAULT, "Your Turn (%s)", human_symbol);
    }
}

void render_player_indicators(const ApplicationState* app) {
    if (app->game_mode != MODE_SINGLE_PLAYER) return;
    
    int x_center = tb_width() / 2;
    int y = 3;
    
    const char* human_symbol = (app->human_player == CELL_X) ? "X" : "O";
    const char* ai_symbol = (app->ai_player == CELL_X) ? "X" : "O";
    
    const char* difficulty_name;
    switch (app->ai_difficulty) {
        case DIFFICULTY_EASY:
            difficulty_name = "Easy";
            break;
        case DIFFICULTY_MEDIUM:
            difficulty_name = "Medium";
            break;
        case DIFFICULTY_HARD:
            difficulty_name = "Hard";
            break;
        default:
            difficulty_name = "Unknown";
            break;
    }
    
    tb_printf(x_center - 12, y++, TB_DEFAULT, TB_DEFAULT, "You: %s  |  AI: %s (%s)", human_symbol, ai_symbol, difficulty_name);
}