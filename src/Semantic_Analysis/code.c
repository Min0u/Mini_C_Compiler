#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int depth = 0;

void tab_depth(FILE *file)
{
    for (int i = 0; i < depth; i++)
    {
        fprintf(file, "\t");
    }
}

void write_code(ast_node *node, FILE *file)
{
    if (node == NULL)
    {
        return;
    }

    switch (node->type)
    {
    case AST_PROGRAM:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
            if (node->childrens[i]->type == AST_DECLARATION)
            {
                fprintf(file, ";\n\n");
            }
            else
            {
                fprintf(file, "\n");
            }
        }
        break;
    case AST_EXT_DECLARATION:
        fprintf(file, "extern ");
        write_code(node->childrens[0], file);
        break;

    case AST_FUNCTION_DEFINITION:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
            if (node->childrens[i]->type == AST_TYPE_SPECIFIER)
            {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
        break;
    case AST_RETURN:
        if (node->childrens_count > 0)
        {
            fprintf(file, "return ");
        }
        else
        {
            fprintf(file, "return");
        }
        write_code(node->childrens[0], file);
        break;
    case AST_FOR:
        fprintf(file, "for (");
        write_code(node->childrens[0], file);
        fprintf(file, "; ");
        write_code(node->childrens[1], file);
        fprintf(file, "; ");
        write_code(node->childrens[2], file);
        fprintf(file, ")\n");
        if (node->childrens[3]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->childrens[3], file);
        break;
    case AST_WHILE:
        fprintf(file, "while (");
        write_code(node->childrens[0], file);
        fprintf(file, ")\n");
        write_code(node->childrens[1], file);
        break;
    case AST_IF:
        fprintf(file, "if (");
        write_code(node->childrens[0], file);
        fprintf(file, ")\n");
        depth++;
        tab_depth(file);
        depth--;
        write_code(node->childrens[1], file);
        break;
    case AST_IF_ELSE:
        fprintf(file, "if (");
        write_code(node->childrens[0], file);
        fprintf(file, ")\n");
        if (node->childrens[1]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->childrens[1], file);
        if (node->childrens[1]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        fprintf(file, "else\n");
        if (node->childrens[2]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->childrens[2], file);
        if (node->childrens[2]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        break;
    case AST_EXPRESSION_STATEMENT:
        if (node->childrens_count > 0)
        {
            write_code(node->childrens[0], file);
        }
        break;
    case AST_STATEMENT_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            tab_depth(file);
            write_code(node->childrens[i], file);
            if (node->childrens[i]->type == AST_FOR || node->childrens[i]->type == AST_IF_ELSE || node->childrens[i]->type == AST_WHILE)
            {
                fprintf(file, "\n");
            } 
            else
            {
                fprintf(file, ";\n");
            }
        }
        break;
    case AST_DECLARATION_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            tab_depth(file);
            
            write_code(node->childrens[i], file);
            fprintf(file, ";\n");
        }
        break;
    case AST_COMPOUND_STATEMENT:

        tab_depth(file);
        depth++;
        fprintf(file, "{\n");
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
        }
        depth--;
        tab_depth(file);
        fprintf(file, "}");
        
        break;
    case AST_PARAMETER_DECLARATION:
        write_code(node->childrens[0], file);
        fprintf(file, " ");
        write_code(node->childrens[1], file);
        break;
    case AST_PARAMETER_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
            if (i < node->childrens_count - 1)
            {
                fprintf(file, ", ");
            }
        }
        break;
    case AST_DIRECT_DECLARATOR:
        if (node->childrens_count == 1)
        {
            write_code(node->childrens[0], file);
            fprintf(file, "()");
        }
        else
        {
            write_code(node->childrens[0], file);
            fprintf(file, "(");
            write_code(node->childrens[1], file);
            fprintf(file, ")");
        }
        break;
    case AST_DECLARATOR:
        fprintf(file, "(");
        write_code(node->childrens[0], file);
        fprintf(file, ")");
        break;
    case AST_STAR_DECLARATOR:
        fprintf(file, "*");
        write_code(node->childrens[0], file);
        break;
    case AST_STRUCT_DECLARATION:
        write_code(node->childrens[0], file);
        if (node->childrens[1]->type == AST_STAR_DECLARATOR)
        {
            fprintf(file, " ");
        }
        write_code(node->childrens[1], file);
        break;
    case AST_STRUCT_DECLARATION_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {   
            depth++;
            tab_depth(file);
            write_code(node->childrens[i], file);
            fprintf(file, ";\n");
            depth--;
        }
        break;
    case AST_STRUCT_SPECIFIER:
        if (node->childrens_count == 1)
        {
            fprintf(file, "{\n");
            depth++;
            tab_depth(file);
            write_code(node->childrens[0], file);
            depth--;
            tab_depth(file);
            fprintf(file, "}");
        }
        else if (node->childrens_count == 2)
        {
            write_code(node->childrens[0], file);
            tab_depth(file);
            fprintf(file, "{\n");
            depth++;
            tab_depth(file);
            write_code(node->childrens[1], file);
            depth--;
            tab_depth(file);
            fprintf(file, "}");
        }
        break;
    case AST_TYPE_SPECIFIER:
        fprintf(file, node->id);
        if (node->childrens_count > 0)
        {
            fprintf(file, " ");
            write_code(node->childrens[0], file);
        }
        break;
    case AST_DECLARATION:
        write_code(node->childrens[0], file);
        if (node->childrens_count > 1)
        {
            fprintf(file, " ");
            write_code(node->childrens[1], file);
        }
        break;
    case AST_ASSIGNMENT:
        write_code(node->childrens[0], file);
        fprintf(file, " = ");
        write_code(node->childrens[1], file);
        break;
    case AST_LOGICAL_OR:
        write_code(node->childrens[0], file);
        fprintf(file, " || ");
        write_code(node->childrens[1], file);
        break;
    case AST_LOGICAL_AND:
        write_code(node->childrens[0], file);
        fprintf(file, " && ");
        write_code(node->childrens[1], file);
        break;
    case AST_EQUALITY:
        write_code(node->childrens[0], file);
        fprintf(file, " == ");
        write_code(node->childrens[1], file);
        break;
    case AST_NEQUALITY:
        write_code(node->childrens[0], file);
        fprintf(file, " != ");
        write_code(node->childrens[1], file);
        break;
    case AST_L_RELATIONAL:
        write_code(node->childrens[0], file);
        fprintf(file, " < ");
        write_code(node->childrens[1], file);
        break;
    case AST_G_RELATIONAL:
        write_code(node->childrens[0], file);
        fprintf(file, " > ");
        write_code(node->childrens[1], file);
        break;
    case AST_LE_RELATIONAL:
        write_code(node->childrens[0], file);
        fprintf(file, " <= ");
        write_code(node->childrens[1], file);
        break;
    case AST_GE_RELATIONAL:
        write_code(node->childrens[0], file);
        fprintf(file, " >= ");
        write_code(node->childrens[1], file);
        break;
    case AST_ADDITIVE:
        write_code(node->childrens[0], file);
        fprintf(file, " + ");
        write_code(node->childrens[1], file);
        break;
    case AST_SUBSTRACTIVE:
        write_code(node->childrens[0], file);
        fprintf(file, " - ");
        write_code(node->childrens[1], file);
        break;
    case AST_MULTIPLICATIVE:
        write_code(node->childrens[0], file);
        fprintf(file, " * ");
        write_code(node->childrens[1], file);
        break;
    case AST_DIVISION:
        write_code(node->childrens[0], file);
        fprintf(file, " / ");
        write_code(node->childrens[1], file);
        break;
    case AST_UNARY_OPERATOR:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
        }
        break;
    case AST_UNARY_AND_OPERATOR:
        fprintf(file, "&");
        break;
    case AST_UNARY_STAR_OPERATOR:
        fprintf(file, "*");
        break;
    case AST_UNARY_MINUS_OPERATOR:
        fprintf(file, "-");
        break;
    case AST_UNARY_SIZEOF:
        fprintf(file, "sizeof");
        write_code(node->childrens[0], file);
        break;
    case AST_UNARY:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
        }
        break;
    case AST_ARGUMENT_EXPRESSION_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
            if (i < node->childrens_count - 1)
            {
                fprintf(file, ", ");
            }
        }
        break;
    case AST_POSTFIX_POINTER:
        write_code(node->childrens[0], file);
        fprintf(file, "->");
        write_code(node->childrens[1], file);
        break;
    case AST_POSTFIX_IDENTIFIER:
        write_code(node->childrens[0], file);
        fprintf(file, ".");
        write_code(node->childrens[1], file);
        break;
    case AST_POSTFIX_ARGUMENT:
        write_code(node->childrens[0], file);
        fprintf(file, "(");
        write_code(node->childrens[1], file);
        fprintf(file, ")");
        break;
    case AST_POSTFIX_NO_ARGUMENT:
        write_code(node->childrens[0], file);
        fprintf(file, "()");
        break;
    case AST_CONSTANT:
        fprintf(file, "%d", node->value);
        break;
    case AST_IDENTIFIER:
        fprintf(file, "%s", node->id);
        break;
    case AST_PRIMARY_EXPRESSION:
        fprintf(file, "(");
        write_code(node->childrens[0], file);
        fprintf(file, ")");
        break;
    case AST_STRUCT_VARIABLE_SPECIFIER:
        fprintf(file, "struct ");
        write_code(node->childrens[0], file);
        fprintf(file, "{\n");
        write_code(node->childrens[1], file);
        fprintf(file, "}");
        break;
    default:
        fprintf(file, "Unknown");
        break;
    }
}