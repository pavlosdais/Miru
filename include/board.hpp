#pragma once

#include "common.hpp"
#include "transposition_table.hpp"
#include <vector>

// Bitboard representation
// https://pages.cs.wisc.edu/~psilord/blog/data/chess-pages/rep.html
// https://www.chessprogramming.org/Bitboards

class gameBoard
{
    private:
        // attack tables
        void create_attack_tables();
        void destroy_attack_tables();
        void create_leaping_pieces_attacks();
        void create_sliding_pieces_attacks(const sliding_pieces sliding_piece);

        // zobrist keys 
        void init_zobrist_keys();
        void destroy_zobrist_keys();

        // evaluation tables
        void create_evaluation_tables();
        void destroy_evaluation_tables();

        // helper pre-calculated attack tables
        Bitboard **pawn_attacks, *knight_attacks, *king_attacks, **bishop_attacks, **rook_attacks, *bishop_masks, *rook_masks;
    public:

        // -board representation-
        Bitboard bitboard[12];  // bitboard storing the indices of all the pieces
        Bitboard occupancy[3];  // helper bitboard containing the indices of white's, black's and both's players pieces

        sint side_to_move;  // side to move
        sint enpassant;     // enpassant square
        sint castle;        // castling rights

        const sint castling_rights[SQUARES] =
        {
            7,  15, 15, 15,  3, 15, 15, 11,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            13, 15, 15, 15, 12, 15, 15, 14
        };

        // store killer and history moves
        Move_Type** killer_moves, **history_moves;
        
        // store principal variation
        bool follow_pv, score_pv;
        Move_Type *pv_length, **pv_table;
        
        // use a transposition table alongside with zobrist hashing to store positions during search
        transposition_table TT;
        Bitboard hash_position, **hash_piece, *hash_enpassant, *hash_castle, hash_side_to_move;
        Bitboard get_hash_position();

        inline int HasNonPawn();

        // store masks for evaluation
        Bitboard* file_masks, *rank_masks, *isolated_masks, **passed_masks;

        // history table
        Bitboard* History;
        short rep_index;
        
        // returns true if the we have repeated the same position twice, false if not
        inline bool is_repetition();

        // uci needed functionality
        int quit, movestogo, movetime, time, inc, starttime, stoptime, timeset, stopped;
        inline int input_waiting();
        inline void read_input();
        inline void communicate();

        // access attack tables
        inline Bitboard get_bishop_attacks(const sint square, Bitboard board);
        inline Bitboard get_rook_attacks(const sint square, Bitboard board);
        inline Bitboard get_king_attacks(const sint square);
        inline Bitboard get_knight_attacks(const sint square);
        inline Bitboard get_pawn_attacks(const sint square, const sint color);
        inline Bitboard get_pawn_pushes(const sint square, const sint color);
        inline Bitboard get_queen_attacks(const sint square, Bitboard board);

        // update history heuristic
        inline void update_history_move(Move_Type move, int depth);

        // update principal variation
        inline void update_PV(Move_Type move, int ply);

        // update killer heuristic
        inline void update_killer_move(Move_Type move, int ply);

        // returns true if the king is in check for the player to move
        inline bool is_king_in_check();

        // resets the board
        void resetBoard();

        // returns true if the square is attacked by the colored player, false if not
        inline bool is_square_attacked(const sint square, const sint color);
        
        // parses through a fen position
        void get_fen_position(string fen_position);

        // prints the board
        void printBoard();

        // constructor
        gameBoard();

        // destructor
        ~gameBoard();
};
