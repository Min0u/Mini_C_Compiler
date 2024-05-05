#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#ifndef _STDC_WANT_LIB_EXT2_
#define _STDC_WANT_LIB_EXT2_ 1
#endif

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
    AST_DECLARATION,
    AST_ASSIGNMENT,
    AST_BOOL_LOGIC,
    AST_BOOL_OP,
    AST_OP,
    AST_UNARY_OPERATOR,
    AST_UNARY_OP,
    AST_UNARY_SIZEOF,
    AST_UNARY,
    AST_ARGUMENT_EXPRESSION_LIST,
    AST_POSTFIX_POINTER,
    AST_POSTFIX_IDENTIFIER,
    AST_POSTFIX_ARGUMENT,
    AST_POSTFIX_NO_ARGUMENT,
    AST_CONSTANT,
    AST_IDENTIFIER,
    AST_PRIMARY_EXPRESSION,
    AST_DECLARATOR,
    AST_STAR_DECLARATOR,
    AST_STRUCT_VARIABLE_SPECIFIER,
    AST_EXT_DECLARATION,
} ast_type;

typedef struct _ast_node
{
    ast_type type;
    struct _ast_node *parent;
    struct _ast_node **childrens;
    int childrens_count;
    int value;
    char *id;

    // For boolean expressions
    int true_label;
    int false_label;

    // Temporary variable
    bool available;

    // Sethi-Ullman number
    int sethi_ullman;
} ast_node;

ast_node *ast_create_node(ast_type type);
ast_node *create_int_leaf(int value);
ast_node *create_id_leaf(char *name);

void ast_add_child(ast_node *parent, ast_node *child);
// Add a child to the front of other children
void ast_add_child_front(ast_node *parent, ast_node *child);
// Add a child before a specific child
void ast_add_child_before(ast_node *parent, ast_node *child, ast_node *before);
// Add a declaration to the declaration list
ast_node *ast_add_temporary(ast_node *node);

// Find a declaration list node
ast_node *find_declaration_list(ast_node *node);
// Find a parent of a node with a specific type
ast_node *find_parent(ast_node *node, ast_type type);
// Find the last parent before a parent with a specific type
ast_node *find_last_parent_before(ast_node *node, ast_type type);
// Find an available temporary variable
ast_node *find_available_temporary(ast_node *node);

// Get an available temporary variable
ast_node *get_temporary(ast_node *node);
// Get all available temporary variables
char **get_all_temporaries(ast_node *node, int *count);

// Mark a temporary variable as available
void available_temporary(ast_node *node, char *name);

// Add a variable to the declaration list
ast_node *split_node_into_var(ast_node *node, char *name);
// Add a temporary variable to the declaration list
ast_node *split_node_into_temp_var(ast_node *node);
// Get the index of a variable
int split_op(ast_node *node, char *tab[], bool tab_bool[], int n);

// Sethi-Ullman
void sethi_ullman(ast_node *node);

// Transform the AST to TAC (Three Address Code)
void *tac_transformation(ast_node *node);

void free_ast(ast_node *root);

void print_complete_ast(ast_node *root);
void print_complete_ast_helper(ast_node *node, int indent);

#endif