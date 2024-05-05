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

        #define SEMANTIC_ERROR 2
        #define SYNTAX_ERROR 3
}

%{

extern int yylineno;
extern FILE *yyin;
extern FILE *yyout;
int yylex();

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

%type < node > primary_expression postfix_expression argument_expression_list unary_expression unary_operator multiplicative_expression additive_expression relational_expression equality_expression logical_and_expression logical_or_expression expression declaration declaration_specifiers type_specifier struct_specifier struct_declaration_list struct_declaration declarator direct_declarator parameter_list parameter_declaration statement compound_statement  declaration_list statement_list expression_statement selection_statement iteration_statement jump_statement program external_declaration function_definition

%left '+' '-'
%left '*' '/'

%nonassoc ELSE_PRIORITY
%nonassoc ELSE

%start main_program

%%

primary_expression
        : IDENTIFIER
        {
                $$ = $1;
        }
        | CONSTANT
        {
                $$ = $1;
        }
        | '(' expression ')'
        {
                $$ = ast_create_node(AST_PRIMARY_EXPRESSION);
                ast_add_child($$, $2);
        }
        ;

postfix_expression
        : primary_expression
        {
                $$ = $1;
        }
        | postfix_expression '(' ')'
        {
                $$ = ast_create_node(AST_POSTFIX_NO_ARGUMENT);
                ast_add_child($$, $1);
        }
        | postfix_expression '(' argument_expression_list ')'
        {
                $$ = ast_create_node(AST_POSTFIX_ARGUMENT);
                ast_add_child($$, $1);
                ast_add_child($$, $3);
        }
        | postfix_expression '.' IDENTIFIER
        {
                $$ = ast_create_node(AST_POSTFIX_IDENTIFIER);
                ast_add_child($$, $1);
                ast_add_child($$, $3);
        }
        | postfix_expression PTR_OP IDENTIFIER
        {
                $$ = ast_create_node(AST_POSTFIX_POINTER);
                ast_add_child($$, $1);
                ast_add_child($$, $3);
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
        }
        | SIZEOF unary_expression
        {
                $$ = ast_create_node(AST_UNARY_SIZEOF);
                ast_add_child($$, $2);
        }
        | SIZEOF '(' type_specifier ')'
        {
                $$ = ast_create_node(AST_UNARY_SIZEOF);
                ast_add_child($$, $3);
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
        }
        | multiplicative_expression '/' unary_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "/";
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
        }
        | additive_expression '-' multiplicative_expression
        {
                $$ = ast_create_node(AST_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "-";
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

        }
        | relational_expression '>' additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = ">";
        }
        | relational_expression LE_OP additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "<=";
        }
        | relational_expression GE_OP additive_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = ">=";
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
        }
        | equality_expression NE_OP relational_expression
        {
                $$ = ast_create_node(AST_BOOL_OP);
                ast_add_child($$, $1);
                ast_add_child($$, $3);

                $$->id = "!=";
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
        }
        ;

declaration
        : declaration_specifiers declarator ';'
        {
                $$ = ast_create_node(AST_DECLARATION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);

                if ($1->type == AST_TYPE_SPECIFIER && ($2->type == AST_IDENTIFIER || $2->type == AST_STAR_DECLARATOR))
                {
                        char *id = find_first_identifier($2);

                        if (id != NULL)
                        {
                                if (lookup_stack_top(stack, id))
                                {
                                        printf("\033[1;31mError: \033[0mVariable \033[1m\"%s\" \033[0malready declared (line %d)\033[0m\n", id, yylineno);
                                        YYERROR;
                                }

                                if (lookup_stack(stack, id))
                                {
                                        printf("\033[1;35mWarning: \033[0mOverwriting variable \033[1m\"%s\" \033[0m(line %d)\033[0m\n", id, yylineno);
                                }
                                
                                Symbol *symbol = create_symbol(id, 4, IDENTIFIER_SYMBOL);
                                push_symbol(stack, id, symbol);
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
        }
        | INT
        {
                $$ = ast_create_node(AST_TYPE_SPECIFIER);

                $$->id = "int";
        }
        | struct_specifier
        {
                $$ = ast_create_node(AST_TYPE_SPECIFIER);
                ast_add_child($$, $1);

                $$->id = "struct";
        }
        ;

struct_specifier
        : STRUCT IDENTIFIER open_brace struct_declaration_list close_brace
        {
                $$ = ast_create_node(AST_STRUCT_VARIABLE_SPECIFIER);
                ast_add_child($$, $2);
                ast_add_child($$, $4);
        }
        | STRUCT open_brace struct_declaration_list close_brace
        {
                $$ = ast_create_node(AST_STRUCT_SPECIFIER);
                ast_add_child($$, $3);
        }
        | STRUCT IDENTIFIER
        {
                $$ = $2;
        }
        ;

struct_declaration_list
        : struct_declaration
        {
                $$ = ast_create_node(AST_STRUCT_DECLARATION_LIST);
                ast_add_child($$, $1); 
        }
        | struct_declaration_list struct_declaration
        {
                ast_add_child($1, $2);
                $$ = $1;
        }
        ;

struct_declaration
        : type_specifier declarator ';'
        {
                $$ = ast_create_node(AST_STRUCT_DECLARATION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);
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
        | direct_declarator '(' parameter_list ')'
        {
                $$ = ast_create_node(AST_DIRECT_DECLARATOR);
                ast_add_child($$, $1);
                ast_add_child($$, $3);
        }
        | direct_declarator '(' ')'
        {
                $$ = ast_create_node(AST_DIRECT_DECLARATOR);
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
        : IF '(' expression ')' statement %prec ELSE_PRIORITY
        {
                $$ = ast_create_node(AST_IF);
                ast_add_child($$, $3);
                ast_add_child($$, $5);
        }
        | IF '(' expression ')' statement ELSE statement %prec ELSE
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
        }
        | RETURN expression ';'
        {
                $$ = ast_create_node(AST_RETURN);
                ast_add_child($$, $2);
        }
        ;

main_program
        : program
        {
                tac_transformation($1);
                // print_complete_ast($1);
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

function_definition
        : declaration_specifiers declarator compound_statement
        {
                $$ = ast_create_node(AST_FUNCTION_DEFINITION);
                ast_add_child($$, $1);
                ast_add_child($$, $2);
                ast_add_child($$, $3);
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

        printf("\033[0;34mParsing started...\033[0m\n");

        yyparse();


        printf("\033[0;34mParsing done :)\033[0m\n");

        fclose(yyin);
        fclose(yyout);
}