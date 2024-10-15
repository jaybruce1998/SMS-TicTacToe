#include "SMSlib.h"

static char board[3][3];
static int currentPlayer = 1; // 1 for player 'X', -1 for player 'O'
static int selectedX = 0, selectedY = 0, winner, movedBefore = 0, moved;

/* Initialize the Tic-Tac-Toe board */
void initialize_board(void) {
    currentPlayer = 1;
    winner = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            board[y][x] = ' ';
        }
    }
    SMS_printatXY(10, 16, "            "); // Clear win message area
}

/* Draw the Tic-Tac-Toe board */
void draw_board(void) {
    for (int y = 0; y < 3; y++) {
        SMS_printatXY(9, 10 + y * 2, " "); // Padding
        for (int x = 0; x < 3; x++) {
            SMS_printatXY(10 + x * 2, 10 + y * 2, board[y][x] == ' ' ? " " : (board[y][x] == 'X' ? "X" : "O")); // Print symbol

            if (x < 2) {
                SMS_printatXY(11 + x * 2, 10 + y * 2, "|");
            }
        }
        SMS_printatXY(15, 10 + y * 2, " "); // Padding

        if (y < 2) {
            SMS_printatXY(10, 11 + y * 2, "-----");
        }
    }

    // Highlight the selected cell (with square brackets)
    SMS_printatXY(10 + selectedX * 2 - 1, 10 + selectedY * 2, "[");
    SMS_printatXY(10 + selectedX * 2 + 1, 10 + selectedY * 2, "]");
}

/* Check for a win or draw */
int check_winner(void) {
    for (int i = 0; i < 3; i++) {
        // Check rows and columns
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];

    // Check for draw
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (board[y][x] == ' ') {
                return 0; // Still playing
            }
        }
    }

    return 'D'; // Draw
}

/* Handle player move */
void place_marker(void) {
    if (board[selectedY][selectedX] == ' ') {
        board[selectedY][selectedX] = currentPlayer == 1 ? 'X' : 'O';
        currentPlayer = -currentPlayer; // Switch player
    }
}

/* Handle player input for navigation and selecting cells */
void handle_input(void) {
    unsigned int value = SMS_getKeysPressed();
    moved = 0;
    if (value & PORT_A_KEY_LEFT) {
        moved = 1;
        if (!movedBefore)
            selectedX = selectedX == 0 ? 2 : selectedX - 1;
    }
    if (value & PORT_A_KEY_RIGHT) {
        moved = 1;
        if (!movedBefore)
            selectedX = selectedX == 2 ? 0 : selectedX + 1;
    }
    if (value & PORT_A_KEY_UP) {
        moved = 1;
        if (!movedBefore)
            selectedY = selectedY == 0 ? 2 : selectedY - 1;
    }
    if (value & PORT_A_KEY_DOWN) {
        moved = 1;
        if (!movedBefore)
            selectedY = selectedY == 2 ? 0 : selectedY + 1;
    }
    movedBefore = moved;
    if ((value & PORT_A_KEY_1) && !winner) {
        place_marker();
    }
    if ((value & PORT_A_KEY_2) && winner) {
        initialize_board();
    }
}

/* Main game loop */
void my_game_loop(void) {
    handle_input();
    draw_board();

    winner = check_winner();
    if (winner == 'X') {
        SMS_printatXY(10, 16, "X Wins!");
    } else if (winner == 'O') {
        SMS_printatXY(10, 16, "O Wins!");
    } else if (winner == 'D') {
        SMS_printatXY(10, 16, "It's a draw!");
    }
}

/* Main entry point */
void main(void) {
    // Set up text mode
    SMS_VRAMmemsetW(0, 0x0000, 16384); // Clear VRAM
    SMS_autoSetUpTextRenderer();       // Set up font and text rendering

    SMS_printatXY(10, 1, "Tic-Tac-Toe");

    initialize_board();

    while (1) {
        my_game_loop();
        SMS_waitForVBlank(); // Sync with the screen refresh
    }
}

SMS_EMBED_SEGA_ROM_HEADER(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE(1,0,"TicTacToe_SMS","Tic-Tac-Toe Game","Simple Tic-Tac-Toe for SMS");
