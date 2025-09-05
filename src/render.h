#ifndef RENDER_H
#define RENDER_H

#include "game.h"
#include <stdint.h>

// Legacy rendering functions (for backward compatibility)
void render_main_menu(const ApplicationState* app);
void render_mode_selection(const ApplicationState* app);
void render_difficulty_selection(const ApplicationState* app);
void render_game_board(const GameState* game);
void render_game_ui(const GameState* game);
void render_game_over(const ApplicationState* app);
void render_cursor(int x, int y);
void render_controls(AppState state);
void clear_screen();
void present_screen();
void render_global_cursor(const ApplicationState* app);
void render_main_menu_with_hover(const ApplicationState* app);
void render_mode_selection_with_hover(const ApplicationState* app);
void render_difficulty_selection_with_hover(const ApplicationState* app);
void render_game_board_with_hover(const ApplicationState* app);
void render_game_over_with_hover(const ApplicationState* app);
void render_ai_thinking_animation(const ApplicationState* app);
void render_ai_turn_indicator(const ApplicationState* app);
void render_player_indicators(const ApplicationState* app);

// New generic rendering functions
void render_application(const ApplicationState* app);
void render_game_selection_menu(const ApplicationState* app);
void render_game_selection_with_hover(const ApplicationState* app);
void render_current_game(const ApplicationState* app);
void render_universal_game_ui(const ApplicationState* app);

// Utility rendering functions
void render_text_centered(const char* text, int x, int y, uint32_t fg, uint32_t bg);
void render_text_at(const char* text, int x, int y, uint32_t fg, uint32_t bg);
void render_border(int x, int y, int width, int height, uint32_t fg, uint32_t bg);
void render_box(int x, int y, int width, int height, uint32_t fg, uint32_t bg);

#endif