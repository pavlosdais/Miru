#include <cstring>
#include <stdlib.h>
#include "../include/board.hpp"
#include "../include/utilities.hpp"
#include "../include/attacks.hpp"
#include "../include/move_generation.hpp"
#include "../include/search.hpp"

#define NUM_OF_TESTS 6

#define PERFT_MAX_DEPTH 4
#define SEARCH_MAX_DEPTH 12

#define START_TEST 0
#define END_TEST NUM_OF_TESTS

void search(gameBoard& curr_board, sint depth, unsigned long& nodes)
{
    if (depth == 0)
    {
        nodes++;
        return;
    }

    _move_list generated_moves[1]; get_moves(curr_board, generated_moves, 0, 0);

    for (short move_number = 0; move_number != generated_moves->number_of_moves; move_number++)
    {
        copy_board_s(curr_board);

        if(!make_move(curr_board, generated_moves->moves[move_number].move, all_moves))
            continue;
        
        search(curr_board, depth-1, nodes);

        revert_board_s(curr_board);
    }
}

unsigned long perft_test(gameBoard& curr_board, sint depth)
{
    unsigned long nodes_searched = 0;

    search(curr_board, depth, nodes_searched);

    return nodes_searched;
}

static void print_in_red()
{
    printf("\033[1;31m");
}

static void print_in_green()
{
    printf("\033[0;32m");
}

static void reset_color()
{
    printf("\033[0m");
}

static inline std::vector<string> get_tests()
{
    std::vector<string> perft_tests;
    perft_tests.push_back(fen_pos1); perft_tests.push_back(fen_pos2); perft_tests.push_back(fen_pos3);
    perft_tests.push_back(fen_pos4); perft_tests.push_back(fen_pos5); perft_tests.push_back(fen_pos6);
    return perft_tests;
}

void perft_tests(gameBoard& curr_board)
{
    std::vector<string> perft_tests = get_tests();

    // the results of the perft tests
    unsigned long results[6][9];
    
    // test 1
    results[0][0] = 20; results[0][1] = 400; results[0][2] = 8902; results[0][3] = 197281; results[0][4] = 4865609; results[0][5] = 119060324; results[0][6] = 3195901860;
    // test 2
    results[1][0] = 48; results[1][1] = 2039; results[1][2] = 97862; results[1][3] = 4085603; results[1][4] = 193690690; results[1][5] = 8031647685;
    // test 3
    results[2][0] = 14; results[2][1] = 191; results[2][2] = 2812; results[2][3] = 43238; results[2][4] = 674624; results[2][5] = 11030083; results[2][6] = 178633661;
    // test 4
    results[3][0] = 6; results[3][1] = 264; results[3][2] = 9467; results[3][3] = 422333; results[3][4] = 15833292; results[3][5] = 706045033;
    // test 5
    results[4][0] = 44; results[4][1] = 1486; results[4][2] = 62379; results[4][3] = 2103487; results[4][4] = 89941194;
    // test 6
    results[5][0] = 46; results[5][1] = 2079; results[5][2] = 89890; results[5][3] = 3894594; results[5][4] = 164075551; results[5][5] = 6923051137; results[5][6] = 287188994746;

    int start_time = get_time();
    for (sint test = START_TEST; test < END_TEST; test++)
    {
        cout << "test " << test+1 << endl;
        curr_board.get_fen_position(perft_tests[test]);
        bool passed = true;

        for (sint depth = 0; depth < PERFT_MAX_DEPTH; depth++)
        {
            unsigned long nodes_searched = perft_test(curr_board, depth+1);
            if (nodes_searched != results[test][depth])
            {
                cout << nodes_searched << " | " << results[test][depth] << " (" << ABS(nodes_searched, results[test][depth]) << ")" << endl;
                
                print_in_red();
                printf("Failed on depth %d\n", depth+1);
                
                passed = false;
                break;
            }
        }
        if (passed)
        {
            print_in_green();
            printf("Passed test\n");
        }
        reset_color();

        cout << endl;

        curr_board.resetBoard();
    }

    cout << "Perft test took " << get_time() - start_time << " ms to complete\n";
}

void search_tests(gameBoard* curr_board)
{
    std::vector<string> perft_tests = get_tests();

    int start_time = get_time();
    for (sint test = 0; test < NUM_OF_TESTS; test++)
    {
        print_in_green();
        cout << "test " << test+1 << endl;

        curr_board->get_fen_position(perft_tests[test]);

        reset_color();
        
        // clear transpotion table
        tt_clear(curr_board->TT);

        print_in_red();
        best_move(curr_board, SEARCH_MAX_DEPTH);
        cout << endl;

        curr_board->resetBoard();
        cout << endl << endl;
    }

    reset_color();

    cout << "Search test took " << get_time() - start_time << " ms to complete\n";
}
