#include "../include/board.hpp"

static inline Bitboard set_mask(const sint rank_number, const sint file_number)
{
    Bitboard mask = EMPTY_BOARD;
    
    for (sint rank = 0; rank < RANKS; rank++)
    {
        for (sint file = 0; file < FILES; file++)
        {
            const sint square = get_square(rank, file);
            
            if (file_number != -1 && file == file_number)
                mask |= set_bit(mask, square);
            
            else if (rank_number != -1 && rank == rank_number)
                mask |= set_bit(mask, square);
        }
    }
    
    return mask;
}

void gameBoard:: create_evaluation_tables()
{
    // allocate memory for the history table
    try
    {
        History = new Bitboard[REPETITION_TABLE_SIZE];
        if (History == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        file_masks = new Bitboard[SQUARES];
        if (file_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        rank_masks = new Bitboard[SQUARES];
        if (rank_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        isolated_masks = new Bitboard[SQUARES];
        if (isolated_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        passed_masks = new Bitboard*[2];
        if (passed_masks == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    // white & black
    for (sint i = 0; i < 2; i++)
    {
        try
        {
            passed_masks[i] = new Bitboard[SQUARES];
            if (passed_masks[i] == NULL) throw MEMORY_ALLOCATION_MESSAGE;
        } catch (const char* message) { cerr << message << endl; }
    }

    for (sint rank = 0; rank < RANKS; rank++)
    {
        for (sint file = 0; file < FILES; file++)
        {
            const sint square = get_square(rank, file);

            // file & rank masks
            rank_masks[square] |= set_mask(file, -1);
            file_masks[square] |= set_mask(-1, file);
            
            // isolated pawns
            isolated_masks[square] |= set_mask(-1, file - 1);
            isolated_masks[square] |= set_mask(-1, file + 1);

            // white passed pawns
            passed_masks[white][square] |= set_mask(-1, file - 1);
            passed_masks[white][square] |= set_mask(-1, file);
            passed_masks[white][square] |= set_mask(-1, file + 1);

            for (sint i = 0, end = 8 - rank; i < end; i++)
                passed_masks[white][square] &= ~rank_masks[(7 - i) * 8 + file];

            // black passed pawns
            passed_masks[black][square] |= set_mask(-1, file - 1);
            passed_masks[black][square] |= set_mask(-1, file);
            passed_masks[black][square] |= set_mask(-1, file + 1);
            
            for (sint i = 0; i < rank + 1; i++)
                passed_masks[black][square] &= ~rank_masks[i * 8 + file];
        }
    }
}

void gameBoard:: destroy_evaluation_tables()
{
    delete[] file_masks;
    delete[] rank_masks;
    delete[] isolated_masks;

    for (sint i = 0; i < 2; i++)
        delete[] passed_masks[i];
    delete[] passed_masks;
}
