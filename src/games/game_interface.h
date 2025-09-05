#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdbool.h>
#include <stddef.h>

// Forward declarations
struct tb_event;

// Core game interface structure
typedef struct GameInterface {
    // Game identification
    const char* game_name;
    const char* game_description;
    
    // Core game lifecycle functions
    void (*init_game)(void* game_state);
    void (*reset_game)(void* game_state);
    void (*update_game)(void* game_state, double delta_time);
    bool (*is_game_active)(const void* game_state);
    bool (*is_game_over)(const void* game_state);
    
    // Input handling
    bool (*handle_input)(void* game_state, const struct tb_event* event, 
                        const void* cursor);
    bool (*handle_cursor_click)(void* game_state, int x, int y);
    
    // Rendering
    void (*render_game)(const void* game_state, int screen_width, int screen_height);
    void (*render_game_ui)(const void* game_state);
    
    // Hover detection for cursor system
    bool (*update_hover_state)(void* game_state, const void* cursor);
    
    // Game state queries
    bool (*has_winner)(const void* game_state);
    bool (*is_draw)(const void* game_state);
    const char* (*get_status_text)(const void* game_state);
    const char* (*get_winner_text)(const void* game_state);
    
    // Memory management
    size_t game_state_size;
    void (*cleanup_game)(void* game_state);
} GameInterface;

// Game type enumeration
typedef enum {
    GAME_TYPE_TICTACTOE,
    GAME_TYPE_TETRIS,
    GAME_TYPE_SNAKE,
    GAME_TYPE_COUNT
} GameType;

// Function to get game interface by type
const GameInterface* get_game_interface(GameType game_type);

// Game registration function (to be called by each game module)
void register_game_interface(GameType game_type, const GameInterface* interface);

#endif