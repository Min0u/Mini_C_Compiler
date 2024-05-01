#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

void write_code(ast_node *node, FILE *file);

void tab_depth(FILE *file);

#endif