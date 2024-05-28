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

typedef enum _Ast_type
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
    AST_DIRECT_DECLARATOR_END,
    AST_STRUCT_DECLARATION_LIST,
    AST_STRUCT,
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
    AST_POSTFIX,
    AST_CONSTANT,
    AST_IDENTIFIER,
    AST_PRIMARY_EXPRESSION,
    AST_DECLARATOR,
    AST_STAR_DECLARATOR,
    AST_EXT_DECLARATION,
} Ast_type;

typedef struct _Ast_node
{
    Ast_type type;
    char *type_name;

    struct _Ast_node *parent;
    struct _Ast_node **children;
    int children_count;

    int value;
    char *id;

    // Variable size
    int size;
    int offset;

    // Boolean expressions
    int true_label;
    int false_label;

    // Temporary variable
    bool available;

    // Sethi-Ullman number
    int sethi_ullman;

    // Error
    char *struct_name;
    bool pointer;
} Ast_node;

Ast_node *ast_create_node(Ast_type type);
Ast_node *create_int_leaf(int value);
Ast_node *create_id_leaf(char *name);

void ast_add_child(Ast_node *parent, Ast_node *child);
// Add a child to the front of other children
void ast_add_child_front(Ast_node *parent, Ast_node *child);
// Add a child before a specific child
void ast_add_child_before(Ast_node *parent, Ast_node *child, Ast_node *before);
// Add a declaration to the declaration list
Ast_node *ast_add_temporary(Ast_node *node, char *type, bool pointer);

// Find a declaration list node
Ast_node *find_declaration_list(Ast_node *node);
// Find a parent of a node with a specific type
Ast_node *find_parent(Ast_node *node, Ast_type type);
// Find the last parent before a parent with a specific type
Ast_node *find_last_parent_before(Ast_node *node, Ast_type type);
// Find an available temporary variable
Ast_node *find_available_temporary(Ast_node *node, char *type, bool pointer);
// Find the first identifier starting from a node
char *find_first_identifier(Ast_node *node);

// Get an available temporary variable, create a new one if none is available
Ast_node *get_temporary(Ast_node *node, char *type, bool pointer);
// Get all available temporary variables
char **get_all_temporaries(Ast_node *node, int *count);

// Mark a temporary variable as available
void available_temporary(Ast_node *node, char *name);

// Add a variable to the statement list, and return the variable that we'll use to replace the node
Ast_node *split_node_into_var(Ast_node *node, char *name);
// Add a temporary variable to the declaration list, if a temporary variable is already available, it is used otherwise a new one is created
Ast_node *split_node_into_temp_var(Ast_node *node, char *type, bool pointer);
// Split a node into a temporary variable and return the index of the temporary variable
int split_op(Ast_node *node, char *tab[], bool tab_bool[], int n);

// Sethi-Ullman
void sethi_ullman(Ast_node *node);

// Transform the AST to TAC (Three Address Code)
void tac_transformation(Ast_node *node);

void free_ast(Ast_node *root);

void print_complete_ast(Ast_node *root);
void print_complete_ast_helper(Ast_node *node, int indent);

#endif