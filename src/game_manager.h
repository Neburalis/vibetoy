#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "games/game_interface.h"
#include <stdbool.h>
#include <stddef.h>

// Game manager structure
typedef struct {
    GameType current_game_type;
    const GameInterface* current_game_interface;
    void* current_game_state;
    bool game_loaded;
    bool game_initialized;
    
    // Timing for games that need it
    double last_update_time;
    double frame_delta;
} GameManager;

// Game manager functions
void init_game_manager(GameManager* manager);
bool load_game(GameManager* manager, GameType game_type);
void unload_current_game(GameManager* manager);
void cleanup_game_manager(GameManager* manager);

// Game state management
bool is_game_loaded(const GameManager* manager);
bool is_game_initialized(const GameManager* manager);
void* get_current_game_state(const GameManager* manager);
const GameInterface* get_current_game_interface(const GameManager* manager);
GameType get_current_game_type(const GameManager* manager);

// Game lifecycle management
bool init_current_game(GameManager* manager);
void reset_current_game(GameManager* manager);
void update_current_game(GameManager* manager, double delta_time);

// Utility functions
const char* get_game_name(GameType game_type);
const char* get_game_description(GameType game_type);
bool is_valid_game_type(GameType game_type);

#endif