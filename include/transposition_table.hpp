#pragma once

#include "common.hpp"

#define hash_size 0x400000

typedef enum
{
    HASH_EXACT=0, HASH_ALPHA=1, HASH_BETA=2, HASH_NONE=3
}
hash_flag;

typedef struct
{
    Bitboard hash_position;  // unique position identifier
    Move_Type move;          // store the move
    int evaluation;          // evaluation (alpha/beta/exact)
    short depth;             // depth of the node
    hash_flag flag;          // flag the type of node (UPPERBOUND, LOWERBOUND, EXACT)
}
_tt_node;

typedef _tt_node* tt_node;

typedef struct
{
    tt_node nodes;
    unsigned int size;
    unsigned int cur_elements;
}
_transposition_table;
typedef _transposition_table* transposition_table;

// create transposition table
void create_transposition_table(transposition_table* TT, unsigned int size);

// destroy transposition table
void destroy_transposition_table(transposition_table TT);

// clear hash_table
inline void tt_clear(transposition_table TT)
{
    for (unsigned int i = 0; i < TT->size; i++)
    {
        TT->nodes[i].hash_position = EMPTY_BOARD;
        TT->nodes[i].move = 0;
        TT->nodes[i].depth = 0;
        TT->nodes[i].flag = HASH_NONE;
        TT->nodes[i].evaluation = 0;
    }
    TT->cur_elements = 0;
}

static inline int tt_search(transposition_table TT, Bitboard hash_position, Move_Type& best_move, int ply, int alpha, int beta, short depth)
{
    unsigned int index = hash_position % TT->size;

    if (TT->nodes[index].hash_position == hash_position && TT->nodes[index].depth >= depth)
    {
        int eval = TT->nodes[index].evaluation;

        if (eval < -MATE_SCORE) eval += ply;
        if (eval > MATE_SCORE) eval -= ply;

        if (TT->nodes[index].flag == HASH_EXACT)
            return eval;
        
        if (TT->nodes[index].flag == HASH_ALPHA && eval <= alpha)
            return alpha;
        
        if (TT->nodes[index].flag == HASH_BETA && eval >= beta)
            return beta;
    }
    best_move = TT->nodes[index].move;
    
    return HASH_NONE;
}

inline void tt_insert(transposition_table TT, Bitboard hash_position, Move_Type move, int ply, int evaluation, short depth, hash_flag flag)
{
    // current scheme is always replace
    unsigned int index = hash_position % TT->size;

    if (evaluation < -MATE_SCORE) evaluation -= ply;
    if (evaluation > MATE_SCORE) evaluation += ply;

    TT->nodes[index].hash_position = hash_position;
    TT->nodes[index].depth = depth;
    TT->nodes[index].flag = flag;
    TT->nodes[index].evaluation = evaluation;
    TT->nodes[index].move = move;
}
