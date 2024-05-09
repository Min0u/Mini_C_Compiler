%option noyywrap nounput noinput batch
%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

extern int yylineno;

%}

id [a-zA-Z]+[_0-9a-zA-Z]*
const 0|[1-9][0-9]*
comment \/\*([^*]|\*[^/])*\*\/

%%

{comment} { /* ignore les commentaires */ }

{const} { yylval.node = create_int_leaf(atoi(yytext)); return CONSTANT; }

extern { return EXTERN; }
int	{ return INT; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }
for { return FOR; }
return { return RETURN; }
void { return VOID; }
struct { return STRUCT; }
sizeof { return SIZEOF; }

\&\& { return AND_OP; }
\=\= { return EQ_OP; }
\>\= { return GE_OP; }
\<\= { return LE_OP; }
\!\= { return NE_OP; }
\|\| { return OR_OP; }
\-\> { return PTR_OP; }

\> { return '>';}
\< { return '<';}
\+ { return '+'; }
\- { return '-'; }
\* { return '*'; }
\/ { return '/'; }
\& { return '&'; }
\= { return '='; }
\( { return '('; }
\) { return ')'; }
\{ { return '{'; }
\} { return '}'; }
\; { return ';'; }
\, { return ','; }

{id} { yylval.node = create_id_leaf(strdup(yytext)); return IDENTIFIER; }

[\t \v \f ]+ { /* ignore spaces */ }
\n { yylineno++; }

. { printf("Error: unexpected character '%c'\n", yytext[0]); exit(1); }

%%