#include "symbol.h"

Symbol *create_symbol(char *id, int size, Symbol_type type)
{
    Symbol *s = (Symbol *)malloc(sizeof(Symbol));

    s->id = strdup(id);

    s->size = size;
    s->offset = -1;

    s->child_count = 0;
    s->children = NULL;

    s->type = type;
    s->type_name = NULL;

    s->struct_name = NULL;
    return s;
}

////////////////////////////////////////////////////////////////////////////////////////

void add_symbol_child(Symbol *parent, Symbol *child)
{
    parent->children = (Symbol **)realloc(parent->children, (parent->child_count + 1) * sizeof(Symbol *));

    parent->children[parent->child_count] = child;
    parent->child_count++;
}

Symbol *lookup_symbol_child(Symbol *parent, char *key)
{
    for (int i = 0; i < parent->child_count; i++)
    {
        if (strcmp(parent->children[i]->id, key) == 0)
        {
            return parent->children[i];
        }
    }
    return NULL;
}