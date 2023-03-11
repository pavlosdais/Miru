#include "../include/board.hpp"
#include "../include/utilities.hpp"
#include "../include/move_generation.hpp"
#include "../include/uci_commands.hpp"

int main(void)
{
    // initialize starting board
    gameBoard* board = new gameBoard();

    // UCI loop
    uci(*board);

    delete board;
    return 0;
}