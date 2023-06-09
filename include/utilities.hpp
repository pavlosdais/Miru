#pragma once

#include "move_generation.hpp"

// returns the index of the char
sint get_index(const char letter);

// returns the promoted piece 
char find_promoted_piece(sint piece);

// prints the move
void print_move(Move_Type move);

// returns the current time (in milliseconds)
int get_time();

// returns a "random" U64 number
Bitboard get_U64();

inline bool gameBoard:: is_repetition()
{
    for (Move_Type position = this->rep_index-1; position > -1; position--)
        if (History[position] == hash_position)
            return true;
    
    return false;
}

#define make_null_move(board, ply)                                                \
    copy_board_p(board);                                                          \
    ply++;                                                                        \
    board->History[++(board->rep_index)] = board->hash_position;                  \
    zobr_hash(board->hash_position, board->hash_side_to_move);                    \
    board->side_to_move ^= 1;                                                     \
    if (board->enpassant != no_sq)                                                \
        zobr_hash(board->hash_position, board->hash_enpassant[board->enpassant]); \
    board->enpassant = no_sq;                                                     \

#define revert_null_move(board, ply)  \
    ply--;                            \
    board->rep_index--;               \
    revert_board_p(board);            \
    if (board->stopped) return 0;     \


inline int gameBoard:: HasNonPawn()
{
    if (hash_side_to_move == white)
        return count_bits(occupancy[white]) ^ bitboard[K] ^ bitboard[P];
    else
        return count_bits(occupancy[black]) ^ bitboard[k] ^ bitboard[p];
}

inline void gameBoard:: add_to_history()  { this->History[++(this->rep_index)] = this->hash_position; }

inline bool gameBoard:: is_square_attacked(const sint square, const sint color)
{
    if (color == white)
    {
        // check knight attacks
        if (get_knight_attacks(square) & bitboard[N]) return true;

        // check queen attacks
        if (get_queen_attacks(square, occupancy[both]) & bitboard[Q]) return true;

        // check bishop attacks
        if (get_bishop_attacks(square, occupancy[both]) & bitboard[B]) return true;

        // check rook attacks
        if (get_rook_attacks(square, occupancy[both]) & bitboard[R]) return true;

        // check pawn attacks
        if (get_pawn_attacks(square, black) & bitboard[P]) return true;

        // check king attacks
        if (get_king_attacks(square) & bitboard[K]) return true;
    }
    else
    {
        // check knight attacks
        if (get_knight_attacks(square) & bitboard[n]) return true;

        // check queen attacks
        if (get_queen_attacks(square, occupancy[both]) & bitboard[q]) return true;

        // check bishop attacks
        if (get_bishop_attacks(square, occupancy[both]) & bitboard[b]) return true;

        // check rook attacks
        if (get_rook_attacks(square, occupancy[both]) & bitboard[r]) return true;
        
        // check pawn attacks
        if (get_pawn_attacks(square, white) & bitboard[p]) return true;

        // check king attacks
        if (get_king_attacks(square) & bitboard[k]) return true;
    }
    
    // square is not being attacked by any piece
    return false;
}

#define get_capture(move) (move & 0x100000)
#define get_piece(move) ((move & 0xf000) >> 12)
#define get_target_square(move) ((move & 0xfc0) >> 6)

inline void gameBoard:: update_history_move(Move_Type move, int depth)
{
    if (!get_capture(move))
        this->history_moves[get_piece(move)][get_target_square(move)] += depth;
}

inline void gameBoard:: update_PV(Move_Type move, int ply)
{
    this->pv_table[ply][ply] = move;
    int nply = ply+1;
    for (int next_ply = nply; next_ply < this->pv_length[nply]; next_ply++)
        this->pv_table[ply][next_ply] = this->pv_table[nply][next_ply];
    
    this->pv_length[ply] = this->pv_length[nply];
}

inline void gameBoard:: update_killer_move(Move_Type move, int ply)
{
    if (!get_capture(move))
    {
        this->killer_moves[1][ply] = this->killer_moves[0][ply];
        this->killer_moves[0][ply] = move;
    }
}

inline bool gameBoard:: is_king_in_check()
{
    if (side_to_move == white)
    {
        if (is_square_attacked(get_lsb(bitboard[K]), black))
            return true;
    }
    else if (is_square_attacked(get_lsb(bitboard[k]), white))
        return true;

    return false;
}
