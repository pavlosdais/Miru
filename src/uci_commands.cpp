#include <string.h>
#include "../include/common.hpp"
#include "../include/uci_commands.hpp"
#include "../include/move_generation.hpp"
#include "../include/search.hpp"
#include "../include/evaluation.hpp"

#define UCI_OK "readyok\n"

inline int parse_move(char* pmove, gameBoard& board)
{
    // generate possible moves
    _move_list generated_moves[1];
    get_moves(board, generated_moves, 0, 0);

    int source_square = (pmove[0] - 'a') + (8 - (pmove[1] - '0')) * 8, target_square = (pmove[2] - 'a') + (8 - (pmove[3] - '0')) * 8;
    
    for (int move_num = 0; move_num != generated_moves->number_of_moves; move_num++)
    {
        int move = generated_moves->moves[move_num].move;
        
        if (source_square == get_source_square(move) && target_square == get_target_square(move))
        {
            int promoted_piece = get_promoted_piece(move);
            
            if (promoted_piece)
            {
                if ((promoted_piece == Q || promoted_piece == q) && pmove[4] == 'q') return move;
                else if ((promoted_piece == R || promoted_piece == r) && pmove[4] == 'r') return move;
                else if ((promoted_piece == B || promoted_piece == b) && pmove[4] == 'b') return move;
                else if ((promoted_piece == N || promoted_piece == n) && pmove[4] == 'n') return move;
                else break;  // invalid move
            }
            return move;
        }
    }
    
    return 0;
}

inline void parse_position(char* position, gameBoard& board)
{
    position += 9;  // parse "position "

    char* position_cpy = position;

    if (strncmp(position, "startpos", 8) == 0)
        board.get_fen_position(fen_starting_position);
    else
    {
        if ((position_cpy = strstr(position, "fen")) == NULL)
            board.get_fen_position(fen_starting_position);
        else
        {
            position_cpy += 4;
            board.get_fen_position(position_cpy);
        }
    }
    
    if ((position_cpy = strstr(position, "moves")) != NULL)
    {
        // parse "moves "
        position_cpy += 6;

        while(*position_cpy)
        {
            int move = parse_move(position_cpy, board);
            
            if (move == 0)  // illegal move
            {
                cout << "Illegal move\n";
                break;
            }
            
            board.add_to_history();  // add position to the history

            make_move(board, move, all_moves);  // play move
            
            // get next move
            while (*position_cpy && *position_cpy != ' ') position_cpy++;
            position_cpy++;
        }
    }
}

inline void parse_go(char* command, gameBoard& board)
{
    short depth = 128;
    char *argument = NULL;

    if ((argument = strstr(command,"infinite"))) {}

    if ((argument = strstr(command, "binc")) && board.side_to_move == black)
        board.inc = atoi(argument + 5);

    if ((argument = strstr(command, "winc")) && board.side_to_move == white)
        board.inc = atoi(argument + 5);

    if ((argument = strstr(command, "wtime")) && board.side_to_move == white)
        board.time = atoi(argument + 6);

    if ((argument = strstr(command, "btime")) && board.side_to_move == black)
        board.time = atoi(argument + 6);

    if ((argument = strstr(command, "movestogo")))
        board.movestogo = atoi(argument + 10);

    if ((argument = strstr(command, "movetime")))
        board.movetime = atoi(argument + 9);

    if ((argument = strstr(command, "depth")))
        depth = atoi(argument + 6);
    
    if (board.movetime != -1)
    {
        board.time = board.movetime;
        board.movestogo = 1;
    }
    board.starttime = get_time();

    if (board.time != -1)
    {
        board.timeset = 1;
        board.time /= board.movestogo;
        board.time -= 50;
        board.stoptime = board.starttime + board.time + board.inc;
    }

    cout << "time:"<< board.time << " start:" << board.starttime << " stop:" << board.stoptime << " depth:" << depth << " timeset:" << board.timeset << endl;

    // start the search
    best_move(&board, depth);
}

void uci(gameBoard& board)
{
    setbuf(stdin, NULL); setbuf(stdout, NULL);
    char input[1500];
    
    // print info about the engine
    cout << "id name " << ENGINE_NAME << ENGINE_VERSION << endl;
    cout << UCI_OK;

    while (true)
    {
        memset(input, 0, sizeof(input)); fflush(stdout);

        // get user or GUI input
        if (!fgets(input, 1500, stdin) || input[0] == '\n')
            continue;

        if (strncmp(input, "ucinewgame", 10) == 0)  // start new game
        {
            parse_position((char*)"position startpos", board);
            tt_clear(board.TT);
        }
        else if (strncmp(input, "exit", 4) == 0)  // exit
            break;
        else if (strncmp(input, "isready", 7) == 0)  // is ready
            cout << UCI_OK;
        else if (strncmp(input, "position", 8) == 0)  // parse position
        {
            parse_position(input, board);
            tt_clear(board.TT);
        }
        else if (strncmp(input, "go", 2) == 0)  // start search
            parse_go(input, board);
        else if (strncmp(input, "uci", 3) == 0)  // print info
        {
            cout << "id name " << ENGINE_NAME << ENGINE_VERSION << endl;
            cout << UCI_OK;
        }
    }
}
