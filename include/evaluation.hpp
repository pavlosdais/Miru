#pragma once

#include "board.hpp"
#include "evaluation_tables.hpp"

// evaluate a position from scratch
inline int evaluate_position(gameBoard& board)
{
    Bitboard piece_board;
    int source_square, double_pawns_num;
    sint num_of_bishops;

    // interpolate between opening, middlegame & endgame
    int opening_bonus = 0, endgame_bonus = 0;
    int game_phase_score = 0, opening_score = 0, endgame_score = 0;

    for (piece_board = board.bitboard[P]; piece_board; pop_bit(piece_board, source_square))  // white pawns evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][P] + positional_eval[OPENING][PAWN][source_square];
        endgame_score += material_eval[ENDGAME][P] + positional_eval[ENDGAME][PAWN][source_square];

        double_pawns_num = count_bits(board.bitboard[P] & board.file_masks[source_square])-1;
        
        // double pawns penalty
        if (double_pawns_num > 0)
        {
            opening_bonus += double_pawns_num * double_pawn_penalty[OPENING];
            endgame_bonus += double_pawns_num * double_pawn_penalty[ENDGAME];
        }
        // isolated pawn penalty
        if (!(board.bitboard[P] & board.isolated_masks[source_square]))
        {
            opening_bonus += isolated_pawn_penalty[OPENING];
            endgame_bonus += isolated_pawn_penalty[ENDGAME];
        }
        // passed pawn bonus
        if (!(board.passed_masks[white][source_square] & board.bitboard[p]))
        {
            opening_bonus += passed_pawn_bonus[OPENING][get_rank[source_square]];
            endgame_bonus += passed_pawn_bonus[ENDGAME][get_rank[source_square]];
        } 
    }

    for (piece_board = board.bitboard[p]; piece_board; pop_bit(piece_board, source_square))   // black pawns evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][p] - positional_eval[OPENING][PAWN][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][p] - positional_eval[ENDGAME][PAWN][mirror_square[source_square]];

        double_pawns_num = count_bits(board.bitboard[p] & board.file_masks[source_square])-1;
        
        // double pawns penalty
        if (double_pawns_num > 0)
        {
            opening_bonus -= double_pawns_num * double_pawn_penalty[OPENING];
            endgame_bonus -= double_pawns_num * double_pawn_penalty[ENDGAME];
        }
        // isolated pawn penalty
        if (!(board.bitboard[p] & board.isolated_masks[source_square]))
        {
            opening_bonus -= isolated_pawn_penalty[OPENING];
            endgame_bonus -= isolated_pawn_penalty[ENDGAME];
        }
        // passed pawn bonus
        if (!(board.passed_masks[black][source_square] & board.bitboard[P]))
        {
            opening_bonus -= passed_pawn_bonus[OPENING][get_rank[source_square]];
            endgame_bonus -= passed_pawn_bonus[ENDGAME][get_rank[source_square]];
        }
    }

    for (piece_board = board.bitboard[N]; piece_board; pop_bit(piece_board, source_square))   // white knight evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][N] + positional_eval[OPENING][KNIGHT][source_square];
        endgame_score += material_eval[ENDGAME][N] + positional_eval[ENDGAME][KNIGHT][source_square];
        game_phase_score += material_eval[OPENING][N];
    }

    for (piece_board = board.bitboard[n]; piece_board; pop_bit(piece_board, source_square))   // black knight evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][n] - positional_eval[OPENING][KNIGHT][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][n] - positional_eval[ENDGAME][KNIGHT][mirror_square[source_square]];
        game_phase_score -= material_eval[OPENING][n];
    }

    for (piece_board = board.bitboard[B], num_of_bishops = 0; piece_board; pop_bit(piece_board, source_square), num_of_bishops++)   // white bishop evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][B] + positional_eval[OPENING][BISHOP][source_square];
        endgame_score += material_eval[ENDGAME][B] + positional_eval[ENDGAME][BISHOP][source_square];
        game_phase_score += material_eval[OPENING][B];
        
        // mobility bonus
        int attacks = count_bits(board.get_bishop_attacks(source_square, board.occupancy[both]));
        opening_bonus += attacks;
        endgame_bonus += attacks;
    }
    if (num_of_bishops == 2)  // double bishop bonus
    {
        opening_bonus += bishop_pair[OPENING];
        endgame_bonus += bishop_pair[ENDGAME];
    }

    for (piece_board = board.bitboard[b], num_of_bishops = 0; piece_board; pop_bit(piece_board, source_square), num_of_bishops++)   // black bishop evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][b] - positional_eval[OPENING][BISHOP][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][b] - positional_eval[ENDGAME][BISHOP][mirror_square[source_square]];
        game_phase_score -= material_eval[OPENING][b];

        // mobility bonus
        int attacks = count_bits(board.get_bishop_attacks(source_square, board.occupancy[both]));
        opening_bonus -= attacks;
        endgame_bonus -= attacks;
    }
    if (num_of_bishops == 2)  // double bishop bonus
    {
        opening_bonus -= bishop_pair[OPENING];
        endgame_bonus -= bishop_pair[ENDGAME];
    }

    for (piece_board = board.bitboard[R]; piece_board; pop_bit(piece_board, source_square))   // white rook evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][R] + positional_eval[OPENING][ROOK][source_square];
        endgame_score += material_eval[ENDGAME][R] + positional_eval[ENDGAME][ROOK][source_square];
        game_phase_score += material_eval[OPENING][R];

        // open & semi-open file bonus
        if ((board.bitboard[P] & board.file_masks[source_square]) == 0)
        {
            opening_bonus += semi_open_file[OPENING];
            endgame_bonus += semi_open_file[ENDGAME];
        }
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
        {
            opening_bonus += open_file[OPENING];
            endgame_bonus += open_file[ENDGAME];
        }
    }

    for (piece_board = board.bitboard[r]; piece_board; pop_bit(piece_board, source_square))   // black rook evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][r] - positional_eval[OPENING][ROOK][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][r] - positional_eval[ENDGAME][ROOK][mirror_square[source_square]];
        game_phase_score -= material_eval[OPENING][r];

        // open & semi-open file bonus
        if ((board.bitboard[p] & board.file_masks[source_square]) == 0)
        {
            opening_bonus -= semi_open_file[OPENING];
            endgame_bonus -= semi_open_file[ENDGAME];
        }
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
        {
            opening_bonus -= open_file[OPENING];
            endgame_bonus -= open_file[ENDGAME];
        }       
    }

    for (piece_board = board.bitboard[Q]; piece_board; pop_bit(piece_board, source_square))   // white queen evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][Q] + positional_eval[OPENING][QUEEN][source_square];
        endgame_score += material_eval[ENDGAME][Q] + positional_eval[ENDGAME][QUEEN][source_square];
        game_phase_score += material_eval[OPENING][Q];
    }

    for (piece_board = board.bitboard[q]; piece_board; pop_bit(piece_board, source_square))   // black queen evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][q] - positional_eval[OPENING][QUEEN][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][q] - positional_eval[ENDGAME][QUEEN][mirror_square[source_square]];
        game_phase_score -= material_eval[OPENING][q];
    }

    for (piece_board = board.bitboard[K]; piece_board; pop_bit(piece_board, source_square))   // white king evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][K] + positional_eval[OPENING][KING][source_square];
        endgame_score += material_eval[ENDGAME][K] + positional_eval[ENDGAME][KING][source_square];

        // very basic king safety
        if ((board.bitboard[P] & board.file_masks[source_square]) == 0)
        {
            opening_bonus -= semi_open_file[OPENING];
            endgame_bonus -= semi_open_file[ENDGAME];
        }
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
        {
            opening_bonus -= open_file[OPENING];
            endgame_bonus -= open_file[ENDGAME];
        }
        int attacks = count_bits(board.get_king_attacks(source_square) & board.occupancy[white]);
        opening_bonus += attacks * king_shield[OPENING];
        endgame_bonus += attacks * king_shield[ENDGAME];
    }

    for (piece_board = board.bitboard[k]; piece_board; pop_bit(piece_board, source_square))   // black king evaluation
    {
        source_square = get_lsb(piece_board);
        opening_score += material_eval[OPENING][k] - positional_eval[OPENING][KING][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][k] - positional_eval[ENDGAME][KING][mirror_square[source_square]];

        // very basic king safety
        if ((board.bitboard[p] & board.file_masks[source_square]) == 0)
        {
            opening_bonus += semi_open_file[OPENING];
            endgame_bonus += semi_open_file[ENDGAME];
        }
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
        {
            opening_bonus += open_file[OPENING];
            endgame_bonus += open_file[ENDGAME];
        }
        int attacks = count_bits(board.get_king_attacks(source_square) & board.occupancy[black]);
        opening_bonus -= attacks * king_shield[OPENING];
        endgame_bonus -= attacks * king_shield[ENDGAME];
    }

    int eval;
    // evaluate opening
    if (game_phase_score > OPENING_SCORE)
        eval = opening_score + opening_bonus;
    // evaluate endgame  
    else if (game_phase_score < ENDGAME_SCORE)
        eval = endgame_score + endgame_bonus;
    // evaluate middlegame
    else
        eval = (opening_score * game_phase_score + endgame_score * (OPENING_SCORE - game_phase_score)) / OPENING_SCORE
        + (opening_bonus + endgame_bonus) / 2;

    return (board.side_to_move == white) ? eval: -eval;
}

inline int evaluate_attack_move(gameBoard& board, sint piece, int target_square)
{
    sint search_piece = (board.side_to_move == white)? p: P;
    while (!get_bit(board.bitboard[search_piece++], target_square));
    return mvv_lva[piece][search_piece-1];
}

inline int move_eval(gameBoard& board, int move, int best_move, int ply)
{
    // hash move
    if (move == best_move)
        return HASH_MOVE_SCORE;

    // principal variation move
    if (!board.follow_pv && board.pv_table[0][ply] == move)
    {
        board.follow_pv = true;

        board.score_pv = false;
        return PV_MOVE_SCORE;
    }
    return 0;
}

inline int evaluate_quiet_move(gameBoard& board, sint move, sint piece, sint square, int ply)
{
    // return killer move evaluation
    if (board.killer_moves[0][ply] == move) return KILLER_MOVE_SCORES[0];
    if (board.killer_moves[1][ply] == move) return KILLER_MOVE_SCORES[1];

    // no killer moves found, return history move evaluation
    return board.history_moves[piece][square];
}
