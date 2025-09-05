#ifndef RENDER_H
#define RENDER_H

#include "game.h"

// Rendering functions
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

#endif