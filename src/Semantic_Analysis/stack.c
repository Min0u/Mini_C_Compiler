#include "stack.h"

Stack *create_stack()
{
    Stack *s = (Stack *)malloc(sizeof(Stack));
    s->top = NULL;
    return s;
}

////////////////////////////////////////////////////////////////////////////////////////

void push(Stack *s, Hash_map *map)
{
    Stack_node *node = (Stack_node *)malloc(sizeof(Stack_node));
    node->map = map;
    node->next = s->top;
    s->top = node;
}

void push_symbol(Stack *s, char *key, Symbol *value)
{
    insert(s->top->map, key, value);
}

void push_symbol_next(Stack *s, char *key, Symbol *value)
{
    Stack_node *node = s->top->next;
    if (node == NULL)
    {
        node = (Stack_node *)malloc(sizeof(Stack_node));
        node->map = create_hash_map();
        node->next = NULL;
        s->top->next = node;
    }
    insert(node->map, key, value);
}

Hash_map *pop(Stack *s)
{
    if (is_empty(s))
    {
        return NULL;
    }
    Stack_node *node = s->top;
    s->top = node->next;
    Hash_map *map = node->map;
    free(node);
    return map;
}

Hash_map *peek(Stack *s)
{
    if (is_empty(s))
    {
        return NULL;
    }
    return s->top->map;
}

////////////////////////////////////////////////////////////////////////////////////////

bool is_empty(Stack *s)
{
    return s->top == NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

Symbol *lookup_stack(Stack *s, char *key)
{
    Stack_node *node = s->top;
    while (node != NULL)
    {
        Symbol *s = lookup(node->map, key);
        if (s != NULL)
        {
            return s;
        }
        node = node->next;
    }
    return NULL;
}

Symbol *lookup_stack_top(Stack *s, char *key)
{
    return lookup(peek(s), key);
}