#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum _Symbol_type
{
    FUNCTION_SYMBOL,
    IDENTIFIER_SYMBOL,
    STRUCT_SYMBOL,
} Symbol_type;

typedef struct _Symbol
{
    char *id;

    // Struct
    int size;
    int offset;

    int child_count;
    struct _Symbol **children;

    Symbol_type type;
    char *type_name;

    char *struct_name;
} Symbol;

Symbol *create_symbol(char *id, int size, Symbol_type type);

void add_symbol_child(Symbol *parent, Symbol *child);
Symbol *lookup_symbol_child(Symbol *parent, char *key);

#endif