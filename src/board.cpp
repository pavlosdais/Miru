#include <cstring>
#include "../include/board.hpp"
#include "../include/utilities.hpp"
#include "../include/attacks.hpp"

gameBoard:: gameBoard()
{
    // create the attack tables
    create_attack_tables();

    // allocate memory for the killer moves
    try
    {
        killer_moves = new Move_Type*[NUM_OF_KILLER_MOVES];
        if (killer_moves == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    for (sint killer_move = 0; killer_move < NUM_OF_KILLER_MOVES; killer_move++)
    {
        try
        {
            killer_moves[killer_move] = new Move_Type[MAX_PLY];
            if (killer_moves[killer_move] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }

        for (sint j = 0; j < MAX_PLY; j++) killer_moves[killer_move][j] = 0;
    }

    // allocate memory for the history moves
    try
    {
        history_moves = new Move_Type*[NUM_OF_PIECES];
        if (history_moves == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }
    for (sint piece = 0; piece < NUM_OF_PIECES; piece++)
    {
        try
        {
            history_moves[piece] = new Move_Type[SQUARES];
            if (history_moves[piece] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }

        for (sint j = 0; j < SQUARES; j++) history_moves[piece][j] = 0;
    }

    // allocate memory for the PV length
    try
    {
        pv_length = new Move_Type[SQUARES];
        if (pv_length == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }
    for (sint i = 0; i < NUM_OF_PIECES; i++)
        pv_length[i] = 0;

    // allocate memory for the PV table
    try
    {
        pv_table = new Move_Type*[SQUARES];
        if (pv_table == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    for (sint square = 0; square < SQUARES; square++)
    {
        try
        {
            pv_table[square] = new Move_Type[SQUARES];
            if (pv_table[square] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }

        for (sint j = 0; j < SQUARES; j++) pv_table[square][j] = 0;
    }

    // initialize time control variables for search
    this->quit = this->starttime = this->inc = this->stopped = this->timeset = this->stoptime = 0;
    this->movestogo = 30;
    this->movetime = time = -1;

    // create transposition table
    hash_position = EMPTY_BOARD;
    init_zobrist_keys();
    create_transposition_table(&TT, hash_size);

    create_evaluation_tables();

    // reset board
    resetBoard();
}

gameBoard:: ~gameBoard()
{
    // free memory used for the killer/history moves
    for (sint i = 0; i < NUM_OF_KILLER_MOVES; i++)
        delete[] killer_moves[i];
    delete[] killer_moves;

    for (sint i = 0; i < NUM_OF_PIECES; i++)
        delete[] history_moves[i];

    delete[] history_moves;

    // free memory used for the principal variation table
    for (sint i = 0; i < SQUARES; i++)
        delete[] pv_table[i];
    delete[] pv_table;
    delete[] pv_length;

    // free memory used for the repetition table
    delete[] History;
    
    // free memory used for evaluation
    destroy_evaluation_tables();

    // free memory used for the transposition table
    destroy_transposition_table(TT);

    // free memory used for the zobrist keys
    destroy_zobrist_keys();

    // free memory used for the attack tables
    destroy_attack_tables();
}

void gameBoard:: get_fen_position(string fen_position)
{
    // reset board
    resetBoard();
    unsigned int i = 0;
    for (sint rank = 0; rank < RANKS; rank++)
    {
        for (sint file = 0; file < FILES; file++)
        {
            const sint square = get_square(rank, file);  // get corresponding square

            // a piece
            if (is_letter(fen_position[i]))
            {
                const sint piece = get_index(fen_position[i++]);  // get piece
                set_bit(bitboard[piece], square);  // place piece
            }

            // squares w/o pieces   
            if (is_digit(fen_position[i]))
            {
                file += fen_position[i++] - '0';
                
                char piece = -1;

                for (sint bb_piece = P, fp = k+1; bb_piece < fp; bb_piece++)
                {
                    // check to see if piece exists
                    if (get_bit(bitboard[bb_piece], square))
                    {
                        piece = bb_piece;
                        break;
                    }
                }
                if (piece == -1) file--;
            }
            if (fen_position[i] == '/') i++;
        }
    }
    i++;

    // side to move
    side_to_move = (fen_position[i] == 'w' || fen_position[i] == 'W')? white : black;

    i += 2;

    // castling rights
    while (fen_position[i] != ' ')
    {
        if (fen_position[i] == 'K')
            castle |= wk;
        else if (fen_position[i] == 'Q')
            castle |= wq;
        else if (fen_position[i] == 'k')
            castle |= bk;
        else if (fen_position[i] == 'q')
            castle |= bq;
        else if (fen_position[i] == '-')
            break;
        i++;
    }

    // enpassant square
    if (fen_position[++i] != '-')
    {
        const sint file = fen_position[i] - 'a';
        const sint rank = 8-(fen_position[i+1] - '0');

        enpassant = get_square(rank, file);
    }

    for (sint white_piece = P, last_piece = K+1; white_piece < last_piece; white_piece++)
        occupancy[white] |= bitboard[white_piece];
    
    for (sint black_piece = p, last_piece = k+1; black_piece < last_piece; black_piece++)
        occupancy[black] |= bitboard[black_piece];
    
    // place pieces
    occupancy[both] |= occupancy[white];
    occupancy[both] |= occupancy[black];

    hash_position = get_hash_position();
}

void gameBoard:: resetBoard()
{
    // reset piece placement
    for (sint piece = P, fin_piece = k+1; piece < fin_piece; piece++)
        bitboard[piece] = EMPTY_BOARD;
    
    for (sint pl = white, fin_piece = both+1; pl < fin_piece; pl++)
        occupancy[pl] = EMPTY_BOARD;

    for (sint i = 0; i < REPETITION_TABLE_SIZE; i++)
        History[i] = EMPTY_BOARD;

    rep_index = castle = 0;
    enpassant = no_sq;
    side_to_move = white;
}
