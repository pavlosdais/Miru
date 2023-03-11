#include "../include/attacks.hpp"
#include "../include/board.hpp"

void gameBoard:: create_attack_tables()
{
    // allocate memory for the attack tables
    try
    {
        pawn_attacks = new Bitboard*[2];
        if (pawn_attacks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    for (sint player = 0; player < 2; player++)
    {
        try
        {
            pawn_attacks[player] = new Bitboard[SQUARES];
            if (pawn_attacks[player] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }
    }

    try
    {
        knight_attacks = new Bitboard[SQUARES];
        if (knight_attacks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        bishop_masks = new Bitboard[SQUARES];
        if (bishop_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        rook_masks = new Bitboard[SQUARES];
        if (rook_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
            
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        king_attacks = new Bitboard[SQUARES];
        if (king_attacks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        bishop_attacks = new Bitboard*[SQUARES];
        if (bishop_attacks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        rook_attacks = new Bitboard*[SQUARES];
        if (rook_attacks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    for (sint square = 0; square < SQUARES; square++)
    {
        try
        {
            bishop_attacks[square] = new Bitboard[512];
            if (bishop_attacks[square] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }

        try
        {
            rook_attacks[square] = new Bitboard[4096];
            if (rook_attacks[square] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }
    }

    // create the attack maps for the leaping pieces (pawns/ knights/ king)
    create_leaping_pieces_attacks();

    // create the attack maps for sliding pieces (bishops/ rooks)
    create_sliding_pieces_attacks(bishop);
    create_sliding_pieces_attacks(rook);
}

void gameBoard:: destroy_attack_tables()
{   
    delete[] knight_attacks;
    delete[] bishop_masks;
    delete[] rook_masks;
    delete[] king_attacks;

    for (sint i = 0; i < 2; i++)
        delete[] pawn_attacks[i];
    delete[] pawn_attacks;

    for (sint square = 0; square < SQUARES; square++)
    {
        delete[] bishop_attacks[square];
        delete[] rook_attacks[square];
    }
    delete[] bishop_attacks;
    delete[] rook_attacks;
}

static inline Bitboard create_knight_attack(const sint square)
{
    // create a board and place the knight at the square
    Bitboard board = EMPTY_BOARD;
    set_bit(board, square);

    // create attack table for the knight
    Bitboard attacks = EMPTY_BOARD;

    if ((board >> 17) & NOT_H_FILE) attacks |= (board >> 17);
    if ((board >> 15) & NOT_A_FILE) attacks |= (board >> 15);
    if ((board >> 10) & NOT_HG_FILE) attacks |= (board >> 10);
    if ((board >> 6) & NOT_AB_FILE) attacks |= (board >> 6);
    if ((board << 17) & NOT_A_FILE) attacks |= (board << 17);
    if ((board << 15) & NOT_H_FILE) attacks |= (board << 15);
    if ((board << 10) & NOT_AB_FILE) attacks |= (board << 10);
    if ((board << 6) & NOT_HG_FILE) attacks |= (board << 6);

    return attacks;
}

static inline Bitboard create_king_attack(const sint square)
{
    // create a board and place the king at the square
    Bitboard board = EMPTY_BOARD;
    set_bit(board, square);

    // create attack table for the king
    Bitboard attacks = EMPTY_BOARD;

    if (board >> 8) attacks |= (board >> 8);
    if ((board >> 9) & NOT_H_FILE) attacks |= (board >> 9);
    if ((board >> 7) & NOT_A_FILE) attacks |= (board >> 7);
    if ((board >> 1) & NOT_H_FILE) attacks |= (board >> 1);
    if (board << 8) attacks |= (board << 8);
    if ((board << 9) & NOT_A_FILE) attacks |= (board << 9);
    if ((board << 7) & NOT_H_FILE) attacks |= (board << 7);
    if ((board << 1) & NOT_A_FILE) attacks |= (board << 1);

    return attacks;
}

static inline Bitboard create_pawn_attack(const sint side, const sint square)
{
    // create a board and place the pawn at the square
    Bitboard board = EMPTY_BOARD;
    set_bit(board, square);

    // create attack table for the pawn
    Bitboard attacks = EMPTY_BOARD;

    if (side == white)  // white
    {
        if ((board >> 7) & NOT_A_FILE) attacks |= (board >> 7);
        if ((board >> 9) & NOT_H_FILE) attacks |= (board >> 9);
    }
    else  // black
    {
        if ((board << 7) & NOT_H_FILE) attacks |= (board << 7);
        if ((board << 9) & NOT_A_FILE) attacks |= (board << 9);
    }

    return attacks;
}

static inline Bitboard create_pawn_push(const sint side, const sint square)
{
    // create a board and place the pawn at the square
    Bitboard board = EMPTY_BOARD;
    set_bit(board, square);

    // create attack table for the pawn
    Bitboard attacks = EMPTY_BOARD;

    if (side == white)  // white
        attacks |= (board >> 8);
    else  // black
        attacks |= (board << 8);

    return attacks;
}

static inline Bitboard create_rook_attacks(const sint square)
{
    // create attack table for the rook
    Bitboard attacks = EMPTY_BOARD;

    const sint target_rank = square / 8, target_file = square % 8;

    for (char rank = target_rank+1; rank < 7; rank++) attacks |= (1ULL << (rank * 8 + target_file));
    for (char rank = target_rank-1; rank > 0; rank--) attacks |= (1ULL << (rank * 8 + target_file));
    for (char file = target_file+1; file < 7; file++) attacks |= (1ULL << (target_rank * 8 + file));
    for (char file = target_file-1; file > 0; file--) attacks |= (1ULL << (target_rank * 8 + file));

    return attacks;
}

static inline Bitboard create_bishop_attacks(const sint square)
{
    // create attack table for the bishop
    Bitboard attacks = EMPTY_BOARD;

    const sint target_rank = square / 8, target_file = square % 8;

    for (char rank = target_rank+1, file = target_file+1; rank < 7 && file < 7; rank++, file++) attacks |= (1ULL << (rank * 8 + file));
    for (char rank = target_rank-1, file = target_file+1; rank > 0 && file < 7; rank--, file++) attacks |= (1ULL << (rank * 8 + file));
    for (char rank = target_rank+1, file = target_file-1; rank < 7 && file > 0; rank++, file--) attacks |= (1ULL << (rank * 8 + file));
    for (char rank = target_rank-1, file = target_file-1; rank > 0 && file > 0; rank--, file--) attacks |= (1ULL << (rank * 8 + file));

    return attacks;
}

static inline Bitboard create_bishop_attacks_ots(const sint square, const Bitboard board)
{
    // create attack map for the bishop
    Bitboard attacks = EMPTY_BOARD;

    const sint target_rank = square / 8, target_file = square % 8;

    for (char rank = target_rank+1, file = target_file+1; rank < 8 && file < 8; rank++, file++)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & board) break;  // a piece is blocking its path
    }
    for (char rank = target_rank-1, file = target_file+1; rank > -1 && file < 8; rank--, file++) 
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & board) break;  // a piece is blocking its path 
    }
    for (char rank = target_rank+1, file = target_file-1; rank < 8 && file > -1; rank++, file--)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & board) break;  // a piece is blocking its path
    }
    for (char rank = target_rank-1, file = target_file-1; rank > -1 && file > -1; rank--, file--)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & board) break;  // a piece is blocking its path
    }

    return attacks;
}

static inline Bitboard create_rook_attacks_ots(const sint square, const Bitboard board)
{
    // create attack map for the bishop
    Bitboard attacks = EMPTY_BOARD;

    const sint target_rank = square / 8, target_file = square % 8;

    for (char rank = target_rank+1; rank < 8; rank++)
    {
        attacks |= (1ULL << (rank * RANKS + target_file));
        if ((1ULL << (rank * 8 + target_file)) & board) break;  // a piece is blocking the path
    }
    for (char rank = target_rank-1; rank > -1; rank--)
    {
        attacks |= (1ULL << (rank * RANKS + target_file));
        if ((1ULL << (rank * 8 + target_file)) & board) break;  // a piece is blocking the path
    }
    for (char file = target_file+1; file < 8; file++)
    {
        attacks |= (1ULL << (target_rank * RANKS + file));
        if ((1ULL << (target_rank * RANKS + file)) & board) break;  // a piece is blocking the path
    }
    for (char file = target_file-1; file > -1; file--)
    {
        attacks |= (1ULL << (target_rank * RANKS + file));
        if ((1ULL << (target_rank * RANKS + file)) & board) break;  // a piece is blocking the path
    }

    return attacks;
}

static inline Bitboard set_occupancy(const int index, const int bits_in_mask, Bitboard attack_mask)
{
    Bitboard occupancy = EMPTY_BOARD;

    for (int bit = 0; bit < bits_in_mask; bit++)
    {
        const sint square = get_lsb(attack_mask);

        pop_bit(attack_mask, square);

        if (index & (1 << bit))
            occupancy |= (1ULL << square);
    }

    return occupancy;
}

void gameBoard:: create_leaping_pieces_attacks()
{
    for (sint square = 0; square < SQUARES; square++)
    {
        // create pawn attacks table
        pawn_attacks[white][square] = create_pawn_attack(white, square);
        pawn_attacks[black][square] = create_pawn_attack(black, square);

        // create king attacks table
        king_attacks[square] = create_king_attack(square);

        // create knight attacks table
        knight_attacks[square] = create_knight_attack(square);
    }
}

void gameBoard:: gameBoard:: create_sliding_pieces_attacks(const sliding_pieces sliding_piece)
{
    for (sint square = 0; square < SQUARES; square++)
    {
        Bitboard attack_mask;
        if (sliding_piece == bishop)
        {
            attack_mask = create_bishop_attacks(square);
            bishop_masks[square] = attack_mask;
        }
        else
        {
            attack_mask = create_rook_attacks(square);
            rook_masks[square] = attack_mask;
        }

        const sint relevant_bits = count_bits(attack_mask);

        for (int i = 0, indicies = 1 << relevant_bits; i < indicies; i++)
        {
            const Bitboard occupancy = set_occupancy(i, relevant_bits, attack_mask);

            if (sliding_piece == bishop)
            {
                const int magic_index = (occupancy * bishop_magic_numbers[square] >> (SQUARES - bishop_relevant_bits[square]));
                bishop_attacks[square][magic_index] = create_bishop_attacks_ots(square, occupancy);
            }
            else  // rook
            {
                const int magic_index = (occupancy * rook_magic_numbers[square] >> (SQUARES - rook_relevant_bits[square]));
                rook_attacks[square][magic_index] = create_rook_attacks_ots(square, occupancy);
            }
        }
    }
}
