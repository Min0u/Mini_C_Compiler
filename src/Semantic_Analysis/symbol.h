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
    int size;
    Symbol_type type;
} Symbol;

Symbol *create_symbol(char *id, int size, Symbol_type type);

#endif