# IPC-Battleship

## Overview
This project is a text-based version of the classic **Battleship** game. It utilizes **shared memory** for inter-process communication and **ncurses** for a simple graphical interface in the terminal. The game allows two players to play against each other, place ships on a grid, take turns to attack, and save/load the game state.

## Features
- **Shared Memory**: The game state is stored in shared memory, allowing communication between multiple processes (e.g., for AI and player interactions).
- **AI Opponent**: A simple AI opponent that makes moves based on the last successful hit.
- **Grid Display**: The game uses **ncurses** to display grids in the terminal with color-coded ships and attack statuses.
- **Save/Load Functionality**: The game state can be saved to a file and loaded later, allowing players to resume their game.
- **Ship Placement**: Players' ships (Battleship, Cruiser, Destroyer) are randomly placed on the grid.

## Dependencies
- **ncurses**: Used for creating the terminal user interface.
- **Shared Memory**: To store the game state, enabling inter-process communication.

## Getting Started
To run this project, you need to have **ncurses** installed on your system. You can install it using the following command on **Linux-based systems**:
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```

You can compile the project using gcc:

```
gcc -o battleship battleship.c -lncurses
```

To start the game, run the following command:

```
./battleship
```

## Game Menu
The game will display a menu with the following options:
- **Start New Game**: Begin a fresh game with random ship placements.
- **Show Grids**: Displays the current state of both players' grids.
- **Relocate Ships**: Randomly places ships again for both players.
- **Save and Exit**: Saves the current game state to a file and exits the program.
- **Load Saved Game**: Load the game state from a previously saved file to continue playing.
- **Display Saved Game State**: View the last saved game state.

## AI Behavior
- The **AI opponent** chooses random coordinates to attack if no successful hits have been made.
- If the **AI** hits a ship, it attempts to continue attacking nearby coordinates based on the last successful hit.

## Game State
The game state includes:
- **Grid1 and Grid2**: Represent the two players' grids.
- **Coordinates Hit by Player1 and Player2**: Tracks successful hits.
- **Current Player**: Indicates whose turn it is (Player 1 or Player 2).
- **Game Over Flag**: Indicates if the game is over.
- **Player Attacks**: A matrix tracking the attacks made by each player on the opponent's grid.
