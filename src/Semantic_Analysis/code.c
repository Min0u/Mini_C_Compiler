#include <stdio.h>

#include "code.h"

int depth = 0;
int bool_counter = 0;

void print_error(Symbol *symbol, char *id, int line)
{
    print_color("31", "Error : ");

    char *error;

    if (symbol->type == FUNCTION_SYMBOL)
    {
        asprintf(&error, "Function \033[1m\"%s\"\033[0m already declared (line %d)", id, line);
    }
    else if (symbol->type == IDENTIFIER_SYMBOL)
    {
        asprintf(&error, "Identifier \033[1m\"%s\"\033[0m already declared (line %d)", id, line);
    }
    else
    {
        asprintf(&error, "Structure \033[1m\"%s\"\033[0m already declared (line %d)", id, line);
    }

    printf("%s", error);
}

void print_warning(Symbol *symbol, char *id, int line)
{
    print_color("35", "Warning : ");

    char *warning;

    if (symbol->type == FUNCTION_SYMBOL)
    {
        asprintf(&warning, "Overriding function \033[1m\"%s\"\033[0m (line %d)", id, line);
    }
    else if (symbol->type == IDENTIFIER_SYMBOL)
    {
        asprintf(&warning, "Overriding identifier \033[1m\"%s\"\033[0m (line %d)", id, line);
    }
    else
    {
        asprintf(&warning, "Overriding structure \033[1m\"%s\"\033[0m (line %d)", id, line);
    }

    printf("%s\n", warning);
}

void print_color(char *color, char *text)
{
    printf("\033[1;%sm%s", color, text);
    printf("\033[0m");
}

////////////////////////////////////////////////////////////////////////////////////////

void tab_depth(FILE *file)
{
    for (int i = 0; i < depth; i++)
    {
        fprintf(file, "\t");
    }
}

////////////////////////////////////////////////////////////////////////////////////////

char *inv_op(char *op)
{
    if (strcmp(op, "<") == 0)
    {
        return ">=";
    }
    else if (strcmp(op, "<=") == 0)
    {
        return ">";
    }
    else if (strcmp(op, ">") == 0)
    {
        return "<=";
    }
    else if (strcmp(op, ">=") == 0)
    {
        return "<";
    }
    else if (strcmp(op, "==") == 0)
    {
        return "!=";
    }
    else if (strcmp(op, "!=") == 0)
    {
        return "==";
    }
    else if (strcmp(op, "||") == 0)
    {
        return "&&";
    }
    else if (strcmp(op, "&&") == 0)
    {
        return "||";
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

void write_code(Ast_node *node, FILE *file)
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
        write_code(node->childrens[0], file);
        fprintf(file, ";\n");
        node->childrens[1]->true_label = bool_counter;
        node->childrens[1]->false_label = bool_counter;
        bool_counter++;
        fprintf(file, "test_%d:\n", node->childrens[1]->true_label);
        tab_depth(file);
        write_code(node->childrens[1], file);
        if (node->childrens[3]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->childrens[3], file);
        if (node->childrens[3]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        write_code(node->childrens[2], file);
        if (node->childrens[1]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        fprintf(file, "goto test_%d;", node->childrens[1]->true_label);
        fprintf(file, "\nfalse_%d:", node->childrens[1]->true_label);
        Ast_node *parent_f = node->parent;
        Ast_node **childrens_f = parent_f->childrens;
        Ast_node *last_f = childrens_f[parent_f->childrens_count - 1];
        if (last_f == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_WHILE:
        node->childrens[0]->true_label = bool_counter;
        node->childrens[0]->false_label = bool_counter;
        bool_counter++;
        fprintf(file, "test_%d:\n", node->childrens[0]->true_label);
        tab_depth(file);
        write_code(node->childrens[0], file);
        write_code(node->childrens[1], file);
        fprintf(file, ";\n");
        tab_depth(file);
        fprintf(file, "goto test_%d;\n", node->childrens[0]->false_label);
        fprintf(file, "\nfalse_%d:", node->childrens[0]->false_label);
        Ast_node *parent_w = node->parent;
        Ast_node **childrens_w = parent_w->childrens;
        Ast_node *last_w = childrens_w[parent_w->childrens_count - 1];
        if (last_w == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_IF:
        node->childrens[0]->true_label = bool_counter;
        node->childrens[0]->false_label = bool_counter;
        bool_counter++;
        write_code(node->childrens[0], file);
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
        fprintf(file, "\nfalse_%d:", node->childrens[0]->true_label);
        Ast_node *parent_i = node->parent;
        Ast_node **childrens_i = parent_i->childrens;
        Ast_node *last_i = childrens_i[parent_i->childrens_count - 1];
        if (last_i == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_IF_ELSE:
        node->childrens[0]->true_label = bool_counter;
        node->childrens[0]->false_label = bool_counter;
        bool_counter++;
        write_code(node->childrens[0], file);
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
        fprintf(file, "goto end_%d;\n", node->childrens[0]->false_label);
        fprintf(file, "false_%d:\n", node->childrens[0]->true_label);
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
        fprintf(file, "\nend_%d:", node->childrens[0]->false_label);
        Ast_node *parent = node->parent;
        Ast_node **childrens = parent->childrens;
        Ast_node *last = childrens[parent->childrens_count - 1];
        if (last == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_EXPRESSION_STATEMENT:
        if (node->parent != NULL && node->parent->type == AST_FOR)
        {
            node->childrens[0]->true_label = node->true_label;
            node->childrens[0]->false_label = node->false_label;
        }
        write_code(node->childrens[0], file);
        break;
    case AST_STATEMENT_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            if (node->childrens[i]->type != AST_WHILE)
            {
                tab_depth(file);
            }

            write_code(node->childrens[i], file);
            if (node->childrens[i]->type == AST_FOR || node->childrens[i]->type == AST_IF_ELSE || node->childrens[i]->type == AST_WHILE || node->childrens[i]->type == AST_IF)
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
        if (node->parent->type == AST_FUNCTION_DEFINITION)
        {
            fprintf(file, "\n");
        }
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
        write_code(node->childrens[0], file);
        break;
    case AST_DIRECT_DECLARATOR_END:
        write_code(node->childrens[0], file);
        fprintf(file, "(");
        if (node->childrens_count == 2)
        {
            write_code(node->childrens[1], file);
        }
        fprintf(file, ")");
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
        fprintf(file, " ");
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
        fprintf(file, "%s", node->id);
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
    case AST_BOOL_LOGIC:
        if (node->id[0] == '&')
        {
            node->childrens[0]->false_label = node->false_label;
            node->childrens[1]->false_label = node->false_label;
            write_code(node->childrens[0], file);
            tab_depth(file);
            write_code(node->childrens[1], file);
        }
        else
        {
            node->childrens[0]->false_label = bool_counter;
            bool_counter++;
            node->childrens[0]->true_label = node->true_label;

            node->childrens[1]->false_label = node->false_label;
            node->childrens[1]->true_label = node->true_label;

            write_code(node->childrens[0], file);
            tab_depth(file);
            fprintf(file, "goto true_%d;\n", node->childrens[0]->true_label);
            fprintf(file, "false_%d:\n", node->childrens[0]->false_label);
            tab_depth(file);
            write_code(node->childrens[1], file);
            fprintf(file, "true_%d:\n", node->childrens[0]->true_label);
        }
        break;
    case AST_BOOL_OP:
        fprintf(file, "if (");
        write_code(node->childrens[0], file);
        char *inv = inv_op(node->id);
        fprintf(file, " %s ", inv);
        write_code(node->childrens[1], file);
        fprintf(file, ") goto false_%d;\n", node->false_label);
        break;
    case AST_OP:
        write_code(node->childrens[0], file);
        fprintf(file, " %s ", node->id);
        write_code(node->childrens[1], file);
        break;
    case AST_UNARY_OPERATOR:
        for (int i = 0; i < node->childrens_count; i++)
        {
            write_code(node->childrens[i], file);
        }
        break;
    case AST_UNARY_OP:
        fprintf(file, "%s", node->id);
        break;
    case AST_UNARY_SIZEOF:
        fprintf(file, "sizeof");
        if (node->childrens[0]->type == AST_TYPE_SPECIFIER)
        {
            fprintf(file, "(");
            write_code(node->childrens[0], file);
            fprintf(file, ")");
        }
        else
        {
            write_code(node->childrens[0], file);
        }
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