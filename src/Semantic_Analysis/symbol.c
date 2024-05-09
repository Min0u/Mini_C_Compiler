#include "symbol.h"
#
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

    s->pointer = false;
    return s;
}

////////////////////////////////////////////////////////////////////////////////////////

void add_symbol_child(Symbol *parent, Symbol *child)
{
    parent->children = (Symbol **)realloc(parent->children, (parent->child_count + 1) * sizeof(Symbol *));

    parent->children[parent->child_count] = child;
    parent->child_count++;
}

////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////

void function_arguments(Ast_node *node, Symbol *symbol)
{
    for (int i = 0; i < node->children_count; i++)
    {
        Ast_node *child = node->children[i];

        if (child->type == AST_PARAMETER_LIST)
        {
            for (int j = 0; j < child->children_count; j++)
            {
                Ast_node *parameter = child->children[j];
                char *id = find_first_identifier(parameter->children[1]);

                Symbol *s = create_symbol(id, parameter->size, IDENTIFIER_SYMBOL);
                s->type_name = strdup(parameter->children[0]->id);

                if (strcmp(s->type_name, "struct") == 0)
                {
                    s->struct_name = find_first_identifier(parameter->children[0]->children[0]);
                }

                if (parameter->children[1]->type == AST_STAR_DECLARATOR)
                {
                    s->pointer = true;
                }

                add_symbol_child(symbol, s);
            }
        }
    }
}

void add_return_symbol(Ast_node *node, Symbol *symbol)
{
    Ast_node *child = node->children[0];
    Ast_node *child2 = node->children[1];

    Symbol *s = create_symbol("return", -1, IDENTIFIER_SYMBOL);

    if (child->type == AST_TYPE_SPECIFIER)
    {
        s->type_name = strdup(child->id);
        s->size = child->size;
    }
    else
    {
        s->type_name = strdup(child->children[0]->id);
        s->size = child->children[0]->size;
    }

    if (strcmp(s->type_name, "struct") == 0)
    {
        s->struct_name = find_first_identifier(child);
    }

    if (child2->type == AST_STAR_DECLARATOR)
    {
        s->pointer = true;
    }

    add_symbol_child(symbol, s);
}