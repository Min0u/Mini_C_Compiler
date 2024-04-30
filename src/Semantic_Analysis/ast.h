#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _ast_type
{
    IDENTIFIER_NODE,
    CONSTANT_NODE,
    UNARY_EXPRESSION_NODE,
    BINARY_EXPRESSION_NODE,
    ASSIGNMENT_NODE,
    FUNCTION_NODE,
    IF_ELSE_NODE,
    WHILE_NODE,
    FOR_NODE,
    RETURN_NODE,
    COMPOUND_NODE,
    DECLARATION_NODE,
    TYPE_SPECIFIER_NODE,
    STRUCT_SPECIFIER_NODE,
    STRUCT_DECLARATION_NODE,
    DECLARATOR_NODE,
    PARAMETER_LIST_NODE,
    PARAMETER_DECLARATION_NODE,
    PROGRAM_NODE,
} ast_type;

typedef struct _ast_node
{
    int value;
    ast_type type;
    struct _ast_node *left_child;
    struct _ast_node *right_child;
} ast_node;

ast_node *create_ast_node(int value, ast_type type, ast_node *left_child, ast_node *right_child);
void print_ast(ast_node *root);
void free_ast(ast_node *root);

#endif