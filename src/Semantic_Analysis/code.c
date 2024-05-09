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
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
            if (node->children[i]->type == AST_DECLARATION)
            {
                if (node->children[i]->children[0]->type != AST_STRUCT)
                {
                    fprintf(file, ";\n\n");
                }
            }
            else
            {
                fprintf(file, "\n");
            }
        }
        break;
    case AST_EXT_DECLARATION:
        fprintf(file, "extern ");
        write_code(node->children[0], file);
        break;
    case AST_FUNCTION_DEFINITION:
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
            if (node->children[i]->type == AST_TYPE_SPECIFIER)
            {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
        break;
    case AST_RETURN:
        if (node->children_count > 0)
        {
            fprintf(file, "return ");
        }
        else
        {
            fprintf(file, "return");
        }
        write_code(node->children[0], file);
        break;
    case AST_FOR:
        write_code(node->children[0], file);
        fprintf(file, ";\n");
        node->children[1]->true_label = bool_counter;
        node->children[1]->false_label = bool_counter;
        bool_counter++;
        fprintf(file, "test_%d:\n", node->children[1]->true_label);
        tab_depth(file);
        write_code(node->children[1], file);
        if (node->children[3]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->children[3], file);
        if (node->children[3]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        write_code(node->children[2], file);
        if (node->children[1]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        fprintf(file, "goto test_%d;", node->children[1]->true_label);
        fprintf(file, "\nfalse_%d:", node->children[1]->true_label);
        Ast_node *parent_f = node->parent;
        Ast_node **children_f = parent_f->children;
        Ast_node *last_f = children_f[parent_f->children_count - 1];
        if (last_f == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_WHILE:
        node->children[0]->true_label = bool_counter;
        node->children[0]->false_label = bool_counter;
        bool_counter++;
        fprintf(file, "test_%d:\n", node->children[0]->true_label);
        tab_depth(file);
        write_code(node->children[0], file);
        write_code(node->children[1], file);
        fprintf(file, ";\n");
        tab_depth(file);
        fprintf(file, "goto test_%d;\n", node->children[0]->false_label);
        fprintf(file, "\nfalse_%d:", node->children[0]->false_label);
        Ast_node *parent_w = node->parent;
        Ast_node **children_w = parent_w->children;
        Ast_node *last_w = children_w[parent_w->children_count - 1];
        if (last_w == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_IF:
        node->children[0]->true_label = bool_counter;
        node->children[0]->false_label = bool_counter;
        bool_counter++;
        write_code(node->children[0], file);
        if (node->children[1]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->children[1], file);
        if (node->children[1]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\nfalse_%d:", node->children[0]->true_label);
        Ast_node *parent_i = node->parent;
        Ast_node **children_i = parent_i->children;
        Ast_node *last_i = children_i[parent_i->children_count - 1];
        if (last_i == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_IF_ELSE:
        node->children[0]->true_label = bool_counter;
        node->children[0]->false_label = bool_counter;
        bool_counter++;
        write_code(node->children[0], file);
        if (node->children[1]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->children[1], file);
        if (node->children[1]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\n");
        tab_depth(file);
        fprintf(file, "goto end_%d;\n", node->children[0]->false_label);
        fprintf(file, "false_%d:\n", node->children[0]->true_label);
        if (node->children[2]->type != AST_COMPOUND_STATEMENT)
        {
            depth++;
            tab_depth(file);
            depth--;
        }
        write_code(node->children[2], file);
        if (node->children[2]->type != AST_COMPOUND_STATEMENT)
        {
            fprintf(file, ";");
        }
        fprintf(file, "\nend_%d:", node->children[0]->false_label);
        Ast_node *parent = node->parent;
        Ast_node **children = parent->children;
        Ast_node *last = children[parent->children_count - 1];
        if (last == node)
        {
            fprintf(file, ";");
        }
        break;
    case AST_EXPRESSION_STATEMENT:
        if (node->parent != NULL && node->parent->type == AST_FOR)
        {
            node->children[0]->true_label = node->true_label;
            node->children[0]->false_label = node->false_label;
        }
        write_code(node->children[0], file);
        break;
    case AST_STATEMENT_LIST:
        for (int i = 0; i < node->children_count; i++)
        {
            if (node->children[i]->type != AST_WHILE)
            {
                tab_depth(file);
            }

            write_code(node->children[i], file);
            if (node->children[i]->type == AST_FOR || node->children[i]->type == AST_IF_ELSE || node->children[i]->type == AST_WHILE || node->children[i]->type == AST_IF)
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
        for (int i = 0; i < node->children_count; i++)
        {
            tab_depth(file);
            write_code(node->children[i], file);
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
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
        }
        depth--;
        tab_depth(file);
        fprintf(file, "}");

        break;
    case AST_PARAMETER_DECLARATION:
        write_code(node->children[0], file);
        fprintf(file, " ");
        if (node->children[1]->type == AST_STAR_DECLARATOR && node->children[1]->children[0]->type == AST_DIRECT_DECLARATOR_END)
        {
            fprintf(file, "*");
            char *id = find_first_identifier(node->children[1]->children[0]->children[0]);
            fprintf(file, "%s", id);
        }
        else
        {
            write_code(node->children[1], file);
        }
        break;
    case AST_PARAMETER_LIST:
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
            if (i < node->children_count - 1)
            {
                fprintf(file, ", ");
            }
        }
        break;
    case AST_DIRECT_DECLARATOR:
        write_code(node->children[0], file);
        break;
    case AST_DIRECT_DECLARATOR_END:
        write_code(node->children[0], file);
        fprintf(file, "(");
        if (node->children_count == 2)
        {
            write_code(node->children[1], file);
        }
        fprintf(file, ")");
        break;
    case AST_DECLARATOR:
        fprintf(file, "(");
        write_code(node->children[0], file);
        fprintf(file, ")");
        break;
    case AST_STAR_DECLARATOR:
        fprintf(file, "*");
        write_code(node->children[0], file);
        break;
    case AST_STRUCT_DECLARATION_LIST:
        for (int i = 0; i < node->children_count; i++)
        {
            depth++;
            tab_depth(file);
            write_code(node->children[i], file);
            fprintf(file, ";\n");
            depth--;
        }
        break;
    case AST_TYPE_SPECIFIER:


        if (node->children_count > 0)
        {
            write_code(node->children[0], file);
        }
        else
        {
            fprintf(file, "%s", node->id);
        }
        break;
    case AST_DECLARATION:
        

        if (node->children[0]->type == AST_TYPE_SPECIFIER && node->children[1]->type == AST_DIRECT_DECLARATOR_END 
             && node->children[1]->children[0]->type == AST_DIRECT_DECLARATOR && node->children[1]->children[0]->children[0]->type == AST_DECLARATOR 
                && node->children[1]->children[0]->children[0]->children[0]->type == AST_STAR_DECLARATOR)
        {
            fprintf(file, "void *");

            char *id = find_first_identifier(node->children[1]->children[0]->children[0]);

            fprintf(file, "%s", id);
        }
        else
        {
            write_code(node->children[0], file);
            if (node->children_count > 1)
            {
                fprintf(file, " ");
                write_code(node->children[1], file);
            }
        }
        break;
    case AST_ASSIGNMENT:
        write_code(node->children[0], file);
        fprintf(file, " = ");
        write_code(node->children[1], file);
        break;
    case AST_BOOL_LOGIC:
        if (node->id[0] == '&')
        {
            node->children[0]->false_label = node->false_label;
            node->children[1]->false_label = node->false_label;
            write_code(node->children[0], file);
            tab_depth(file);
            write_code(node->children[1], file);
        }
        else
        {
            node->children[0]->false_label = bool_counter;
            bool_counter++;
            node->children[0]->true_label = node->true_label;

            node->children[1]->false_label = node->false_label;
            node->children[1]->true_label = node->true_label;

            write_code(node->children[0], file);
            tab_depth(file);
            fprintf(file, "goto true_%d;\n", node->children[0]->true_label);
            fprintf(file, "false_%d:\n", node->children[0]->false_label);
            tab_depth(file);
            write_code(node->children[1], file);
            fprintf(file, "true_%d:\n", node->children[0]->true_label);
        }
        break;
    case AST_BOOL_OP:
        fprintf(file, "if (");
        write_code(node->children[0], file);
        char *inv = inv_op(node->id);
        fprintf(file, " %s ", inv);
        write_code(node->children[1], file);
        fprintf(file, ") goto false_%d;\n", node->false_label);
        break;
    case AST_OP:
        write_code(node->children[0], file);
        fprintf(file, " %s ", node->id);
        write_code(node->children[1], file);
        break;
    case AST_UNARY_OPERATOR:
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
        }
        break;
    case AST_UNARY_OP:
        fprintf(file, "%s", node->id);
        break;
    case AST_UNARY_SIZEOF:
        fprintf(file, "%d", node->size);
        break;
    case AST_UNARY:
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
        }
        break;
    case AST_ARGUMENT_EXPRESSION_LIST:
        for (int i = 0; i < node->children_count; i++)
        {
            write_code(node->children[i], file);
            if (i < node->children_count - 1)
            {
                fprintf(file, ", ");
            }
        }
        break;
    case AST_POSTFIX_POINTER:
        write_code(node->children[0], file);
        fprintf(file, "->");
        write_code(node->children[1], file);
        break;
    case AST_POSTFIX:
        write_code(node->children[0], file);
        fprintf(file, "(");
        if (node->children_count > 1)
        {
            write_code(node->children[1], file);
        }
        fprintf(file, ")");
        break;
    case AST_CONSTANT:
        fprintf(file, "%d", node->value);
        break;
    case AST_IDENTIFIER:
        fprintf(file, "%s", node->id);
        break;
    case AST_PRIMARY_EXPRESSION:
        fprintf(file, "(");
        write_code(node->children[0], file);
        fprintf(file, ")");
        break;
    case AST_STRUCT:
        if (node->children_count == 1)
        {
            fprintf(file, "void");
        }
        break;
    default:
        fprintf(file, "Unknown");
        break;
    }
}