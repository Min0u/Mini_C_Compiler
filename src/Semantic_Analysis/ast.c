#include <stdio.h>
#include "ast.h"

ast_node *create_ast_node(int value, ast_type type, ast_node *left_child, ast_node *right_child)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    node->value = value;
    node->type = type;
    node->left_child = left_child;
    node->right_child = right_child;
    return node;
}

void print_ast(ast_node *root)
{
    if (root == NULL)
    {
        return;
    }
    switch (root->type)
    {
    case IDENTIFIER_NODE:
        printf("IDENTIFIER_NODE: %d\n", root->value);
        break;
    case CONSTANT_NODE:
        printf("CONSTANT_NODE: %d\n", root->value);
        break;
    case UNARY_EXPRESSION_NODE:
        printf("UNARY_EXPRESSION_NODE: %d\n", root->value);
        break;
    case BINARY_EXPRESSION_NODE:
        printf("BINARY_EXPRESSION_NODE: %d\n", root->value);
        break;
    case ASSIGNMENT_NODE:
        printf("ASSIGNMENT_NODE: %d\n", root->value);
        break;
    case FUNCTION_NODE:
        printf("FUNCTION_NODE: %d\n", root->value);
        break;
    case IF_ELSE_NODE:
        printf("IF_ELSE_NODE: %d\n", root->value);
        break;
    case WHILE_NODE:
        printf("WHILE_NODE: %d\n", root->value);
        break;
    case FOR_NODE:
        printf("FOR_NODE: %d\n", root->value);
        break;
    case RETURN_NODE:
        printf("RETURN_NODE: %d\n", root->value);
        break;
    case COMPOUND_NODE:
        printf("COMPOUND_NODE: %d\n", root->value);
        break;
    case DECLARATION_NODE:
        printf("DECLARATION_NODE: %d\n", root->value);
        break;
    case TYPE_SPECIFIER_NODE:
        printf("TYPE_SPECIFIER_NODE: %d\n", root->value);
        break;
    case STRUCT_SPECIFIER_NODE:
        printf("STRUCT_SPECIFIER_NODE: %d\n", root->value);
        break;
    case STRUCT_DECLARATION_NODE:
        printf("STRUCT_DECLARATION_NODE: %d\n", root->value);
        break;
    case DECLARATOR_NODE:
        printf("DECLARATOR_NODE: %d\n", root->value);
        break;
    case PARAMETER_LIST_NODE:
        printf("PARAMETER_LIST_NODE: %d\n", root->value);
        break;
    case PARAMETER_DECLARATION_NODE:
        printf("PARAMETER_DECLARATION_NODE: %d\n", root->value);
        break;
    case PROGRAM_NODE:
        printf("PROGRAM_NODE: %d\n", root->value);
        break;
    }
}

void free_ast(ast_node *root)
{
    if (root == NULL)
    {
        return;
    }
    free_ast(root->left_child);
    free_ast(root->right_child);
    free(root);
}