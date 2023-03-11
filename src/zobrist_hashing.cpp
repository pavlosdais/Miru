#include "../include/board.hpp"
#include "../include/utilities.hpp"
#include "../include/move_generation.hpp"
#include "../include/uci_commands.hpp"
#include "../include/tests.hpp"
#include "../include/search.hpp"
#include "../include/evaluation.hpp"

Bitboard gameBoard:: get_hash_position()
{
    Bitboard hash_key = EMPTY_BOARD;

    // hash pieces
    for (sint piece = P, fin_piece = k+1; piece < fin_piece; piece++)
    {
        Bitboard board = bitboard[piece];

        while (board)
        {
            int source_square = get_lsb(board);

            zobr_hash(hash_key, hash_piece[piece][source_square]);

            pop_bit(board, source_square);
        }
    }

    // hash enpassant
    if (enpassant != no_sq)
        zobr_hash(hash_key, hash_enpassant[enpassant]);
    
    // hash castling rights
    zobr_hash(hash_key, hash_castle[castle]);

    // hash side to move
    if (side_to_move == black)
        zobr_hash(hash_key, hash_side_to_move);

    return hash_key;
}

void gameBoard:: init_zobrist_keys()
{
    // initialize side to move hash value
    hash_side_to_move = get_U64();

    // initialize piece hash values
    try
    {
        hash_piece = new Bitboard*[NUM_OF_PIECES];
        if (hash_piece == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    for (sint i = 0; i < NUM_OF_PIECES; i++)
    {
        try
        {
            hash_piece[i] = new Bitboard[SQUARES];
            if (hash_piece[i] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }

        // initialize piece hash values
        for (sint j = 0; j < SQUARES; j++)
            hash_piece[i][j] = get_U64();
    }

    try
    {
        hash_enpassant = new Bitboard[SQUARES];
        if (hash_enpassant == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    // initialize enpassant hash values
    for (sint i = 0; i < NUM_OF_PIECES; i++)
        hash_enpassant[i] = get_U64();

    try
    {
        hash_castle = new Bitboard[16];
        if (hash_castle == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }
    
    // initialize castle hash values
    for (sint i = 0; i < 12; i++)
        hash_castle[i] = get_U64();
}

void gameBoard:: destroy_zobrist_keys()
{
    for (int i = 0; i < NUM_OF_PIECES; i++)
        delete[] hash_piece[i];

    delete[] hash_castle;
    delete[] hash_enpassant;
    delete[] hash_piece;
}
