
#ifdef WIN64
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include "../include/common.hpp"

static Bitboard keys[] = {5884158743414976048, 5884158743414976048};

static inline Bitboard rotate(Bitboard v, uint8_t s)  { return (v >> s) | (v << (64 - s)); }

Bitboard get_U64()
{
    Bitboard tmp = keys[0];
    keys[0] += rotate(keys[1] ^ 0xc5462216u ^ ((uint64_t) 0xcf14f4ebu << 32), 1);

    return keys[1] += rotate(tmp ^ 0x75ecfc58u ^ ((uint64_t) 0x9576080cu << 32), 9);
}

sint get_index(const char letter)
{
    switch (letter)
    {
        case ('P'):
            return P; break;
        case ('p'):
            return p; break;
        case ('R'):
            return R; break;
        case ('r'):
            return r; break;
        case ('B'):
            return B; break;
        case ('b'):
            return b; break;
        case ('N'):
            return N; break;
        case ('n'):
            return n; break;
        case ('K'):
            return K; break;
        case ('k'):
            return k; break;
        case ('Q'):
            return Q; break;
        default:
            return q;
    }
}

char find_promoted_piece(sint piece)
{
    switch (piece)
    {
        case (Q):
            return 'q'; break;
        case (R):
            return 'r'; break;
        case (B):
            return 'b'; break;
        case (b):
            return 'b'; break;
        case (N):
            return 'n'; break;
        case (n):
            return 'n'; break;
        default:
            return 'q';
    }
}

#define get_source_square(move) (move & 0x3f)
#define get_target_square(move) ((move & 0xfc0) >> 6)
#define promoted_piece(move) ((move & 0xf0000) >> 16)

void print_move(Move_Type move)
{
    cout << square_to_coordinates[get_source_square(move)] << square_to_coordinates[get_target_square(move)];
    int promoted = promoted_piece(move);
    if (promoted) cout << find_promoted_piece(promoted);
}

int get_time()
{
    #ifdef WIN64
        return GetTikCout();
    #else
        struct timeval time;
        gettimeofday(&time, NULL);
        return time.tv_sec * 1000 + time.tv_usec / 1000;
    #endif
}
