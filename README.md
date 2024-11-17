# IPC-Battleship
BattleShip Game with Shared Memory
Overview
This project is a text-based version of the classic Battleship game. It utilizes shared memory for inter-process communication and ncurses for a simple graphical interface in the terminal. The game allows two players to play against each other, placing ships on a grid, taking turns to attack, and saving/loading the game state.

Features
Shared Memory: The game state is stored in shared memory, allowing communication between multiple processes (e.g., for AI and player interactions).
AI Opponent: A simple AI opponent that makes moves based on the last successful hit.
Grid Display: The game uses ncurses to display grids in the terminal with color-coded ships and attack statuses.
Save/Load Functionality: The game state can be saved to a file and loaded later, so players can resume their game.
Ship Placement: Players’ ships (Battleship, Cruiser, Destroyer) are randomly placed on the grid.
Dependencies
ncurses: Used for creating the terminal user interface.
Shared Memory: To store the game state, enabling inter-process communication.

To run this project, make sure you have ncurses installed on your system. You can install it using the following command on Linux-based systems:

sudo apt-get install libncurses5-dev libncursesw5-dev

Compilation
You can compile the project using gcc:

gcc -o battleship battleship.c -lncurses

Running the Game
To start the game, simply run:

./battleship

Start New Game: Begin a fresh game with new random ship placements.
Show Grids: Displays the current state of both players' grids.
Relocate Ships: Randomly places ships on the grid again.
Save and Exit: Saves the current game state to a file and exits the program.
Load Saved Game: Load the game state from a previously saved file to continue playing.
Display Saved Game State: View the last saved game state.

Game Mechanics
Each player has a grid (grid1 for Player 1, grid2 for Player 2).
Ships are represented by different characters (B for Battleship, C for Cruiser, D for Destroyer).
Players take turns to attack a grid, and the result of each attack is displayed.
The game ends when one player successfully hits all the parts of the opponent's ships.

AI Behavior
The AI opponent chooses random coordinates to attack if no successful hits have been made.
If the AI hits a ship, it tries to continue attacking nearby coordinates based on the previous successful hit.

License
This project is licensed under the MIT License.

