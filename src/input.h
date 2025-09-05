#ifndef INPUT_H
#define INPUT_H

#include \"game.h\"
#include \"../lib/termbox2/termbox2.h\"

// Main input dispatcher
void handle_application_input(ApplicationState* app, const struct tb_event* event);

// State-specific input handlers
void handle_main_menu_input(ApplicationState* app, const struct tb_event* event);
void handle_game_selection_input(ApplicationState* app, const struct tb_event* event);
void handle_mode_selection_input(ApplicationState* app, const struct tb_event* event);
void handle_difficulty_selection_input(ApplicationState* app, const struct tb_event* event);
void handle_playing_input(ApplicationState* app, const struct tb_event* event);
void handle_game_over_input(ApplicationState* app, const struct tb_event* event);

// Universal input handlers
void handle_universal_game_input(ApplicationState* app, const struct tb_event* event);
void handle_cursor_movement(ApplicationState* app, const struct tb_event* event);
void handle_mouse_input(ApplicationState* app, const struct tb_event* event);

// Input processing utilities
bool is_movement_key(const struct tb_event* event);
bool is_action_key(const struct tb_event* event);
bool is_escape_key(const struct tb_event* event);

#endif