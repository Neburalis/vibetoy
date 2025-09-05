#include "game_manager.h"
#include "games/game_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global registry for game interfaces
static const GameInterface* game_registry[GAME_TYPE_COUNT] = {NULL};

// Initialize game manager
void init_game_manager(GameManager* manager) {
    if (!manager) return;
    
    manager->current_game_type = GAME_TYPE_COUNT; // Invalid type initially
    manager->current_game_interface = NULL;
    manager->current_game_state = NULL;
    manager->game_loaded = false;
    manager->game_initialized = false;
    manager->last_update_time = 0.0;
    manager->frame_delta = 0.0;
}

// Load a game by type
bool load_game(GameManager* manager, GameType game_type) {
    if (!manager || !is_valid_game_type(game_type)) {
        return false;
    }
    
    // Unload current game if any
    unload_current_game(manager);
    
    // Get the game interface
    const GameInterface* interface = get_game_interface(game_type);
    if (!interface) {
        return false;
    }
    
    // Allocate memory for game state
    void* game_state = malloc(interface->game_state_size);
    if (!game_state) {
        return false;
    }
    
    // Set up the manager
    manager->current_game_type = game_type;
    manager->current_game_interface = interface;
    manager->current_game_state = game_state;
    manager->game_loaded = true;
    manager->game_initialized = false;
    
    return true;
}

// Unload current game
void unload_current_game(GameManager* manager) {
    if (!manager || !manager->game_loaded) {
        return;
    }
    
    // Cleanup game if it has a cleanup function
    if (manager->current_game_interface && 
        manager->current_game_interface->cleanup_game &&
        manager->current_game_state) {
        manager->current_game_interface->cleanup_game(manager->current_game_state);
    }
    
    // Free game state memory
    if (manager->current_game_state) {
        free(manager->current_game_state);
        manager->current_game_state = NULL;
    }
    
    // Reset manager state
    manager->current_game_type = GAME_TYPE_COUNT;
    manager->current_game_interface = NULL;
    manager->game_loaded = false;
    manager->game_initialized = false;
}

// Cleanup game manager
void cleanup_game_manager(GameManager* manager) {
    if (!manager) return;
    
    unload_current_game(manager);
    init_game_manager(manager);
}

// Game state queries
bool is_game_loaded(const GameManager* manager) {
    return manager && manager->game_loaded;
}

bool is_game_initialized(const GameManager* manager) {
    return manager && manager->game_initialized;
}

void* get_current_game_state(const GameManager* manager) {
    return (manager && manager->game_loaded) ? manager->current_game_state : NULL;
}

const GameInterface* get_current_game_interface(const GameManager* manager) {
    return (manager && manager->game_loaded) ? manager->current_game_interface : NULL;
}

GameType get_current_game_type(const GameManager* manager) {
    return (manager && manager->game_loaded) ? manager->current_game_type : GAME_TYPE_COUNT;
}

// Game lifecycle management
bool init_current_game(GameManager* manager) {
    if (!manager || !manager->game_loaded || 
        !manager->current_game_interface || 
        !manager->current_game_state) {
        return false;
    }
    
    if (manager->current_game_interface->init_game) {
        manager->current_game_interface->init_game(manager->current_game_state);
        manager->game_initialized = true;
        return true;
    }
    
    return false;
}

void reset_current_game(GameManager* manager) {
    if (!manager || !manager->game_loaded || 
        !manager->current_game_interface || 
        !manager->current_game_state) {
        return;
    }
    
    if (manager->current_game_interface->reset_game) {
        manager->current_game_interface->reset_game(manager->current_game_state);
    }
}

void update_current_game(GameManager* manager, double delta_time) {
    if (!manager || !manager->game_loaded || 
        !manager->current_game_interface || 
        !manager->current_game_state) {
        return;
    }
    
    manager->frame_delta = delta_time;
    
    if (manager->current_game_interface->update_game) {
        manager->current_game_interface->update_game(manager->current_game_state, delta_time);
    }
}

// Utility functions
const char* get_game_name(GameType game_type) {
    const GameInterface* interface = get_game_interface(game_type);
    return interface ? interface->game_name : "Unknown Game";
}

const char* get_game_description(GameType game_type) {
    const GameInterface* interface = get_game_interface(game_type);
    return interface ? interface->game_description : "No description available";
}

bool is_valid_game_type(GameType game_type) {
    return (game_type >= 0 && game_type < GAME_TYPE_COUNT);
}

// Game interface registration and retrieval
void register_game_interface(GameType game_type, const GameInterface* interface) {
    if (is_valid_game_type(game_type) && interface) {
        game_registry[game_type] = interface;
    }
}

const GameInterface* get_game_interface(GameType game_type) {
    if (is_valid_game_type(game_type)) {
        return game_registry[game_type];
    }
    return NULL;
}