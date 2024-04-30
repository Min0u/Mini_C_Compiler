#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _ast_type
{
    AST_PROGRAM,
    AST_FUNCTION_DEFINITION,
    AST_RETURN,
    AST_FOR,
    AST_WHILE,
    AST_IF,
    AST_IF_ELSE,
    AST_EXPRESSION_STATEMENT,
    AST_STATEMENT_LIST,
    AST_DECLARATION_LIST,
    AST_COMPOUND_STATEMENT,
    AST_PARAMETER_DECLARATION,
    AST_PARAMETER_LIST,
    AST_DIRECT_DECLARATOR,
    AST_STRUCT_DECLARATION,
    AST_STRUCT_DECLARATION_LIST,
    AST_STRUCT_SPECIFIER,
    AST_TYPE_SPECIFIER,
    AST_DECLARATION_SPECIFIERS,
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_LOGICAL_OR,
    AST_LOGICAL_AND,
    AST_EQUALITY,
    AST_RELATIONAL,
    AST_ADDITIVE,
    AST_MULTIPLICATIVE,
    AST_UNARY_OPERATOR,
    AST_UNARY,
    AST_ARGUMENT_EXPRESSION_LIST,
    AST_POSTFIX_EXPRESSION,
    AST_CONSTANT,
    AST_IDENTIFIER,
    AST_PRIMARY_EXPRESSION,
    AST_DECLARATOR,
} ast_type;

typedef struct _ast_node
{
    ast_type type;
    struct _ast_node **childrens;
    int childrens_count;
    int value;
    char *id;
} ast_node;

ast_node *ast_create_node(ast_type type);

void ast_add_child(ast_node *parent, ast_node *child);

ast_node *create_int_leaf(int value);
ast_node *create_id_leaf(char *name);

void print_complete_ast(ast_node *root);
void print_complete_ast_helper(ast_node *node, int indent);

void free_ast(ast_node *root);

#endif