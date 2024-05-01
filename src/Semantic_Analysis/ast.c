#include <stdio.h>
#include "ast.h"

ast_node *ast_create_node(ast_type type)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    node->type = type;
    node->childrens = NULL;
    node->childrens_count = 0;
    node->value = -1;
    node->id = NULL;
    
    return node;
}

void ast_add_child(ast_node *parent, ast_node *child)
{
    if (parent->childrens == NULL)
    {
        parent->childrens = (ast_node **)malloc(sizeof(ast_node *));
    }
    else
    {
        parent->childrens = (ast_node **)realloc(parent->childrens, sizeof(ast_node *) * (parent->childrens_count + 1));
    }
    
    parent->childrens[parent->childrens_count] = child;
    parent->childrens_count++;
}

ast_node *create_int_leaf(int value)
{
    ast_node *node = ast_create_node(AST_CONSTANT);
    node->value = value;
    return node;
}

ast_node *create_id_leaf(char *name)
{
    ast_node *node = ast_create_node(AST_IDENTIFIER);
    node->id = name;
    return node;
}

void print_complete_ast_helper(ast_node *node, int indent)
{
    if (node == NULL)
    {
        return;
    }

    for (int i = 0; i < indent - 1; i++)
    {
        printf("    ");
    }
    if (indent > 0)
    {
        printf("└── ");
    }

    switch (node->type)
    {
    case AST_PROGRAM:
        printf("Program\n");
        break;
    case AST_PROGRAM_EXT:
        printf("Program Ext\n");
        break;
    case AST_FUNCTION_DEFINITION:
        printf("Function Definition\n");
        break;
    case AST_RETURN:
        printf("Return\n");
        break;
    case AST_FOR:
        printf("For\n");
        break;
    case AST_WHILE:
        printf("While\n");
        break;
    case AST_IF:
        printf("If\n");
        break;
    case AST_IF_ELSE:
        printf("If Else\n");
        break;
    case AST_EXPRESSION_STATEMENT:
        printf("Expression Statement\n");
        break;
    case AST_STATEMENT_LIST:
        printf("Statement List\n");
        break;
    case AST_DECLARATION_LIST:
        printf("Declaration List\n");
        break;
    case AST_COMPOUND_STATEMENT:
        printf("Compound Statement\n");
        break;
    case AST_PARAMETER_DECLARATION:
        printf("Parameter Declaration\n");
        break;
    case AST_PARAMETER_LIST:
        printf("Parameter List\n");
        break;
    case AST_DIRECT_DECLARATOR:
        printf("Direct Declarator\n");
        break;
    case AST_STRUCT_DECLARATION:
        printf("Struct Declaration\n");
        break;
    case AST_STRUCT_DECLARATION_LIST:
        printf("Struct Declaration List\n");
        break;
    case AST_STRUCT_SPECIFIER:
        printf("Struct Specifier\n");
        break;
    case AST_TYPE_SPECIFIER:
        printf("Type Specifier : %s\n", node->id);
        break;
    case AST_DECLARATION_SPECIFIERS:
        printf("Declaration Specifiers\n");
        break;
    case AST_DECLARATION:
        printf("Declaration\n");
        break;
    case AST_ASSIGNMENT:
        printf("Assignment\n");
        break;
    case AST_LOGICAL_OR:
        printf("Logical Or\n");
        break;
    case AST_LOGICAL_AND:
        printf("Logical And\n");
        break;
    case AST_EQUALITY:
        printf("Equality\n");
        break;
    case AST_NEQUALITY:
        printf("Not Equality\n");
        break;
    case AST_LE_RELATIONAL:
        printf("Less or Equal Relational\n");
        break;
    case AST_GE_RELATIONAL:
        printf("Greater or Equal Relational\n");
        break;
    case AST_L_RELATIONAL:
        printf("Less Relational\n");
        break;
    case AST_G_RELATIONAL:
        printf("Greater Relational\n");
        break;
    case AST_ADDITIVE:
        printf("Additive\n");
        break;
    case AST_SUBSTRACTIVE:
        printf("Subtractive\n");
        break;
    case AST_MULTIPLICATIVE:
        printf("Multiplicative\n");
        break;
    case AST_DIVISION:
        printf("Division\n");
        break;
    case AST_UNARY_OPERATOR:
        printf("Unary Operator\n");
        break;
    case AST_UNARY_AND_OPERATOR:
        printf("Unary And Operator\n");
        break;
    case AST_UNARY_STAR_OPERATOR:
        printf("Unary Star Operator\n");
        break;
    case AST_UNARY_MINUS_OPERATOR:
        printf("Unary Minus Operator\n");
        break;
    case AST_UNARY_SIZEOF:
        printf("Unary Sizeof\n");
        break;
    case AST_UNARY:
        printf("Unary\n");
        break;
    case AST_ARGUMENT_EXPRESSION_LIST:
        printf("Argument Expression List\n");
        break;
    case AST_POSTFIX_POINTER:
        printf("Postfix Pointer\n");
        break;
    case AST_POSTFIX_IDENTIFIER:
        printf("Postfix Identifier\n");
        break;
    case AST_POSTFIX_ARGUMENT:
        printf("Postfix Argument\n");
        break;
    case AST_POSTFIX_NO_ARGUMENT:
        printf("Postfix No Argument\n");
        break;
    case AST_CONSTANT:
        printf("Constant : %d\n", node->value);
        break;
    case AST_IDENTIFIER:
        printf("Identifier : %s\n", node->id);
        break;
    case AST_PRIMARY_EXPRESSION:
        printf("Primary Expression\n");
        break;
    case AST_DECLARATOR:
        printf("Declarator\n");
        break;
    case AST_STAR_DECLARATOR:
        printf("Star Declarator\n");
        break;
    case AST_STRUCT_VARIABLE_SPECIFIER:
        printf("Struct Variable Specifier\n");
        break;
    default:
        printf("Unknown\n");
        break;
    }

    for (int i = 0; i < node->childrens_count; i++)
    {
        print_complete_ast_helper(node->childrens[i], indent + 1);
    }
}

void print_complete_ast(ast_node *root)
{
    print_complete_ast_helper(root, 0);
}

void free_ast(ast_node *root)
{
    if (root == NULL)
    {
        return;
    }
    free_ast(root->childrens);
    free(root);
}