#include <string.h>
#include "../include/common.hpp"
#include "../include/uci_commands.hpp"
#include "../include/move_generation.hpp"
#include "../include/search.hpp"

#define UCI_OK "readyok\n"

inline int parse_move(char* pmove, gameBoard& board)
{
    // get all legal moves
    int source_square = get_square((pmove[0]-'a'), (pmove[1]-'0')), target_square = get_square((pmove[2]-'a'), (pmove[3]-'0'));

    _move_list generated_moves[1];
    generate_moves(board, generated_moves, 0, 0);
    
    int move_len = strlen(pmove);
    bool is_promotion = (move_len == 6 || move_len == 5);

    for (int move = 0; move != generated_moves->number_of_moves; move++)
    {
        int promote = get_promoted_piece(generated_moves->moves[move].move);

        string str_move = square_to_coordinates[get_source_square(generated_moves->moves[move].move)] +
                          square_to_coordinates[get_target_square(generated_moves->moves[move].move)];
        
        // same moves
        if (pmove[0] == str_move[0] && pmove[1] == str_move[1] && pmove[2] == str_move[2] && pmove[3] == str_move[3])
        {
            if (!promote)
            {
                make_move(board, generated_moves->moves[move].move, all_moves);
                return true;
            }

            if (is_promotion)
            {
                if (pmove[4] == find_promoted_piece(promote))
                {
                    make_move(board, generated_moves->moves[move].move, all_moves);
                    return true;
                }
            }
        }
    }
    return false;
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
        position_cpy+=6;

        char* p = new char[10];
        while (*position_cpy)
        {
            int i = 0;
            while (*position_cpy && *position_cpy != ' ')
            {
                p[i++] = *position_cpy;
                position_cpy++;
            }
            p[i] = '\0';

            board.History[++(board.rep_index)] = board.hash_position;

            if (parse_move(p, board) == false)
            {
                cout << "Error whilst trying to play the move" << endl;
                break;
            }

            position_cpy++;
        }
        delete[] p;
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

    // search position
    best_move(&board, depth);
}

void uci(gameBoard& board)
{
    setbuf(stdin, NULL); setbuf(stdout, NULL);
    char* input = new char[1500];
    
    cout << "id name " << ENGINE_NAME << ENGINE_VERSION << endl;
    cout << UCI_OK;

    while (true)
    {
        memset(input, 0, sizeof(input)); fflush(stdout);

        // get user or GUI input
        if (!fgets(input, 1500, stdin) || input[0] == '\n')
            continue;

        if (strncmp(input, "ucinewgame", 10) == 0)
        {
            parse_position((char*)"position startpos", board);
            tt_clear(board.TT);
        }
        else if (strncmp(input, "exit", 4) == 0)
            break;
        else if (strncmp(input, "isready", 7) == 0)
            cout << UCI_OK;
        else if (strncmp(input, "position", 8) == 0)
        {
            parse_position(input, board);
            tt_clear(board.TT);
        }
        else if (strncmp(input, "go", 2) == 0)
            parse_go(input, board);
        else if (strncmp(input, "uci", 3) == 0)
        {
            cout << "id name " << ENGINE_NAME << ENGINE_VERSION << endl;
            cout << UCI_OK;
        }
    }
    delete[] input;
}
