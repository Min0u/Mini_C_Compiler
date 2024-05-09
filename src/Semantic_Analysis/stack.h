#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashmap.h"
#include "symbol.h"

typedef struct _Stack_node
{
    struct _Hash_map *map;
    struct _Stack_node *next;
} Stack_node;

typedef struct _Stack
{
    struct _Stack_node *top;
} Stack;

Stack *create_stack();

void push(Stack *s, Hash_map *map);
// Insert symbol into the top of the stack
void push_symbol(Stack *s, char *key, Symbol *value);
// Insert symbol into the next stack
void push_symbol_next(Stack *s, char *key, Symbol *value);
// Pop the top element from the stack : return the top element and remove it
struct _Hash_map *pop(Stack *s);
// Peek the top element from the stack : return the top element without removing it
struct _Hash_map *peek(Stack *s);

bool is_empty(Stack *s);

// Lookup a key in the stack
Symbol *lookup_stack(Stack *s, char *key);
// Look for a key on top of the stack
Symbol *lookup_stack_top(Stack *s, char *key);

void print_stack(Stack *s);

#endif
