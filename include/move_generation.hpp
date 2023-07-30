#pragma once

#include <cstring>
#include "board.hpp"
#include "attacks.hpp"
#include "evaluation.hpp"
#include "utilities.hpp"

#define copy_board_s(board)                                                                          \
    Bitboard bitboard_cpy[12], occupancy_cpy[3], hash_position_cpy = board.hash_position;            \
    sint enpassant_cpy = board.enpassant, castle_cpy = board.castle, side_cpy = board.side_to_move;  \
    memcpy(bitboard_cpy, board.bitboard, sizeof(bitboard_cpy));                                      \
    memcpy(occupancy_cpy, board.occupancy, sizeof(occupancy_cpy));                                   \

#define revert_board_s(board)                                                                   \
    memcpy(board.bitboard, bitboard_cpy, sizeof(board.bitboard));                               \
    memcpy(board.occupancy, occupancy_cpy, sizeof(board.occupancy));                            \
    board.side_to_move = side_cpy; board.enpassant = enpassant_cpy; board.castle = castle_cpy;  \
    board.hash_position = hash_position_cpy                                                     \

#define copy_board_p(board)                                                                             \
    Bitboard bitboard_cpy[12], occupancy_cpy[3], hash_position_cpy = board->hash_position;              \
    sint enpassant_cpy = board->enpassant, castle_cpy = board->castle, side_cpy = board->side_to_move;  \
    memcpy(bitboard_cpy, board->bitboard, sizeof(bitboard_cpy));                                        \
    memcpy(occupancy_cpy, board->occupancy, sizeof(occupancy_cpy));                                     \

#define revert_board_p(board)                                                                      \
    memcpy(board->bitboard, bitboard_cpy, sizeof(board->bitboard));                                \
    memcpy(board->occupancy, occupancy_cpy, sizeof(board->occupancy));                             \
    board->side_to_move = side_cpy; board->enpassant = enpassant_cpy; board->castle = castle_cpy;  \
    board->hash_position = hash_position_cpy;                                                      \

#define copy_board_non_cap_white(board)                                                              \
    Bitboard bitboard_cpy[6], occupancy_cpy[3], hash_position_cpy = board.hash_position;             \
    sint enpassant_cpy = board.enpassant, castle_cpy = board.castle, side_cpy = board.side_to_move;  \
    memcpy(bitboard_cpy, board.bitboard, sizeof(bitboard_cpy));                                    \
    memcpy(occupancy_cpy, board.occupancy, sizeof(occupancy_cpy));                                   \

#define revert_board_non_cap_white(board)                                                       \
    memcpy(board.bitboard, bitboard_cpy, sizeof(bitboard_cpy);                             \
    memcpy(board.occupancy, occupancy_cpy, sizeof(board.occupancy));                            \
    board.side_to_move = side_cpy; board.enpassant = enpassant_cpy; board.castle = castle_cpy;  \
    board.hash_position = hash_position_cpy                                                     \

#define copy_board_non_cap_black(board)                                                              \
    Bitboard bitboard_cpy[6], occupancy_cpy[3], hash_position_cpy = board.hash_position;            \
    sint enpassant_cpy = board.enpassant, castle_cpy = board.castle, side_cpy = board.side_to_move;  \
    memcpy(bitboard_cpy, board.bitboard+6, sizeof(bitboard_cpy));                                      \
    memcpy(occupancy_cpy, board.occupancy, sizeof(occupancy_cpy));                                   \

#define revert_board_non_cap_black(board)                                                       \
    memcpy(board.bitboard, bitboard_cpy+6, sizeof(bitboard_cpy));                               \
    memcpy(board.occupancy, occupancy_cpy, sizeof(board.occupancy));                            \
    board.side_to_move = side_cpy; board.enpassant = enpassant_cpy; board.castle = castle_cpy;  \
    board.hash_position = hash_position_cpy                                                     \


// Move representation
// https://www.chessprogramming.org/Encoding_Moves

// 0000 0000 0000 0000 00xx xxxx  - source square (0x3f)
// 0000 0000 0000 xxxx xx00 0000  - target square (0xfc0)
// 0000 0000 xxxx 0000 0000 0000  - piece (0xf000)
// 0000 xxxx 0000 0000 0000 0000  - promoted piece (0xf0000)

// 000x 0000 0000 0000 0000 0000  - capture (0x100000)
// 00x0 0000 0000 0000 0000 0000  - double pawn push (0x200000)
// 0x00 0000 0000 0000 0000 0000  - enpassant (0x400000)
// x000 0000 0000 0000 0000 0000  - castling (0x800000)

// macro to generate the move using a 32 byte int
#define generate_move(source_sq, target_sq, piece, promoted_piece, capture, double_pawn, enpassant, castling)                                               \
    (source_sq) | (target_sq << 6) | (piece << 12) | (promoted_piece << 16) | (capture << 20) | (double_pawn << 21) | (enpassant << 22) | (castling << 23)  \

// common macros to get info about the type of move
#define get_piece(move) ((move & 0xf000) >> 12)  // gets the piece itself

#define get_source_square(move) (move & 0x3f)  // gets the source square of the piece

#define get_target_square(move) ((move & 0xfc0) >> 6)   // gets the target square of the piece

#define get_promoted_piece(move) ((move & 0xf0000) >> 16)  // in case of promotion, gets the promoted piece

#define get_capture(move) (move & 0x100000)  // in case of a capture, gets the captured piece

#define get_double_pawn_push(move) (move & 0x200000)  // is it a double pawn push?

#define get_enpassant_sq(move) (move & 0x400000)  // enpassant square

#define get_castling(move) (move & 0x800000)  // in case of castling, gets the side

// the maximum number of moves that can exist in a position
#define MAX_MOVES 218

typedef struct move_info
{
    Move_Type move;  // the move itself
    int move_eval;   // the evaluation of the move
}
move_info;

typedef struct _move_list
{
    move_info moves[MAX_MOVES];  
    short number_of_moves;
}
_move_list;
typedef _move_list* move_list;

// insert move & its evaluation into the list
static inline void insert_list(move_list move_list, Move_Type move, int evaluation)
{
    move_list->moves[move_list->number_of_moves].move = move;
    move_list->moves[move_list->number_of_moves].move_eval = evaluation;
    (move_list->number_of_moves)++;
}

static inline void swap(move_info** a, move_info** b)
{
    move_info** tmp = a;
    a = b;
    b = tmp;
}

static inline short partition(move_list arr, int low, int high)
{
    const int pivot = arr->moves[high].move_eval;
    short i = low-1;

    for (short j = low; j < high; j++)
    {
        if (arr->moves[j].move_eval > pivot)
        {
            i++;
            swap(arr->moves[i], arr->moves[j]);
        }
    }
    swap(arr->moves[i+1], arr->moves[high]);
    return i+1;
}

static void quick_sort(move_list arr, int low, int high)
{
    if (low < high)
    {
        short pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

static inline bool make_move(gameBoard& curr_board, int move,  move_types move_flag)
{
    // is this move a capture?
    int capture = get_capture(move);

    if (move_flag == capture_move && !capture)  // we are looking for a capture move and it is not one, skip it
        return false;

    // preserve board state
    copy_board_s(curr_board);
    
    // get information about the type of move (source square, target square, piece)
    int promoted_piece, source_square = get_source_square(move), target_square = get_target_square(move), piece = get_piece(move);

    zobr_hash(curr_board.hash_position, curr_board.hash_piece[piece][source_square]);
    zobr_hash(curr_board.hash_position, curr_board.hash_piece[piece][target_square]);

    // move piece from its source to its target square
    pop_bit(curr_board.bitboard[piece], source_square); set_bit(curr_board.bitboard[piece], target_square);

    // white's turn to move
    if (curr_board.side_to_move == white)
    {
        if (capture)  // capture move
        {
            sint search_piece = p, end_piece = k+1;

            if (get_enpassant_sq(move))
            {
                zobr_hash(curr_board.hash_position, curr_board.hash_piece[p][target_square+8]);
                pop_bit(curr_board.bitboard[p], target_square+8);
            }
            else
            {
                while (!get_bit(curr_board.bitboard[search_piece++], target_square));
                --search_piece;
                zobr_hash(curr_board.hash_position, curr_board.hash_piece[search_piece][target_square]);
                pop_bit(curr_board.bitboard[search_piece], target_square);
            }
            
            curr_board.occupancy[black] = EMPTY_BOARD;
            for (search_piece = p; search_piece < end_piece; search_piece++)
                curr_board.occupancy[black] |= curr_board.bitboard[search_piece];
        }
        else if (get_castling(move))  // castling move
        {
            int ss, ts;
            switch (target_square)
            {
                case (g1):
                    ss = h1; ts = f1;
                    break;

                case (c1):
                    ss = a1; ts = d1;
                    break;
            }
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[R][ss]);
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[R][ts]);
            pop_bit(curr_board.bitboard[R], ss);
            set_bit(curr_board.bitboard[R], ts);
        }

        if (curr_board.enpassant != no_sq)
            zobr_hash(curr_board.hash_position, curr_board.hash_enpassant[curr_board.enpassant]);
        curr_board.enpassant = no_sq;

        if (get_double_pawn_push(move))  // double pawn push
        {
            zobr_hash(curr_board.hash_position, curr_board.hash_enpassant[target_square+8]);
            curr_board.enpassant = target_square+8;
        }
        else if ((promoted_piece = get_promoted_piece(move)))  // pawn promotion
        {
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[P][target_square]);
            pop_bit(curr_board.bitboard[P], target_square);
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[promoted_piece][target_square]);
            set_bit(curr_board.bitboard[promoted_piece], target_square);
        }
        
        zobr_hash(curr_board.hash_position, curr_board.hash_castle[curr_board.castle]);
        curr_board.castle &= curr_board.castling_rights[source_square];
        curr_board.castle &= curr_board.castling_rights[target_square];

        curr_board.occupancy[both] = EMPTY_BOARD | curr_board.occupancy[black];
        curr_board.occupancy[white] = EMPTY_BOARD;
        for (sint curr_piece = P, final_piece = K+1; curr_piece < final_piece; curr_piece++)
            curr_board.occupancy[white] |= curr_board.bitboard[curr_piece];
        
        curr_board.occupancy[both] |= curr_board.occupancy[white];
        curr_board.side_to_move = black;

        zobr_hash(curr_board.hash_position, curr_board.hash_castle[curr_board.castle]);
        zobr_hash(curr_board.hash_position, curr_board.hash_side_to_move);

        // if by performing the move, the king is exposed to a check ignore the move
        if (curr_board.is_square_attacked(get_lsb(curr_board.bitboard[K]), curr_board.side_to_move))
        {
            revert_board_s(curr_board);  // take move back
            return false;
        }
    }
    // black's turn to move - basically mirrored
    else
    {
        if (capture)
        {
            sint search_piece = P, end_piece = K+1;

            if (get_enpassant_sq(move))
            {
                zobr_hash(curr_board.hash_position, curr_board.hash_piece[P][target_square-8]);
                pop_bit(curr_board.bitboard[P], target_square-8);
            }
            else
            {
                while (!get_bit(curr_board.bitboard[search_piece++], target_square));
                --search_piece;
                zobr_hash(curr_board.hash_position, curr_board.hash_piece[search_piece][target_square]);
                pop_bit(curr_board.bitboard[search_piece], target_square);
            }
            
            curr_board.occupancy[white] = EMPTY_BOARD;
            for (search_piece = P; search_piece < end_piece; search_piece++)
                curr_board.occupancy[white] |= curr_board.bitboard[search_piece];
        }
        else if (get_castling(move))
        {
            int ss, ts;
            switch (target_square)
            {
                case (g8):
                    ss = h8; ts = f8;
                    break;

                case (c8):
                    ss = a8; ts = d8;
                    break;
            }
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[r][ss]);
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[r][ts]);
            pop_bit(curr_board.bitboard[r], ss);
            set_bit(curr_board.bitboard[r], ts);
        }

        if (curr_board.enpassant != no_sq)
            zobr_hash(curr_board.hash_position, curr_board.hash_enpassant[curr_board.enpassant]);
        curr_board.enpassant = no_sq;

        if (get_double_pawn_push(move))  // double pawn push
        {
            zobr_hash(curr_board.hash_position, curr_board.hash_enpassant[target_square-8]);
            curr_board.enpassant = target_square-8;
        }
        else if ((promoted_piece = get_promoted_piece(move)))  // pawn promotion
        {
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[p][target_square]);
            pop_bit(curr_board.bitboard[p], target_square);
            zobr_hash(curr_board.hash_position, curr_board.hash_piece[promoted_piece][target_square]);
            set_bit(curr_board.bitboard[promoted_piece], target_square);
        }
        
        zobr_hash(curr_board.hash_position, curr_board.hash_castle[curr_board.castle]);
        curr_board.castle &= curr_board.castling_rights[source_square];
        curr_board.castle &= curr_board.castling_rights[target_square];

        curr_board.occupancy[both] = EMPTY_BOARD | curr_board.occupancy[white];
        curr_board.occupancy[black] = EMPTY_BOARD;
        for (sint curr_piece = p, final_piece = k+1; curr_piece < final_piece; curr_piece++)
            curr_board.occupancy[black] |= curr_board.bitboard[curr_piece];
        
        curr_board.occupancy[both] |= curr_board.occupancy[black];
        curr_board.side_to_move = white;

        zobr_hash(curr_board.hash_position, curr_board.hash_castle[curr_board.castle]);
        zobr_hash(curr_board.hash_position, curr_board.hash_side_to_move);

        // if by performing the move, the king is exposed to a check ignore the move
        if (curr_board.is_square_attacked(get_lsb(curr_board.bitboard[k]), curr_board.side_to_move))
        {
            revert_board_s(curr_board);  // take move back
            return false;
        }
    }
    return true;
}

static inline void generate_moves_white(gameBoard& curr_board, move_list move_list, Move_Type best_move, int ply)
{
    int source_square, target_square, move_id, special_score;
    const Bitboard not_white = ~curr_board.occupancy[white];
    Bitboard board, attacks;
    
    // pawn moves
    for (board = curr_board.bitboard[P]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);
        target_square = source_square-8;

        if (target_square > -1 && !get_bit(curr_board.occupancy[both], target_square))
        {
            if (source_square > h8 && source_square < a6)
            {
                move_id = generate_move(source_square, target_square, P, Q, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));

                move_id = generate_move(source_square, target_square, P, N, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));

                move_id = generate_move(source_square, target_square, P, R, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));
                
                move_id = generate_move(source_square, target_square, P, B, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));
            }
            else
            {
                move_id = generate_move(source_square, target_square, P, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));
                
                if ((source_square > h3 && source_square < a1) && !get_bit(curr_board.occupancy[both], target_square-=8))
                {
                    move_id = generate_move(source_square, target_square, P, 0, 0, 1, 0, 0);
                    special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                    insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, P, target_square, ply));
                }
            }
        }
        
        for (attacks = curr_board.get_pawn_attacks(source_square, white) & curr_board.occupancy[black]; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            // capture moves
            if (source_square > h8 && source_square < a6)
            {
                int eval = evaluate_attack_move(curr_board, P, target_square);

                move_id = generate_move(source_square, target_square, P, Q, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, P, N, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, P, R, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, P, B, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);
            }
            else
            {
                move_id = generate_move(source_square, target_square, P, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, P, target_square));
            }
        }

        if (curr_board.enpassant != no_sq)
        {
            if ((attacks = curr_board.get_pawn_attacks(source_square, white) & (1ULL << curr_board.enpassant)))
            {
                move_id = generate_move(source_square, get_lsb(attacks), P, 0, 1, 0, 1, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, 0);
            }
        }
    }
    
    // knight moves
    for (board = curr_board.bitboard[N]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);
        for (attacks = curr_board.get_knight_attacks(source_square) & not_white; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);
            
            if (get_bit(curr_board.occupancy[black], target_square))
            {
                move_id = generate_move(source_square, target_square, N, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, N, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, N, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, N, target_square, ply));
            }
        }
    }
    
    // bishop moves
    for (board = curr_board.bitboard[B]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);
        for (attacks = curr_board.get_bishop_attacks(source_square, curr_board.occupancy[both]) & not_white; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);
            
            if (get_bit(curr_board.occupancy[black], target_square))
            {
                move_id = generate_move(source_square, target_square, B, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, B, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, B, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, B, target_square, ply));
            }
        }
    }
    
    // rook moves
    for (board = curr_board.bitboard[R]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_rook_attacks(source_square, curr_board.occupancy[both]) & not_white; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[black], target_square))
            {
                move_id = generate_move(source_square, target_square, R, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, R, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, R, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, R, target_square, ply));
            }
        }
    }

    // queen moves
    for (board = curr_board.bitboard[Q]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_queen_attacks(source_square, curr_board.occupancy[both]) & not_white; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[black], target_square))
            {
                move_id = generate_move(source_square, target_square, Q, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, Q, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, Q, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, Q, target_square, ply));
            }
        }
    }

    // king moves
    if (curr_board.castle & wk)  // castling king side
    {
        if (!get_bit(curr_board.occupancy[both], f1) && !get_bit(curr_board.occupancy[both], g1) && !curr_board.is_square_attacked(e1, black) && !curr_board.is_square_attacked(f1, black))
        {
            move_id = generate_move(e1, g1, K, 0, 0, 0, 0, 1);
            special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
            insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, K, g1, ply)); 
        }
    }
    if (curr_board.castle & wq)  // castling queen side
    {
        if (!get_bit(curr_board.occupancy[both], d1) && !get_bit(curr_board.occupancy[both], c1) && !get_bit(curr_board.occupancy[both], b1) &&
        !curr_board.is_square_attacked(e1, black) && !curr_board.is_square_attacked(d1, black))
        {
            move_id = generate_move(e1, c1, K, 0, 0, 0, 0, 1);
            special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
            insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, K, c1, ply));
        }
    }
    for (board = curr_board.bitboard[K]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);
        
        for (attacks = curr_board.get_king_attacks(source_square) & not_white; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);
            if (get_bit(curr_board.occupancy[black], target_square))
            {
                move_id = generate_move(source_square, target_square, K, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, K, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, K, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, K, target_square, ply));
            }
        }
    }
}

static inline void generate_moves_black(gameBoard& curr_board, move_list move_list, Move_Type best_move, int ply)
{
    int source_square, target_square, move_id, special_score;
    const Bitboard not_black = ~curr_board.occupancy[black];
    Bitboard board, attacks;

    // pawn moves
    for (board = curr_board.bitboard[p]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);
        target_square = source_square+8;

        if (target_square < no_sq && !get_bit(curr_board.occupancy[both], target_square))
        {
            if (source_square > h3 && source_square < a1)
            {
                move_id = generate_move(source_square, target_square, p, q, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));

                move_id = generate_move(source_square, target_square, p, n, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));

                move_id = generate_move(source_square, target_square, p, r, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));

                move_id = generate_move(source_square, target_square, p, b, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));
            }
            else
            {
                move_id = generate_move(source_square, target_square, p, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));

                if ((source_square > h8 && source_square < a6) && !get_bit(curr_board.occupancy[both], target_square+=8))
                {
                    move_id = generate_move(source_square, target_square, p, 0, 0, 1, 0, 0);
                    special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                    insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));
                }  
            }
        }

        for (attacks = curr_board.get_pawn_attacks(source_square, black) & curr_board.occupancy[white]; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (source_square > h3 && source_square < a1)
            {
                int eval = evaluate_attack_move(curr_board, p, target_square);

                move_id = generate_move(source_square, target_square, p, q, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, p, n, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, p, r, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);

                move_id = generate_move(source_square, target_square, p, b, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:eval);
            }
            else
            {
                move_id = generate_move(source_square, target_square, p, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, p, target_square));
            }
        }

        if (curr_board.enpassant != no_sq)
        {
            if ((attacks = curr_board.get_pawn_attacks(source_square, black) & (1ULL << curr_board.enpassant)))
            {
                move_id = generate_move(source_square, get_lsb(attacks), p, 0, 1, 0, 1, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, p, target_square, ply));
            }
        }
    }

    // knight moves
    for (board = curr_board.bitboard[n]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_knight_attacks(source_square) & not_black; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[white], target_square))
            {
                move_id = generate_move(source_square, target_square, n, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, n, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, n, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, n, target_square, ply));
            } 
        }
    }
    
    // bishop moves
    for (board = curr_board.bitboard[b]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_bishop_attacks(source_square, curr_board.occupancy[both]) & not_black; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[white], target_square))
            {
                move_id = generate_move(source_square, target_square, b, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, b, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, b, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, b, target_square, ply));
            }
        }
    }
    
    // rook moves
    for (board = curr_board.bitboard[r]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_rook_attacks(source_square, curr_board.occupancy[both]) & not_black; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[white], target_square))
            {
                move_id = generate_move(source_square, target_square, r, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, r, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, r, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, r, target_square, ply));
            } 
        }
    }

    // queen moves
    for (board = curr_board.bitboard[q]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_queen_attacks(source_square, curr_board.occupancy[both]) & not_black; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[white], target_square))
            {
                move_id = generate_move(source_square, target_square, q, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, q, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, q, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, q, target_square, ply));
            }
        }
    }

    // king moves
    if (curr_board.castle & bk)  // king side castling
    {
        if (!get_bit(curr_board.occupancy[both], f8) && !get_bit(curr_board.occupancy[both], g8) && !curr_board.is_square_attacked(e8, white) && !curr_board.is_square_attacked(f8, white))
        {
            move_id = generate_move(e8, g8, k, 0, 0, 0, 0, 1);
            special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
            insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, k, g8, ply));
        }    
    }
    if (curr_board.castle & bq)  // queen side castling
    {
        if (!get_bit(curr_board.occupancy[both], d8) && !get_bit(curr_board.occupancy[both], c8) && !get_bit(curr_board.occupancy[both], b8) &&
        !curr_board.is_square_attacked(e8, white) && !curr_board.is_square_attacked(d8, white))
        {
            move_id = generate_move(e8, c8, k, 0, 0, 0, 0, 1);
            special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
            insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, k, c8, ply));
        }
    }
    for (board = curr_board.bitboard[k]; board; pop_bit(board, source_square))
    {
        source_square = get_lsb(board);

        for (attacks = curr_board.get_king_attacks(source_square) & not_black; attacks; pop_bit(attacks, target_square))
        {
            target_square = get_lsb(attacks);

            if (get_bit(curr_board.occupancy[white], target_square))
            {
                move_id = generate_move(source_square, target_square, k, 0, 1, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score:evaluate_attack_move(curr_board, k, target_square));
            }
            else
            {
                move_id = generate_move(source_square, target_square, k, 0, 0, 0, 0, 0);
                special_score = move_eval(curr_board, move_id, best_move, ply);  // pv/ hash move
                insert_list(move_list, move_id, (special_score) ? special_score: evaluate_quiet_move(curr_board, move_id, k, target_square, ply));
            }
        }
    }
}

inline void get_moves(gameBoard& curr_board, move_list generated_moves, Move_Type best_move, int ply)
{
    curr_board.follow_pv = false;
    generated_moves->number_of_moves = 0;
    if (curr_board.side_to_move == white) generate_moves_white(curr_board, generated_moves, best_move, ply);
    else generate_moves_black(curr_board, generated_moves, best_move, ply);
}

inline void generate_moves(gameBoard& curr_board, move_list generated_moves, Move_Type best_move, int ply)
{
    get_moves(curr_board, generated_moves, best_move, ply);
    
    // sort the moves using quick sort
    quick_sort(generated_moves, 0, generated_moves->number_of_moves-1);
}
