%code requires 
{
        #include "../../Semantic_Analysis/ast.h"
}

%code top 
{
        #include <stdio.h>
        #include <stdlib.h>
        #include "y.tab.h"

        #define SEMANTIC_ERROR 1
        #define SYNTAX_ERROR 2
}

%{

extern int yylineno;
int yylex();

int yyerror(const char *s ) 
{
fprintf(stderr, "%s at line %d\n", s, yylineno);
exit(SYNTAX_ERROR);
}

%}

%define parse.error verbose

%union 
{
        ast_node *node;
}

%token IDENTIFIER
%token CONSTANT
%token SIZEOF
%token PTR_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP
%token EXTERN
%token INT VOID
%token STRUCT
%token IF ELSE WHILE FOR RETURN
%token '(' ')' '{' '}' ';' ',' '.' '-' '+' '*' '/' '&' '=' '<' '>'

%type < node > primary_expression postfix_expression argument_expression_list unary_expression unary_operator multiplicative_expression additive_expression relational_expression equality_expression logical_and_expression logical_or_expression expression declaration declaration_specifiers type_specifier struct_specifier struct_declaration_list struct_declaration declarator direct_declarator parameter_list parameter_declaration statement compound_statement  declaration_list statement_list expression_statement selection_statement iteration_statement jump_statement program external_declaration function_definition

%left '+' '-'
%left '*' '/'

%nonassoc ELSE_PRIORITY
%nonassoc ELSE

%start program

%%

primary_expression
        : IDENTIFIER
        | CONSTANT
        | '(' expression ')'
        ;

postfix_expression
        : primary_expression
        | postfix_expression '(' ')'
        | postfix_expression '(' argument_expression_list ')'
        | postfix_expression '.' IDENTIFIER
        | postfix_expression PTR_OP IDENTIFIER
        ;

argument_expression_list
        : expression
        | argument_expression_list ',' expression
        ;

unary_expression
        : postfix_expression
        | unary_operator unary_expression
        | SIZEOF unary_expression
        ;

unary_operator
        : '&'
        | '*'
        | '-'
        ;

multiplicative_expression
        : unary_expression
        | multiplicative_expression '*' unary_expression
        | multiplicative_expression '/' unary_expression
        ;

additive_expression
        : multiplicative_expression
        | additive_expression '+' multiplicative_expression
        | additive_expression '-' multiplicative_expression
        ;

relational_expression
        : additive_expression
        | relational_expression '<' additive_expression
        | relational_expression '>' additive_expression
        | relational_expression LE_OP additive_expression
        | relational_expression GE_OP additive_expression
        ;

equality_expression
        : relational_expression
        | equality_expression EQ_OP relational_expression
        | equality_expression NE_OP relational_expression
        ;

logical_and_expression
        : equality_expression
        | logical_and_expression AND_OP equality_expression
        ;

logical_or_expression
        : logical_and_expression
        | logical_or_expression OR_OP logical_and_expression
        ;

expression
        : logical_or_expression
        | unary_expression '=' expression
        ;

declaration
        : declaration_specifiers declarator ';'
        | struct_specifier ';'
        ;

declaration_specifiers
        : EXTERN type_specifier
        | type_specifier
        ;

type_specifier
        : VOID
        | INT
        | struct_specifier
        ;

struct_specifier
        : STRUCT IDENTIFIER '{' struct_declaration_list '}'
        | STRUCT '{' struct_declaration_list '}'
        | STRUCT IDENTIFIER
        ;

struct_declaration_list
        : struct_declaration
        | struct_declaration_list struct_declaration
        ;

struct_declaration
        : type_specifier declarator ';'
        ;

declarator
        : '*' direct_declarator
        | direct_declarator
        ;

direct_declarator
        : IDENTIFIER
        | '(' declarator ')'
        | direct_declarator '(' parameter_list ')'
        | direct_declarator '(' ')'
        ;

parameter_list
        : parameter_declaration
        | parameter_list ',' parameter_declaration
        ;

parameter_declaration
        : declaration_specifiers declarator
        ;

statement
        : compound_statement
        | expression_statement
        | selection_statement
        | iteration_statement
        | jump_statement
        ;

compound_statement
        : '{' '}'
        | '{' statement_list '}'
        | '{' declaration_list '}'
        | '{' declaration_list statement_list '}'
        ;

declaration_list
        : declaration
        | declaration_list declaration
        ;

statement_list
        : statement
        | statement_list statement
        ;

expression_statement
        : ';'
        | expression ';'
        ;

selection_statement
        : IF '(' expression ')' statement %prec ELSE_PRIORITY
        | IF '(' expression ')' statement ELSE statement %prec ELSE
        ;

iteration_statement
        : WHILE '(' expression ')' statement
        | FOR '(' expression_statement expression_statement expression ')' statement
        ;

jump_statement
        : RETURN ';'
        | RETURN expression ';'
        ;

program
        : external_declaration
        | program external_declaration
        ;

external_declaration
        : function_definition
        | declaration
        ;

function_definition
        : declaration_specifiers declarator compound_statement
        ;

%%

int main()
{
        int c = yyparse();

        while (c)
        {
                c = yyparse();
        }

        exit(get_error_code());
}