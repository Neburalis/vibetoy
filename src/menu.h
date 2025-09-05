#ifndef MENU_H
#define MENU_H

#include "game.h"

#include "../lib/termbox2/termbox2.h"

// Menu functions
void init_application(ApplicationState* app);
void handle_menu_input(ApplicationState* app, const struct tb_event* event);
void handle_mode_selection_input(ApplicationState* app, const struct tb_event* event);
void handle_difficulty_selection_input(ApplicationState* app, const struct tb_event* event);
void handle_game_input(ApplicationState* app, const struct tb_event* event);
void handle_game_over_input(ApplicationState* app, const struct tb_event* event);
void start_single_player_game(ApplicationState* app);

#endif