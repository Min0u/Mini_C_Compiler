%code requires 
{
        #include "../../src/Semantic_Analysis/ast.h"
        #include "../../src/Semantic_Analysis/code.h"
        #include "../../src/Semantic_Analysis/symbol.h"
        #include "../../src/Semantic_Analysis/stack.h"
        #include "../../src/Semantic_Analysis/hashmap.h"
}

%code top 
{
        #include <stdio.h>
        #include <stdlib.h>
        #include "y.tab.h"

        #define SYNTAX_ERROR 1
}

%{

extern int yylineno;
extern FILE *yyin;
extern FILE *yyout;
int yylex();

char *current_function;
Stack *stack;

int yyerror(const char *s ) 
{
fprintf(stderr, "%s at line %d\n", s, yylineno);
exit(SYNTAX_ERROR);
}

%}

%define parse.error verbose

%union 
{
        Ast_node *node;
}

%token < node > IDENTIFIER
%token < node > CONSTANT
%token SIZEOF
%token LE_OP GE_OP EQ_OP NE_OP
%token PTR_OP
%token AND_OP OR_OP
%token EXTERN
%token < node > INT VOID
%token STRUCT
%token IF ELSE WHILE FOR RETURN

%type < node > primary_expression postfix_expression argument_expression_list direct_declarator_start struct_identifier unary_expression unary_operator multiplicative_expression additive_expression relational_expression equality_expression logical_and_expression logical_or_expression expression declaration declaration_specifiers type_specifier struct_specifier struct_declaration_list struct_declaration declarator direct_declarator parameter_list parameter_declaration statement compound_statement  declaration_list statement_list expression_statement selection_statement iteration_statement jump_statement program external_declaration function function_definition

%left '+' '-'
%left '*' '/'

%nonassoc ELSE_THEN
%nonassoc ELSE

%start main_program

%%

primary_expression
        : IDENTIFIER
        {      
                $$ = $1; 

                Symbol *sym = lookup_stack(stack, $1->id);
                if (!sym)
                {
                        printf("\033[1;31mError: \033[0mUnknown identifier \033[1m\"%s\"\033[0m (line %d)\n", $1->id, yylineno);
                        YYERROR;
                }

                $1->size = sym->size;
                $1->type_name = sym->type_name;
                $1->struct_name = sym->struct_name;
                $1->pointer = sym->pointer;
        }
        | CONSTANT
        {
                $$ = $1;

                $$->size = 4;
                $$->type_name = "int";
        }
        | '(' expression ')'
        {
                $$ = ast_create_node(AST_PRIMARY_EXPRESSION);
                ast_add_child($$, $2);

                $$->size = $2->size;
                $$->type_name = $2->type_name;
                $$->struct_name = $2->struct_name;
                $$->pointer = $2->pointer;
        }
        ;

postfix_expression
        : primary_expression
        {
                $$ = $1;
        }
        | postfix_expression '(' ')'
        {
                $$ = ast_create_node(AST_POSTFIX);
                ast_add_child($$, $1);

                char *id = find_first_identifier($1);

                Symbol *sym = lookup_stack(stack, id);

                if (sym->type != FUNCTION_SYMBOL)
                {
                        printf("\033[1;31mError:\033[0m \033[1m\"%s\"\033[0m is not a function (line %d)\n", id, yylineno);
                        YYERROR;
                }

                if ((sym->child_count - 1) != 0)
                {
                        printf("\033[1;31mError: \033[0mFunction \033[1m\"%s\"\033[0m requires %d arguments but none were given (line %d)\n", id, sym->child_count - 1, yylineno);
                        YYERROR;
                }

                $$->type_name = sym->children[0]->type_name;
                $$->size = sym->children[0]->size;
                $$->struct_name = sym->children[0]->struct_name;
                $$->pointer = sym->children[0]->pointer;
        }
        | postfix_expression '(' argument_expression_list ')'
        {
                $$ = ast_create_node(AST_POSTFIX);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                char *id = find_first_identifier($1);

                Symbol *sym = lookup_stack(stack, id);

                if (sym->type != FUNCTION_SYMBOL)
                {
                        printf("\033[1;31mError:\033[0m \033[1m\"%s\"\033[0m is not a function (line %d)\n", id, yylineno);
                        YYERROR;
                }

                if ($3->children_count != 0)
                {
                        if ((sym->child_count - 1) != $3->children_count)
                        {
                                printf("\033[1;31mError: \033[0mFunction \033[1m\"%s\"\033[0m requires %d arguments but %d were given (line %d)\n", id, sym->child_count - 1, $3->children_count, yylineno);
                                YYERROR;
                        }
                }

                $$->type_name = sym->children[0]->type_name;
                $$->size = sym->children[0]->size;
                $$->struct_name = sym->children[0]->struct_name;
                $$->pointer = sym->children[0]->pointer;

                for (int i = 0; i < $3->children_count; i++)
                {
                        Ast_node *argument = $3->children[i];

                        if (sym->children[i+1]->pointer)
                        {
                                continue;
                        }

                        if (strcmp(sym->children[i+1]->type_name, argument->type_name) != 0)
                        {
                                printf("\033[1;31mError: \033[0mExpected argument of type \033[1m\"%s\"\033[0m but got \033[1m\"%s\"\033[0m (line %d)\n", sym->children[i+1]->type_name, argument->type_name, yylineno);
                                YYERROR;
                        }

                        if (strcmp(argument->type_name, "struct") == 0)
                        {
                                if (strcmp(sym->children[i+1]->struct_name, argument->struct_name) != 0)
                                {
                                        printf("yes\n");
                                        printf("\033[1;31mError: \033[0mExpected argument of type \033[1m\"%s\"\033[0m but got \033[1m\"%s\"\033[0m (line %d)\n", sym->children[i+1]->struct_name, argument->struct_name, yylineno);
                                        YYERROR;
                                }
                        }
                }
        }
        | postfix_expression PTR_OP IDENTIFIER
        {
                $$ = ast_create_node(AST_POSTFIX_POINTER);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                char *id = find_first_identifier($1);

                Symbol *sym = lookup_stack(stack, id);

                if (strcmp(sym->type_name, "struct") != 0)
                {
                        printf("\033[1;31mError: \033[0mUnknown structure \033[1m\"%s\"\033[0m (line %d)\n", id, yylineno);
                        YYERROR;
                }

                Symbol *s = lookup_stack(stack, sym->struct_name);
                Symbol *s1 = lookup_symbol_child(s, $3->id);

                if (!s1)
                {
                        printf("\033[1;31mError: \033[0mUnknown structure member \033[1m\"%s\"\033[0m (line %d)\n", $3->id, yylineno);
                        YYERROR;
                }

                $$->offset = s1->offset;
                $$->type_name = s1->type_name;
                $$->size = s1->size;
                $$->struct_name = s1->struct_name;
                $$->pointer = s1->pointer;
        }
        ;

argument_expression_list
        : expression
        {
                $$ = ast_create_node(AST_ARGUMENT_EXPRESSION_LIST);
                ast_add_child($$, $1);
        }
        | argument_expression_list ',' expression
        {
                ast_add_child($1, $3);
                $$ = $1;
        }
        ;

unary_expression
        : postfix_expression
        {
                $$ = $1;
        }
        | unary_operator unary_expression
        {
                $$ = ast_create_node(AST_UNARY);
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                if (strcmp($1->id, "&") == 0)
                {
                        $$->pointer = true;
                }

                $$->size = $2->size;
                $$->type_name = $2->type_name;
        }
        | SIZEOF unary_expression
        {
                $$ = ast_create_node(AST_UNARY_SIZEOF);
                ast_add_child($$, $2);

                char *id = find_first_identifier($2);

                Symbol *sym = lookup_stack(stack, id);
                if (sym)
                {
                        $$->size = sym->size;
                }

                $$->type_name = "int";
        }
        | SIZEOF '(' type_specifier ')'
        {
                $$ = ast_create_node(AST_UNARY_SIZEOF);
                ast_add_child($$, $3);

                if ($3->type == AST_TYPE_SPECIFIER)
                {
                        if (strcmp($3->id, "int") == 0)
                        {
                                $$->size = 4;
                        }
                        else if (strcmp($3->id, "void") == 0)
                        {
                                $$->size = 0;
                        }
                        else
                        {
                                $$->size = $3->size;
                        }
                }

                $$->type_name = "int";
        }
        ;

unary_operator
        : '&'
        {
                $$ = ast_create_node(AST_UNARY_OP);

                $$->id = "&";
        }
        | '*'
        {
                $$ = ast_create_node(AST_UNARY_OP);

                $$->id = "*";
        }
        | '-'
        {
                $$ = ast_create_node(AST_UNARY_OP);

                $$->id = "-";
        }
        ;

multiplicative_expression
        : unary_expression
        {
                $$ = $1;
        }
        | multiplicative_expression '*' unary_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "*";
                $$->type_name = "int";
        }
        | multiplicative_expression '/' unary_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "/";
                $$->type_name = "int";
        }
        ;

additive_expression
        : multiplicative_expression
        {
                $$ = $1;
        }
        | additive_expression '+' multiplicative_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "+";
                $$->type_name = "int";
        }
        | additive_expression '-' multiplicative_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "-";
                $$->type_name = "int";
        }
        ;

relational_expression
        : additive_expression
        {
                $$ = $1;
        }
        | relational_expression '<' additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "<";
                $$->type_name = "int";

        }
        | relational_expression '>' additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = ">";
                $$->type_name = "int";
        }
        | relational_expression LE_OP additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "<=";
                $$->type_name = "int";
        }
        | relational_expression GE_OP additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = ">=";
                $$->type_name = "int";
        }
        ;

equality_expression
        : relational_expression
        {
                $$ = $1;
        }
        | equality_expression EQ_OP relational_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "==";
                $$->type_name = "int";
        }
        | equality_expression NE_OP relational_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "!=";
                $$->type_name = "int";
        }
        ;

logical_and_expression
        : equality_expression
        {
                $$ = $1;
        }
        | logical_and_expression AND_OP equality_expression
        {
                $$ = ast_create_node(AST_BOOL_LOGIC);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "&&";
                $$->type_name = "int";
        }
        ;

logical_or_expression
        : logical_and_expression
        {
                $$ = $1;
        }
        | logical_or_expression OR_OP logical_and_expression
        {
                $$ = ast_create_node(AST_BOOL_LOGIC);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "||";
                $$->type_name = "int";
        }
        ;

expression
        : logical_or_expression
        {
                $$ = $1;
        }
        | unary_expression '=' expression
        {
                $$ = ast_create_node(AST_ASSIGNMENT);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                if (!($1->pointer && $3->pointer))
                {
                        if (strcmp($1->type_name, $3->type_name) != 0)
                        {
                                printf("\033[1;31mError: \033[0mCannot assign \033[1m\"%s\"\033[0m to \033[1m\"%s\"\033[0m (line %d)\n", $3->type_name, $1->type_name, yylineno);
                                YYERROR;
                        }
                }

                $$->size = $1->size;
                $$->type_name = $1->type_name;
                $$->struct_name = $1->struct_name;
                $$->pointer = $1->pointer;
                $$->offset = $1->offset;
        }
        ;

declaration
        : declaration_specifiers declarator ';'
        {
                $$ = ast_create_node(AST_DECLARATION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                if ($1->type == AST_TYPE_SPECIFIER && $2->type == AST_DIRECT_DECLARATOR_END 
                        && $2->children[0]->type == AST_DIRECT_DECLARATOR && $2->children[0]->children[0]->type == AST_DECLARATOR 
                                && $2->children[0]->children[0]->children[0]->type == AST_STAR_DECLARATOR)
                {
                                pop(stack);

                                char *id1 = find_first_identifier($2->children[0]->children[0]);

                                Symbol *symbol = create_symbol(id1, -1, FUNCTION_SYMBOL);
                                push_symbol(stack, id1, symbol);

                                add_return_symbol($$, symbol);

                                function_arguments($2, symbol);

                                symbol->pointer = true;
                                symbol->type_name = "function";
                }
                else if ($1->type == AST_TYPE_SPECIFIER && ($2->type == AST_IDENTIFIER || $2->type == AST_STAR_DECLARATOR))
                {
                        char *id = find_first_identifier($2);

                        if (id != NULL)
                        {
                                Symbol *sym = lookup_stack_top(stack, id);
                                if (sym)
                                {
                                        print_error(sym, id, yylineno);
                                        YYERROR;
                                }

                                Symbol *sym2 = lookup_stack(stack, id);
                                if (sym2)
                                {
                                        print_warning(sym2, id, yylineno);
                                }
                                
                                Symbol *symbol = create_symbol(id, $1->size, IDENTIFIER_SYMBOL);
                                symbol->type_name = $1->id;

                                if (strcmp($1->id, "struct") == 0)
                                {
                                        char *id1 = find_first_identifier($1);
                                        symbol->struct_name = id1;
                                }

                                if ($2->type == AST_STAR_DECLARATOR)
                                {
                                        symbol->pointer = true;
                                }

                                push_symbol(stack, id, symbol);
                                
                                $$->size = $1->size;
                        }
                } 
                else if ($1->type == AST_EXT_DECLARATION || $2->type == AST_DIRECT_DECLARATOR_END)
                {
                        char *id = find_first_identifier($2);

                        if (id != NULL)
                        {
                                Symbol *sym = lookup_stack(stack, id);

                                if (sym)
                                {
                                        print_error(sym, id, yylineno);
                                        YYERROR;
                                }

                                pop(stack);

                                Symbol *symbol = create_symbol(id, -1, FUNCTION_SYMBOL);
                                push_symbol(stack, id, symbol);

                                add_return_symbol($$, symbol);

                                Ast_node *declarator = $2;

                                if (declarator->type == AST_STAR_DECLARATOR)
                                {
                                        declarator = declarator->children[0];
                                }

                                function_arguments(declarator, symbol);

                                symbol->type_name = "function";
                        }

                }
        }
        | struct_specifier ';'
        {
                $$ = ast_create_node(AST_DECLARATION);
                ast_add_child($$, $1);
        }
        ;

declaration_specifiers
        : EXTERN type_specifier
        {
                $$ = ast_create_node(AST_EXT_DECLARATION);
                ast_add_child($$, $2);
        }
        | type_specifier
        {
                $$ = $1;
        }
        ;

type_specifier
        : VOID
        {
                $$ = ast_create_node(AST_TYPE_SPECIFIER);

                $$->id = "void";
                $$->size = 0;
        }
        | INT
        {
                $$ = ast_create_node(AST_TYPE_SPECIFIER);

                $$->id = "int";
                $$->size = 4;
        }
        | struct_specifier
        {
                $$ = ast_create_node(AST_TYPE_SPECIFIER);
                ast_add_child($$, $1);

                $$->id = "struct";
                $$->size = $1->size;
        }
        ;

struct_identifier
        : STRUCT IDENTIFIER
        {
                $$ = $2;

                char *id = find_first_identifier($2);

                if (id != NULL)
                {
                        Symbol *s = lookup_stack(stack, id);
                        if (s)
                        {
                                print_error(s, id, yylineno);
                                YYERROR;
                        }

                        Symbol *symbol = create_symbol(id, 0, STRUCT_SYMBOL);
                        push_symbol(stack, id, symbol);
                }
        }
        ;

struct_specifier
        : struct_identifier open_brace struct_declaration_list close_brace
        {
                $$ = ast_create_node(AST_STRUCT);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                char *id = find_first_identifier($1);

                Symbol *s = lookup_stack(stack, id);

                int offset = 0;

                $$->size = $3->size;

                s->size = $3->size;

                for (int i = 0; i < $3->children_count; i++)
                {
                        Ast_node *declaration = $3->children[i];
                        Ast_node *declarator = declaration->children[1];

                        if (declaration->size == 0)
                        {
                                declaration->size = s->size;
                        }
                        
                        declarator->offset = offset;
                        offset += declaration->size;

                        char *id1 = find_first_identifier(declarator);

                        Symbol *s1 = create_symbol(id1, declaration->size, IDENTIFIER_SYMBOL);

                        s1->struct_name = id;

                        if(declaration->children[0]->type == AST_TYPE_SPECIFIER)
                        {
                                s1->type_name = declaration->children[0]->id;
                        }

                        if (declaration->children[1]->type == AST_STAR_DECLARATOR)
                        {
                                s1->pointer = true;
                        }

                        s1->offset = declarator->offset;

                        add_symbol_child(s, s1);
                }
        }
        | STRUCT open_brace struct_declaration_list close_brace
        {
                $$ = ast_create_node(AST_STRUCT);
                ast_add_child($$, $3);

                $$->size = $3->size;
        }
        | STRUCT IDENTIFIER
        {
                $$ = ast_create_node(AST_STRUCT);
                ast_add_child($$, $2);

                Symbol *s = lookup_stack(stack, $2->id);

                if (!s)
                {
                        printf("\033[1;31mError: \033[0mUnknown structure \033[1m\"%s\"\033[0m (line %d)\n", $2->id, yylineno);
                        YYERROR;
                }

                $$->size = s->size;  

                if (s->type != STRUCT_SYMBOL)
                {
                        printf("\033[1;31mError: \033[0mIdentifier \033[1m\"%s\"\033[0m is not a structure (line %d)\n", $2->id, yylineno);
                        YYERROR;
                }      
        }
        ;

struct_declaration_list
        : struct_declaration
        {
                $$ = ast_create_node(AST_STRUCT_DECLARATION_LIST);
                ast_add_child($$, $1); 

                $$->size = $1->size;
        }
        | struct_declaration_list struct_declaration
        {
                ast_add_child($1, $2);
                $$ = $1;

                $$->size = $1->size + $2->size;
        }
        ;

struct_declaration
        : type_specifier declarator ';'
        {
                $$ = ast_create_node(AST_DECLARATION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                $$->size = $1->size;
        }
        ;

declarator
        : '*' direct_declarator
        {
                $$ = ast_create_node(AST_STAR_DECLARATOR);
                ast_add_child($$, $2);
        }
        | direct_declarator
        {
                $$ = $1;
        }
        ;

direct_declarator_start
        : direct_declarator '('
        {
                $$ = ast_create_node(AST_DIRECT_DECLARATOR);
                ast_add_child($$, $1);
        
                Hash_map *hashmap = create_hash_map();
                push(stack, hashmap);
        }

direct_declarator
        : IDENTIFIER
        {
                $$ = $1;
        }
        | '(' declarator ')'
        {
                $$ = ast_create_node(AST_DECLARATOR);
                ast_add_child($$, $2);
        }
        | direct_declarator_start parameter_list ')'
        {
                $$ = ast_create_node(AST_DIRECT_DECLARATOR_END);
                ast_add_child($$, $1);
                ast_add_child($$, $2);
        }
        | direct_declarator_start ')'
        {
                $$ = ast_create_node(AST_DIRECT_DECLARATOR_END);
                ast_add_child($$, $1);
        }
        ;

parameter_list
        : parameter_declaration
        {
                $$ = ast_create_node(AST_PARAMETER_LIST);
                ast_add_child($$, $1);
        }
        | parameter_list ',' parameter_declaration
        {
                ast_add_child($1, $3);
                $$ = $1;
        }
        ;

parameter_declaration
        : declaration_specifiers declarator
        {
                $$ = ast_create_node(AST_PARAMETER_DECLARATION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                char *id = find_first_identifier($2);

                if (id != NULL)
                {
                        Symbol *s1 = lookup_stack_top(stack, id);
                        if (s1)
                        {
                                print_error(s1, id, yylineno);
                                YYERROR;
                        }

                        Symbol *s2 = lookup_stack(stack, id);
                        if (s2)
                        {
                                print_warning(s2, id, yylineno);
                        }

                        if ($1->type == AST_TYPE_SPECIFIER && $2->type == AST_STAR_DECLARATOR && $2->children[0]->type == AST_DIRECT_DECLARATOR_END)
                        {
                                pop(stack);

                                char *id1 = find_first_identifier($2->children[0]->children[0]);

                                Symbol *symbol = create_symbol(id1, -1, FUNCTION_SYMBOL);
                                push_symbol(stack, id1, symbol);

                                add_return_symbol($$, symbol);

                                function_arguments($2->children[0], symbol);

                                symbol->type_name = "function";
                                symbol->pointer = true;
                        }
                        else
                        {
                                Symbol *symbol = create_symbol(id, 4, IDENTIFIER_SYMBOL);

                                symbol->type_name = $1->id;

                                if (strcmp($1->id, "struct") == 0)
                                {
                                        char *id1 = find_first_identifier($1);
                                        symbol->struct_name = id1;
                                }

                                if ($2->type == AST_STAR_DECLARATOR)
                                {
                                        symbol->pointer = true;
                                }

                                push_symbol(stack, id, symbol);
                        }
                                
                }
        }
        ;

statement
        : compound_statement
        {
                $$ = $1;
        }
        | expression_statement
        {
                $$ = $1;
        }
        | selection_statement
        {
                $$ = $1;
        }
        | iteration_statement
        {
                $$ = $1;
        }
        | jump_statement
        {
                $$ = $1;
        }
        ;

open_brace
        : '{'
        {
                Hash_map *hashmap = create_hash_map();
                push(stack, hashmap);
        }
        ;

close_brace
        : '}'
        {
                pop(stack);
        }
        ;

compound_statement
        : open_brace close_brace
        {
                $$ = ast_create_node(AST_COMPOUND_STATEMENT);
        }
        | open_brace statement_list close_brace
        {
                $$ = ast_create_node(AST_COMPOUND_STATEMENT);
                ast_add_child($$, $2);
        }
        | open_brace declaration_list close_brace
        {
                $$ = ast_create_node(AST_COMPOUND_STATEMENT);
                ast_add_child($$, $2);
        }
        | open_brace declaration_list statement_list close_brace
        {
                $$ = ast_create_node(AST_COMPOUND_STATEMENT);
                ast_add_child($$, $2);
                ast_add_child($$, $3);
        }
        ;

declaration_list
        : declaration
        {
                $$ = ast_create_node(AST_DECLARATION_LIST);
                ast_add_child($$, $1);
        }
        | declaration_list declaration
        {
                ast_add_child($1, $2);
                $$ = $1;
        }
        ;

statement_list
        : statement
        {
                $$ = ast_create_node(AST_STATEMENT_LIST);
                ast_add_child($$, $1);
        }
        | statement_list statement
        {
                ast_add_child($1, $2);
                $$ = $1;
        }
        ;

expression_statement
        : ';'
        {
                $$ = ast_create_node(AST_EXPRESSION_STATEMENT);
        }
        | expression ';'
        {
                $$ = ast_create_node(AST_EXPRESSION_STATEMENT);
                ast_add_child($$, $1);
        }
        ;

selection_statement
        : IF '(' expression ')' statement %prec ELSE_THEN
        {
                $$ = ast_create_node(AST_IF);
                ast_add_child($$, $3);
                ast_add_child($$, $5);
        }
        | IF '(' expression ')' statement ELSE statement
        {
                $$ = ast_create_node(AST_IF_ELSE);
                ast_add_child($$, $3);
                ast_add_child($$, $5);
                ast_add_child($$, $7);
        }
        ;

iteration_statement
        : WHILE '(' expression ')' statement
        {
                $$ = ast_create_node(AST_WHILE);
                ast_add_child($$, $3);
                ast_add_child($$, $5);
        }
        | FOR '(' expression_statement expression_statement expression ')' statement
        {
                $$ = ast_create_node(AST_FOR);
                ast_add_child($$, $3);
                ast_add_child($$, $4);
                ast_add_child($$, $5);
                ast_add_child($$, $7);
        }
        ;

jump_statement
        : RETURN ';'
        {
                $$ = ast_create_node(AST_RETURN);

                Symbol *s = lookup_stack(stack, current_function);

                if (strcmp(s->children[0]->type_name, "void") != 0)
                {
                        printf("\033[1;31mError: \033[0mFunction \033[1m\"%s\"\033[0m expects a return value (line %d)\n", current_function, yylineno);
                        YYERROR;
                }
        }
        | RETURN expression ';'
        {
                $$ = ast_create_node(AST_RETURN);
                ast_add_child($$, $2);

                Symbol *s = lookup_stack(stack, current_function);

                if (!(s->children[0]->pointer && $2->pointer))
                {
                        if (strcmp(s->children[0]->type_name, $2->type_name) != 0)
                        {
                                printf("\033[1;31mError: \033[0mFunction \033[1m\"%s\"\033[0m expects a return value of type \033[1m\"%s\"\033[0m but got \033[1m\"%s\"\033[0m (line %d)\n", current_function, s->children[0]->type_name, $2->type_name, yylineno);
                                YYERROR;
                        }

                        if (strcmp(s->children[0]->type_name, "struct") == 0)
                        {
                                if (strcmp(s->children[0]->struct_name, $2->struct_name) != 0)
                                {
                                        printf("\033[1;31mError: \033[0mFunction \033[1m\"%s\"\033[0m expects a return value of type \033[1m\"%s\"\033[0m but got \033[1m\"%s\"\033[0m (line %d)\n", current_function, s->children[0]->struct_name, $2->struct_name, yylineno);
                                        YYERROR;
                                }
                        }
                }
        }
        ;

main_program
        : program
        {
                tac_transformation($1);
//                print_complete_ast($1);
                write_code($1, yyout);
                free_ast($1);
        }
        ;

program
        : external_declaration
        {
                $$ = ast_create_node(AST_PROGRAM);
                ast_add_child($$, $1);
        }
        | program external_declaration
        {
                ast_add_child($1, $2);
                $$ = $1;
        }
        ;
        
external_declaration
        : function_definition
        {
                $$ = $1;
        }
        | declaration
        {
                $$ = $1;
        }
        ;

function
        : declaration_specifiers declarator
        {
                $$ = ast_create_node(AST_FUNCTION_DEFINITION);  
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                char *id = find_first_identifier($2);

                if (id != NULL)
                {
                        Symbol *s = lookup_stack_top(stack, id);
                        if (s)
                        {
                                print_error(s, id, yylineno);
                                YYERROR;
                        }

                        Symbol *symbol = create_symbol(id, -1, FUNCTION_SYMBOL);
                        push_symbol_next(stack, id, symbol);

                        add_return_symbol($$, symbol);
                                
                        Ast_node *declarator = $2;

                        if (declarator->type == AST_STAR_DECLARATOR)
                        {
                                declarator = declarator->children[0];
                        }

                        function_arguments(declarator, symbol);

                        symbol->type_name = "function";

                        current_function = id;
                }

        }

function_definition
        : function compound_statement
        {
                ast_add_child($1, $2);
                $$ = $1;

                pop(stack);
        }
        ;

%%

int main(int argc, char **argv)
{
        stack = create_stack();
        Hash_map *hashmap = create_hash_map();

        push(stack, hashmap);

        if (argc < 2)
        {
                fprintf(stderr, "\033[1;36mUsage: \033[0;36m%s <input_file> <output_file>\033[0m\n", argv[0]);
                return 1;
        }

        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
                fprintf(stderr, "\033[0;31mCannot open file %s\n\033[0m", argv[1]);
                return 1;
        }

        yyout = fopen(argv[2], "w");
        if (yyout == NULL)
        {
                fprintf(stderr, "\033[0;31mCannot open file %s\n\033[0m", argv[2]);
                return 1;
        }

        printf("\033[0;34mStarting compilation...\033[0m\n");

        yyparse();

        printf("\033[0;34mCompilation finished :)\033[0m\n");

        fclose(yyin);
        fclose(yyout);
}