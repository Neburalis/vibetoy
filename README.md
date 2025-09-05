# VibeToy - Tic-Tac-Toe Console Game

A terminal-based tic-tac-toe game implemented in C++ using the termbox2 library, featuring innovative free-form cursor navigation inspired by Stone Story RPG.

## Features

- **Inverted cursor navigation**: Stone Story RPG-style cursor that inverts colors at its position (white background, black text), making it always visible
- **Free-form movement**: Cursor can move freely across the entire screen
- **Hover-based interaction**: Menu items and game cells highlight when cursor hovers over them
- **Visual feedback**: Clickable elements change color when hovered (cyan background)
- **Interactive game board**: Game cells highlight in yellow when cursor is positioned over them
- **Main menu with options**: Start new game, continue, or quit
- **Visual game board**: Clear X/O markings with hover highlighting
- **Win/draw detection**: Automatic game state detection
- **Restart and menu navigation**: Options available during gameplay and game over

## Building

Make sure the termbox2 git submodule is initialized:

```bash
git submodule update --init --recursive
```

Then build the game:

```bash
make
```

## Running

```bash
./tictactoe
```

## Controls

### Universal Controls (All States)
- ↑↓←→ or WASD - Move inverted cursor around the screen
- Enter - Activate/click on highlighted element

### Main Menu
- Navigate cursor over menu items to highlight them
- Enter - Select highlighted menu option
- N - Quick new game
- C - Quick continue (if game in progress)
- Q - Quick quit

### During Game
- Navigate cursor over game board cells to highlight them
- Enter - Place mark (X or O) on highlighted cell
- R - Restart game
- M - Return to main menu
- Q - Quit

### Game Over Screen
- Navigate cursor over options to highlight them
- Enter - Select highlighted option
- R - Quick restart
- M - Quick return to main menu
- Q - Quick quit

## Game Rules

- Players alternate between X and O
- First player is X
- Win by getting 3 in a row (horizontal, vertical, or diagonal)
- Game ends in draw if board is full with no winner

## Architecture

The game is implemented using functional programming principles without classes:

- `src/main.cpp` - Main application loop and termbox2 initialization
- `src/game.cpp` - Core game logic functions
- `src/menu.cpp` - Menu system and input handling
- `src/render.cpp` - Screen rendering functions
- `src/*.h` - Header files with function declarations and data structures

## Dependencies

- C++17 compatible compiler (g++)
- termbox2 library (included as git submodule at `lib/termbox2`)
- POSIX-compatible system (Linux, macOS)
- Make build system

## Technical Details

- **Language**: C++17 with functional programming style (no classes)
- **UI Library**: termbox2 for cross-platform terminal user interface
- **Build System**: Makefile with compiler flags: `-Wall -Wextra -std=c++17 -Ilib`
- **Architecture**: Modular functional design with explicit state passing
- **Input System**: Event-driven input handling with global cursor state

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

This is a copyleft license that allows you to use, modify, and distribute the code, but requires that any derivative works also be released under the GPL-3.0 license.

## Contributing

Contributions are welcome! Please ensure your code follows the functional programming style used in this project (no classes, explicit state passing).