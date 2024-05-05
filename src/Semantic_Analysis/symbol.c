#include "symbol.h"

Symbol *create_symbol(char *id, int size, Symbol_type type)
{
    Symbol *s = (Symbol *)malloc(sizeof(Symbol));
    s->id = strdup(id);
    s->size = size;
    s->type = type;
    return s;
}