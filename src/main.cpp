#define TB_IMPL
#include "game.h"
#include "menu.h"
#include "render.h"
#include <stdio.h>

int main() {
    // Initialize termbox2
    if (tb_init() != 0) {
        fprintf(stderr, "Failed to initialize termbox\n");
        return 1;
    }
    
    // Enable mouse input and hide system cursor
    tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    tb_hide_cursor();
    
    ApplicationState app;
    init_application(&app);
    
    // Main game loop
    while (app.current_state != STATE_QUIT) {
        clear_screen();
        
        // Update hover state before rendering
        update_hover_state(&app);
        
        // Render current state
        switch (app.current_state) {
            case STATE_MAIN_MENU:
                render_main_menu_with_hover(&app);
                break;
                
            case STATE_MODE_SELECTION:
                render_mode_selection_with_hover(&app);
                break;
                
            case STATE_DIFFICULTY_SELECTION:
                render_difficulty_selection_with_hover(&app);
                break;
                
            case STATE_PLAYING:
                render_game_ui(&app.game);
                render_game_board_with_hover(&app);
                
                // Render AI-specific elements for single player mode
                if (app.game_mode == MODE_SINGLE_PLAYER) {
                    render_player_indicators(&app);
                    render_ai_turn_indicator(&app);
                    render_ai_thinking_animation(&app);
                }
                
                render_controls(STATE_PLAYING);
                break;
                
            case STATE_GAME_OVER:
                render_game_ui(&app.game);
                render_game_board_with_hover(&app);
                
                // Render AI-specific elements for single player mode
                if (app.game_mode == MODE_SINGLE_PLAYER) {
                    render_player_indicators(&app);
                }
                
                render_game_over_with_hover(&app);
                break;
                
            case STATE_QUIT:
                break;
        }
        
        // Render global cursor on top of everything
        render_global_cursor(&app);
        
        present_screen();
        
        // Handle input
        struct tb_event event;
        if (tb_poll_event(&event) == TB_OK) {
            if (event.type == TB_EVENT_MOUSE) {
                // Update cursor position to mouse coordinates
                set_cursor_position(&app, event.x, event.y);
                
                // Handle mouse clicks
                if (event.key == TB_KEY_MOUSE_LEFT) {
                    handle_cursor_click(&app);
                }
            } else if (event.type == TB_EVENT_KEY) {
                switch (app.current_state) {
                    case STATE_MAIN_MENU:
                        handle_menu_input(&app, &event);
                        break;
                        
                    case STATE_MODE_SELECTION:
                        handle_mode_selection_input(&app, &event);
                        break;
                        
                    case STATE_DIFFICULTY_SELECTION:
                        handle_difficulty_selection_input(&app, &event);
                        break;
                        
                    case STATE_PLAYING:
                        handle_game_input(&app, &event);
                        break;
                        
                    case STATE_GAME_OVER:
                        // Game over has its own input handling
                        handle_game_over_input(&app, &event);
                        break;
                        
                    case STATE_QUIT:
                        break;
                }
            }
        }
        
        // Process AI turn if needed
        if (app.current_state == STATE_PLAYING && app.game_mode == MODE_SINGLE_PLAYER) {
            if (app.game.current_player == app.ai_player && app.game.game_active && !app.ai_thinking) {
                trigger_ai_move(&app);
            }
            
            if (app.ai_thinking) {
                process_ai_turn(&app);
            }
        }
    }
    
    tb_shutdown();
    return 0;
}