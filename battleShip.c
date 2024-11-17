/*
                                         Project Group 2
        UTKU AKKUŞOĞLU
        MEHMET GÖKGÜL 
        EFE BEKİR ALTOP
        ALİ BATUHAN ZEYTÜN
        BATUHAN TUNÇER
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define GRID_SIZE 8

typedef struct {     // shared memory items
    char grid1[GRID_SIZE][GRID_SIZE];
    char grid2[GRID_SIZE][GRID_SIZE];
    int coordinatesHitBy1;
    int coordinatesHitBy2;
    int isOver;
    int currentPlayer;
    int isFirstStart;
    int player1Attacks[GRID_SIZE][GRID_SIZE];
    int player2Attacks[GRID_SIZE][GRID_SIZE];
} GameState;

typedef struct {
    int size;
    char symbol;
} Ship;

void createInitialGrid(char grid[GRID_SIZE][GRID_SIZE]);
void displayGrid(char grid[GRID_SIZE][GRID_SIZE]);
int canPlaceShip(char grid[GRID_SIZE][GRID_SIZE], int x, int y, int size, int isHorizontal);
void placeShip(char grid[GRID_SIZE][GRID_SIZE], Ship ship);
void saveGameState(GameState *game_state);
void loadGameState(GameState *game_state);
void displaySavedGameState();
void showMenu();
void startGame(GameState* game_state);
void showGrids(GameState* game_state);
void relocateShips(GameState* game_state);
void exitProgram(int shm_fd, const char* name);
int* ai(char last_successful_hit[2], int playerAttacks[GRID_SIZE][GRID_SIZE]);


GameState *game_state;
const char *filename = "game_bs.dat";  

int main() {
    const int SIZE = sizeof(GameState);
    const char* name = "BattleShip";
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    game_state = mmap(0, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    game_state->isFirstStart = 1;
    srand(time(0));
    
    initscr();
    start_color();  
    init_pair(1, COLOR_BLUE, COLOR_BLACK);      // for Battlehip(B)
    init_pair(2, COLOR_GREEN, COLOR_BLACK);     // for Cruisers(C)
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);    // for Destroyers(D)
    init_pair(4, COLOR_RED, COLOR_BLACK);       // for hit mark(X)
    cbreak();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);  
    mousemask(0, NULL);

    int choice;

    while (1) {
        showMenu();
        printw("Enter choice: ");
        refresh();
        
        choice = getch() - '0';
        printw("\n");
        refresh();

        switch (choice) {
            case 1:
                startGame(game_state);
                break;
            case 2:
                showGrids(game_state);
                break;
            case 3:
                relocateShips(game_state);
                break;
            case 4:
                saveGameState(game_state);
                printw("Saving game and exiting...");
                refresh();
                exitProgram(shm_fd, name);
                refresh();
                return 0;
            case 5:
                loadGameState(game_state);
                break;
            case 6:
                displaySavedGameState();
                break;
            default:
                printw("Invalid choice. Please select again.");
                refresh();
                break;
        }
    }
    refresh();
}

void showMenu() {
    printw("\n--- Game Menu ---\n");
    printw("1. Start New Game\n");
    printw("2. Show Grids\n");
    printw("3. Relocate Ships\n");
    printw("4. Save Game and Exit\n");
    printw("5. Load Saved Game\n");
    printw("6. Display Saved Game State\n\n");
    refresh();
}

void createInitialGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '#';
        }
    }

    Ship ships[] = { {4, 'B'}, {3, 'C'}, {3, 'C'}, {2, 'D'}, {2, 'D'} };
    int num_ships = sizeof(ships) / sizeof(ships[0]);

    for (int i = 0; i < num_ships; i++) {
        placeShip(grid, ships[i]);
    }
}

int canPlaceShip(char grid[GRID_SIZE][GRID_SIZE], int x, int y, int size, int isHorizontal) {
    for (int i = 0; i < size; i++) {
        int nx = x + (isHorizontal ? 0 : i);
        int ny = y + (isHorizontal ? i : 0);
        if (nx >= GRID_SIZE || ny >= GRID_SIZE || grid[nx][ny] != '#') {
            return 0;
        }
        if ((nx > 0 && grid[nx - 1][ny] != '#') ||                                             // control of upper
            (nx < GRID_SIZE - 1 && grid[nx + 1][ny] != '#') ||                                 // control of down
            (ny > 0 && grid[nx][ny - 1] != '#') ||                                             // control of left
            (ny < GRID_SIZE - 1 && grid[nx][ny + 1] != '#') ||                                 // control of right
            (nx > 0 && ny > 0 && grid[nx - 1][ny - 1] != '#') ||                               // control of upper left
            (nx > 0 && ny < GRID_SIZE - 1 && grid[nx - 1][ny + 1] != '#') ||                   // control of upper right
            (nx < GRID_SIZE - 1 && ny > 0 && grid[nx + 1][ny - 1] != '#') ||                   // control of down left
            (nx < GRID_SIZE - 1 && ny < GRID_SIZE - 1 && grid[nx + 1][ny + 1] != '#')) {       // control of down right
            return 0;
        }
    }
    return 1;
}

void placeShip(char grid[GRID_SIZE][GRID_SIZE], Ship ship) {
    int x, y, isHorizontal;
    do {
        x = rand() % GRID_SIZE;
        y = rand() % GRID_SIZE;
        isHorizontal = rand() % 2;
    } while (!canPlaceShip(grid, x, y, ship.size, isHorizontal));

    for (int i = 0; i < ship.size; i++) {
        int nx = x + (isHorizontal ? 0 : i);
        int ny = y + (isHorizontal ? i : 0);
        grid[nx][ny] = ship.symbol;
    }
}

void displayGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 'B') {
                attron(COLOR_PAIR(1)); 
                printw("%c ", grid[i][j]);
                attroff(COLOR_PAIR(1));
            } else if (grid[i][j] == 'C') {
                attron(COLOR_PAIR(2)); 
                printw("%c ", grid[i][j]);
                attroff(COLOR_PAIR(2));
            } else if (grid[i][j] == 'D') {
                attron(COLOR_PAIR(3));  
                printw("%c ", grid[i][j]);
                attroff(COLOR_PAIR(3));
            } else if (grid[i][j] == 'X') {
                attron(COLOR_PAIR(4));  
                printw("%c ", grid[i][j]);
                attroff(COLOR_PAIR(4));
            } else {
                printw("%c ", grid[i][j]);
            }
        }
        printw("\n");
    }
    refresh();
}

void saveGameState(GameState *game_state) {
    FILE *file = fopen(filename, "wb");
    fwrite(game_state, sizeof(GameState), 1, file);
    fclose(file);
}

void loadGameState(GameState *game_state) {
    if (access(filename, F_OK) == -1) { 
        printw("No saved game found. Starting a new game.\n\n");
        refresh();
        return;
    }

    FILE *file = fopen(filename, "rb");
    if (file) {
        if (fread(game_state, sizeof(GameState), 1, file) != 1) {
            printw("Error loading game state. The file may be corrupted.\n\n");
            refresh();
        } else {
            printw("Game state loaded.\n\n");
            refresh();
        }
        fclose(file);
    } else {
        printw("Error opening file.\n\n");
        refresh();
    }
}

void displaySavedGameState() {
    FILE *file = fopen(filename, "rb");
    if (file) {
        fread(game_state, sizeof(GameState), 1, file);
        printw("\nSaved Player1's grid:\n");
        displayGrid(game_state->grid1);
        printw("Saved Player2's grid:\n");
        displayGrid(game_state->grid2);
        printw("Coordinates hit by Player1: %d\n", game_state->coordinatesHitBy1);
        printw("Coordinates hit by Player2: %d\n\n", game_state->coordinatesHitBy2);
        fclose(file);
        refresh();
    } else {
        printw("No saved game found.\n\n");
        refresh();
    }
}

void startGame(GameState* game_state) {
    if (game_state->isFirstStart == 1 || game_state->isOver == 1) {
        relocateShips(game_state);
        game_state->isOver = 0;
    }
    printw("\nPlayer1's initial grid:\n");
    displayGrid(game_state->grid1);
    printw("\nPlayer2's initial grid:\n");
    displayGrid(game_state->grid2);
    printw("-------------------------------------------------\n");
    refresh();
    
    pid_t pid = fork();
    srand(getpid());

    int last_shot_player1 = 0;         // Was player1's last hit successful?
    int last_shot_player2 = 0;         // Was player2's last hit successful?
    char last_successful_hit1[2];     // Coordinate of player1's last successful hit
    char last_successful_hit2[2];     // Coordinate of player2's last successful hit
    char last_hit_ship_by_player1;     // type of last hit ship(B, C, D) by player1
    char last_hit_ship_by_player2;     // type of last hit ship(B, C, D) by player2

    if (pid < 0) {
        fprintf(stderr, "Fork failed!\n");
        exit(1);
    } else if (pid == 0) {  
        while (game_state->isOver != 1) {
            if (game_state->currentPlayer == 2) {
                int x, y;
                if(last_shot_player2 == 1){
                    int *choice = ai(last_successful_hit2, game_state->player2Attacks);
                    x = choice[0];
                    y = choice[1];
                }
                else{
                    do {
                        x = rand() % GRID_SIZE;
                        y = rand() % GRID_SIZE;
                    }while (game_state->player2Attacks[x][y]);
                }

                printw("Player2's choice is (%d,%d) -> ", x, y);
                refresh();
                int hit = (game_state->grid1[x][y] != '#');
                game_state->player2Attacks[x][y] = 1;

                if (hit) {
                    last_shot_player2 = 1;
                    last_hit_ship_by_player2 = game_state->grid1[x][y];
                    game_state->grid1[x][y] = 'X';
                    last_successful_hit2[0] = x;
                    last_successful_hit2[1] = y;
                    game_state->coordinatesHitBy2++;
                    saveGameState(game_state);
                    printw("Player2 hit a ship part and game saved\n");
                    refresh();
                    displayGrid(game_state->grid1);
                } else {
                    printw("Player2 missed\n");
                    refresh();
                    last_shot_player2 = 0;
                }

                if (game_state->coordinatesHitBy2 == 14) {
                    game_state->isOver = 1;
                    printw("Player2 won!\n");
                }
                game_state->currentPlayer = 1;
            }
            sleep(1);
            refresh();
        }
        exit(0);
    } else {  
        while (game_state->isOver != 1) {
            if (game_state->currentPlayer == 1) {
                int x, y;
                if(last_shot_player1 == 1){
                    int *choice = ai(last_successful_hit1, game_state->player1Attacks);
                    x = choice[0];
                    y = choice[1];
                }
                else{
                    do {
                        x = rand() % GRID_SIZE;
                        y = rand() % GRID_SIZE;
                   } while (game_state->player1Attacks[x][y]);
                }

                printw("Player1's choice is (%d,%d) -> ", x, y);
                refresh();
                int hit = (game_state->grid2[x][y] != '#');
                game_state->player1Attacks[x][y] = 1;

                if (hit) {
                    last_shot_player1 = 1;
                    last_hit_ship_by_player1 = game_state->grid2[x][y];
                    game_state->grid2[x][y] = 'X';
                    last_successful_hit1[0] = x;
                    last_successful_hit1[1] = y;
                    game_state->coordinatesHitBy1++;
                    saveGameState(game_state);
                    printw("Player1 hit a ship part and game saved\n");
                    refresh();
                    displayGrid(game_state->grid2);
                } else {
                    printw("Player1 missed\n");
                    refresh();
                    last_shot_player1 = 0;
                }

                if (game_state->coordinatesHitBy1 == 14) {
                    game_state->isOver = 1;
                    printw("Player1 won!\n");
                }
                game_state->currentPlayer = 2;
            }
            sleep(1);
            refresh();
        }
        wait(NULL);
    }
}

void showGrids(GameState* game_state) {
    printw("\nPlayer1's grid:\n");
    displayGrid(game_state->grid1);
    printw("\nPlayer2's grid:\n");
    displayGrid(game_state->grid2);
    printw("-------------------------------------------------\n\n");
    refresh();
}

void relocateShips(GameState* game_state) {
    createInitialGrid(game_state->grid1);
    createInitialGrid(game_state->grid2);
    game_state->coordinatesHitBy1 = 0;
    game_state->coordinatesHitBy2 = 0;
    game_state->isOver = 0;
    game_state->currentPlayer = 1;
    game_state->isFirstStart = 0;
    memset(game_state->player1Attacks, 0, sizeof(game_state->player1Attacks));
    memset(game_state->player2Attacks, 0, sizeof(game_state->player2Attacks));
    printw("Ships located!\n\n");
    refresh();
}

int* ai(char last_successful_hit[2], int playerAttacks[GRID_SIZE][GRID_SIZE]){
    int x = last_successful_hit[0];
    int y = last_successful_hit[1];
    int* choice = malloc(2 * sizeof(int));
    if(x + 1 < GRID_SIZE && playerAttacks[x + 1][y] != 1){
        x++; 
    } 
    else if(x - 1 >= 0 && playerAttacks[x - 1][y] != 1){
        x--;   
    }
    else if(y + 1 < GRID_SIZE && playerAttacks[x][y + 1] != 1){
        y++;
    }  
    else if(y - 1 >= 0 && playerAttacks[x][y - 1] != 1){
         y--;
    }
    else{
        do {
            x = rand() % GRID_SIZE;
            y = rand() % GRID_SIZE;
        } while (playerAttacks[x][y]);
    }
    choice[0] = x;
    choice[1] = y;
    return choice;
}

void exitProgram(int shm_fd, const char* name) {
    endwin();  
    printw("Exiting...\n");
    refresh();  
    close(shm_fd);  
    shm_unlink(name);  
    exit(0); 
}
