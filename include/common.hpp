#pragma once

#include <iostream>
using namespace std;

#define ENGINE_NAME "Miru"
#define ENGINE_VERSION "v1.0"

#define RANKS 8
#define FILES 8
#define NUM_OF_KILLER_MOVES 2

// maximum ply number the search can store
#define MAX_PLY 200

// maximum number of depth the search can store
#define REPETITION_TABLE_SIZE 250

#define NUM_OF_PIECES 12
#define SQUARES 64
#define get_square(rank, file) (rank*RANKS + file)

// needed for search
#define INFINITY 50000
#define NEG_INFINITY -50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000

#define MEMORY_ALLOCATION_MESSAGE "Memory allocation failure"

#define ABS(a, b) ((a > b)? a-b: b-a)

typedef unsigned long long Bitboard;  // 64 bytes - used to represent the board
typedef int Move_Type;                // 4 bytes - used to represent the move
typedef unsigned char sint;           // 1 byte

#define HASH_MOVE_SCORE 40000
#define PV_MOVE_SCORE 20000

// enumerate the squares
typedef enum
{
    //          files
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,  // ranks
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
}
Squares;

static string square_to_coordinates[SQUARES] =
{
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// enumerate the pieces
typedef enum
{
    P=0, N=1, B=2, R=3, Q=4,  K=5,  // white's pieces
    p=6, n=7, b=8, r=9, q=10, k=11  // black's pieces
}
Pieces;

// enumerate the colors of the players
typedef enum
{
    white=0, black=1, both=2
}
Colors;

// enumerate the castling rights
typedef enum
{
    wk=1, wq=2, bk=4, bq=8
}
castling_rights;

typedef enum
{
    all_moves=0, capture_move=1
}
move_types;

// enumerate sliding pieces
typedef enum
{
    bishop=0, rook=1, queen=2
}
sliding_pieces;

// enumerate game phases (opening/middlegame/endgame)
typedef enum
{
    OPENING=0, MIDDLEGAME=2, ENDGAME=1
}
game_phase;

// enumerate piece types
typedef enum
{
    PAWN=0, KNIGHT=1, BISHOP=2, ROOK=3, QUEEN=4, KING=5
}
piece_types;

// bit manipulation operations
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define count_bits(bitboard) (__builtin_popcountll(bitboard))
#define get_lsb(bitboard) (__builtin_ctzll(bitboard))
#define get_msb(bitboard) (__builtin_clzll(bitboard))
#define zobr_hash(hash1, hash2) (hash1 ^= hash2)

// useful FEN positions
#define fen_starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
// debug & testing positions
// https://www.chessprogramming.org/Perft
#define fen_pos1 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define fen_pos2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"
#define fen_pos3 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -"
#define fen_pos4 "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
#define fen_pos5 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
#define fen_pos6 "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"

#define is_letter(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z'))
#define is_digit(a) (a >= '0' && a <= '9')

// some needed occupancy boards
#define EMPTY_BOARD 0ULL
#define NOT_A_FILE 18374403900871474942ULL
#define NOT_H_FILE 9187201950435737471ULL
#define NOT_HG_FILE 4557430888798830399ULL
#define NOT_AB_FILE 18229723555195321596ULL
#define RANK2 71776119061217280ULL
#define RANK7 65280ULL
