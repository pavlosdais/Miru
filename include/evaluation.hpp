#pragma once

#include "board.hpp"
#include "evaluation_tables.hpp"

// gets the rank that the square belongs in
static sint get_rank[] =
{
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0
};

static sint passed_pawn_bonus[] = {0, 3, 7, 12, 18, 29, 50, 200};

// evaluate a position from scratch
inline int evaluate_position(gameBoard& board)
{
    Bitboard piece_board;
    int eval = 0, source_square, double_pawns;
    sint num_of_bishops;

    // interpolate between opening, middlegame & endgame
    int game_phase_score = 0, opening_score = 0, endgame_score = 0;
    
    for (piece_board = board.bitboard[P]; piece_board; pop_bit(piece_board, source_square))  // white pawns evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][P] + positional_eval[OPENING][PAWN][source_square];
        endgame_score += material_eval[ENDGAME][P] + positional_eval[ENDGAME][PAWN][source_square];

        double_pawns = count_bits(board.bitboard[P] & board.file_masks[source_square]);
        
        if (double_pawns > 1)
            eval += double_pawns * DOUBLE_PAWNS_PENALTY;
        
        if (get_rank[source_square] != 0 && get_rank[source_square] != 7 && (board.bitboard[P] & board.isolated_masks[source_square]) == 0)
            eval += ISOLATED_PAWN_PENALTY;
        
        if ((board.passed_masks[white][source_square] & board.bitboard[p]) == 0)
            eval += passed_pawn_bonus[get_rank[source_square]];
    }

    for (piece_board = board.bitboard[p]; piece_board; pop_bit(piece_board, source_square))   // black pawns evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][p] - positional_eval[OPENING][PAWN][mirror_square[source_square]];
        endgame_score += material_eval[ENDGAME][p] - positional_eval[ENDGAME][PAWN][mirror_square[source_square]];

        double_pawns = count_bits(board.bitboard[p] & board.file_masks[source_square]);
        
        if (double_pawns > 1)
            eval -= double_pawns * DOUBLE_PAWNS_PENALTY;
        
        if (get_rank[source_square] != 0 && get_rank[source_square] != 7 && (board.bitboard[p] & board.isolated_masks[source_square]) == 0)
            eval -= ISOLATED_PAWN_PENALTY;
        
        if ((board.passed_masks[black][source_square] & board.bitboard[P]) == 0)
            eval -= passed_pawn_bonus[get_rank[mirror_square[source_square]]];
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
    
        opening_score += (material_eval[OPENING][n] - positional_eval[OPENING][KNIGHT][mirror_square[source_square]]);
        endgame_score += (material_eval[ENDGAME][n] - positional_eval[ENDGAME][KNIGHT][mirror_square[source_square]]);
        game_phase_score -= material_eval[OPENING][n];
    }

    for (piece_board = board.bitboard[B], num_of_bishops = 0; piece_board; pop_bit(piece_board, source_square), num_of_bishops++)   // white bishop evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][B] + positional_eval[OPENING][BISHOP][source_square];
        endgame_score += material_eval[ENDGAME][B] + positional_eval[ENDGAME][BISHOP][source_square];
        game_phase_score += material_eval[OPENING][B];

        eval += count_bits(board.get_bishop_attacks(source_square, board.occupancy[both]));
    }
    if (num_of_bishops == 2) eval += 11;  // double bishop bonus

    for (piece_board = board.bitboard[b], num_of_bishops = 0; piece_board; pop_bit(piece_board, source_square), num_of_bishops++)   // black bishop evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += (material_eval[OPENING][b] - positional_eval[OPENING][BISHOP][mirror_square[source_square]]);
        endgame_score += (material_eval[ENDGAME][b] - positional_eval[ENDGAME][BISHOP][mirror_square[source_square]]);
        game_phase_score -= material_eval[OPENING][b];

        eval -= count_bits(board.get_bishop_attacks(source_square, board.occupancy[both]));
    }
    if (num_of_bishops == 2) eval -= 11;  // double bishop bonus

    for (piece_board = board.bitboard[R]; piece_board; pop_bit(piece_board, source_square))   // white rook evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][R] + positional_eval[OPENING][ROOK][source_square];
        endgame_score += material_eval[ENDGAME][R] + positional_eval[ENDGAME][ROOK][source_square];
        game_phase_score += material_eval[OPENING][R];

        if ((board.bitboard[P] & board.file_masks[source_square]) == 0)
            eval += SEMI_OPEN_FILE;
        
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
            eval += OPEN_FILE;
    }

    for (piece_board = board.bitboard[r]; piece_board; pop_bit(piece_board, source_square))   // black rook evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += (material_eval[OPENING][r] - positional_eval[OPENING][ROOK][mirror_square[source_square]]);
        endgame_score += (material_eval[ENDGAME][r] - positional_eval[ENDGAME][ROOK][mirror_square[source_square]]);
        game_phase_score -= material_eval[OPENING][r];

        if ((board.bitboard[p] & board.file_masks[source_square]) == 0)
            eval -= SEMI_OPEN_FILE;
        
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
            eval -= OPEN_FILE;
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

        opening_score += (material_eval[OPENING][q] - positional_eval[OPENING][QUEEN][mirror_square[source_square]]);
        endgame_score += (material_eval[ENDGAME][q] - positional_eval[ENDGAME][QUEEN][mirror_square[source_square]]);
        game_phase_score -= material_eval[OPENING][q];
    }

    for (piece_board = board.bitboard[K]; piece_board; pop_bit(piece_board, source_square))   // white king evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += material_eval[OPENING][K] + positional_eval[OPENING][KING][source_square];
        endgame_score += material_eval[ENDGAME][K] + positional_eval[ENDGAME][KING][source_square];

        if ((board.bitboard[P] & board.file_masks[source_square]) == 0)
            eval -= SEMI_OPEN_FILE;
        
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
            eval -= OPEN_FILE;
        
        eval += count_bits(board.get_king_attacks(source_square) & board.occupancy[white]) * KING_SHIELD;
    }

    for (piece_board = board.bitboard[k]; piece_board; pop_bit(piece_board, source_square))   // black king evaluation
    {
        source_square = get_lsb(piece_board);

        opening_score += (material_eval[OPENING][k] - positional_eval[OPENING][KING][mirror_square[source_square]]);
        endgame_score += (material_eval[ENDGAME][k] - positional_eval[ENDGAME][KING][mirror_square[source_square]]);

        if ((board.bitboard[p] & board.file_masks[source_square]) == 0)
            eval += SEMI_OPEN_FILE;
        
        if (((board.bitboard[P] | board.bitboard[p]) & board.file_masks[source_square]) == 0)
            eval += OPEN_FILE;
        
        eval -= count_bits(board.get_king_attacks(source_square) & board.occupancy[black]) * KING_SHIELD;
    }

    if (game_phase_score > OPENING_SCORE)
        eval += opening_score;
    else if (game_phase_score < ENDGAME_SCORE)
        eval += endgame_score;
    else
        eval += (opening_score*game_phase_score + endgame_score*(opening_score - endgame_score)) / OPENING_SCORE;

    return (board.side_to_move == white)? eval: -eval;
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
