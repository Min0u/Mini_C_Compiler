%option noyywrap nounput noinput batch
%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

%}

id [_a-zA-Z]+[_0-9a-zA-Z]*
const 0|[1-9][0-9]*
comment \/\*([^*]|\*[^/])*\*\/

%%

{comment} { /* ignore les commentaires */ }

{const} { return CONSTANT; }

extern { return EXTERN; }
int { return INT; }
void { return VOID; }
goto { return GOTO; }
if { return IF; }
return { return RETURN; }

\=\= { return EQ_OP; }
\>\= { return GE_OP; }
\<\= { return LE_OP; }
\!\= { return NE_OP; }

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
\: { return ':'; }
\, { return ','; }

{id} { return IDENTIFIER; }

[\t \v \f ]+ { /* ignore les espaces, tabulations, sauts de ligne, etc. */ }
\n {}

. { printf("Error: unexpected character '%c'\n", yytext[0]); exit(1); }

<<EOF>> { exit(0);}

%%