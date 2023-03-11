#include "../include/transposition_table.hpp"

void create_transposition_table(transposition_table* TT, unsigned int size)
{
    try
    {
        *TT = new _transposition_table;
        if (*TT == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    try
    {
        (*TT)->nodes = new _tt_node[size];
        if ((*TT)->nodes == NULL) throw MEMORY_ALLOCATION_MESSAGE;
    } catch (const char* message) { cerr << message << endl; }

    (*TT)->size = size;

    tt_clear(*TT);
}

void destroy_transposition_table(transposition_table TT)
{
    delete[] TT->nodes;
    delete TT;
}
