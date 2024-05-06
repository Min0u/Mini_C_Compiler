#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "symbol.h"

void print_error(Symbol *symbol, char *id, int line);
void print_warning(Symbol *symbol, char *id, int line);
void print_color(char *color, char *text);

void tab_depth(FILE *file);

// Inveter operations
char *inv_op(char *op);

void write_code(Ast_node *node, FILE *file);

#endif