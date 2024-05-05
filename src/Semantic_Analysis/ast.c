#include <stdio.h>
#include "ast.h"

int var_count = 0;

ast_node *ast_create_node(ast_type type)
{
    ast_node *node = (ast_node *)malloc(sizeof(ast_node));
    node->type = type;
    node->parent = NULL;
    node->childrens = NULL;
    node->childrens_count = 0;
    node->value = -1;
    node->id = NULL;
    node->false_label = -1;
    node->true_label = -1;
    node->available = true;
    node->sethi_ullman = 0;

    return node;
}

ast_node *create_int_leaf(int value)
{
    ast_node *node = ast_create_node(AST_CONSTANT);
    node->value = value;
    return node;
}

ast_node *create_id_leaf(char *name)
{
    ast_node *node = ast_create_node(AST_IDENTIFIER);
    node->id = name;
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////

void ast_add_child(ast_node *parent, ast_node *child)
{
    child->parent = parent;

    if (parent->childrens == NULL)
    {
        parent->childrens = (ast_node **)malloc(sizeof(ast_node *));
    }
    else
    {
        parent->childrens = (ast_node **)realloc(parent->childrens, sizeof(ast_node *) * (parent->childrens_count + 1));
    }

    parent->childrens[parent->childrens_count] = child;
    parent->childrens_count++;
}

void ast_add_child_front(ast_node *parent, ast_node *child)
{
    child->parent = parent;

    if (parent->childrens == NULL)
    {
        parent->childrens = (ast_node **)malloc(sizeof(ast_node *));
    }
    else
    {
        parent->childrens = (ast_node **)realloc(parent->childrens, sizeof(ast_node *) * (parent->childrens_count + 1));
    }

    // Shift all the elements to the right
    for (int i = parent->childrens_count; i > 0; i--)
    {
        parent->childrens[i] = parent->childrens[i - 1];
    }

    parent->childrens[0] = child;
    parent->childrens_count++;
}

// Add a child before a specific child (before is the specific child)
void ast_add_child_before(ast_node *parent, ast_node *child, ast_node *before)
{
    child->parent = parent;

    if (parent->childrens == NULL)
    {
        parent->childrens = (ast_node **)malloc(sizeof(ast_node *));
    }
    else
    {
        parent->childrens = (ast_node **)realloc(parent->childrens, sizeof(ast_node *) * (parent->childrens_count + 1));
    }

    // Find the index of before
    int i = 0;
    for (i = 0; i < parent->childrens_count; i++)
    {
        if (parent->childrens[i] == before)
        {
            break;
        }
    }

    // Shift all the elements to the right
    for (int j = parent->childrens_count; j > i; j--)
    {
        parent->childrens[j] = parent->childrens[j - 1];
    }

    parent->childrens[i] = child;
    parent->childrens_count++;
}

ast_node *ast_add_temporary(ast_node *node)
{
    ast_node *list = find_declaration_list(node);

    ast_node *declaration = ast_create_node(AST_DECLARATION);

    ast_node *specifier = ast_create_node(AST_TYPE_SPECIFIER);
    specifier->id = "int";

    char *name;
    asprintf(&name, "_temp_%d", var_count++);
    // identifier node for the declaration list
    ast_node *identifier = create_id_leaf(strdup(name));
    identifier->available = false;

    ast_add_child(declaration, specifier);
    ast_add_child(declaration, identifier);

    ast_add_child(list, declaration);

    return identifier;
}

////////////////////////////////////////////////////////////////////////////////////////

ast_node *find_declaration_list(ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Find a compound statement node
    ast_node *parent = find_parent(node, AST_COMPOUND_STATEMENT);

    for (int i = 0; i < parent->childrens_count; i++)
    {
        // If the node is a declaration list
        if (parent->childrens[i]->type == AST_DECLARATION_LIST)
        {
            return parent->childrens[i];
        }
    }

    // If there is no declaration list, create a new one
    ast_node *list = ast_create_node(AST_DECLARATION_LIST);
    ast_add_child_front(parent, list);
    return list;
}

ast_node *find_parent(ast_node *node, ast_type type)
{
    if (node == NULL)
    {
        return NULL;
    }

    // node parent
    ast_node *parent = node->parent;

    while (parent != NULL)
    {
        // If parent type = type we are looking for, return it
        if (parent->type == type)
        {
            return parent;
        }
        parent = parent->parent;
    }

    return NULL;
}

ast_node *find_last_parent_before(ast_node *node, ast_type type)
{
    if (node == NULL || node->parent == NULL)
    {
        return NULL;
    }

    ast_node *last_parent = node;

    while (last_parent->parent != NULL)
    {

        if (last_parent->parent->type == type)
        {
            return last_parent;
        }
        last_parent = last_parent->parent;
    }

    return NULL;
}

ast_node *find_available_temporary(ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    ast_node *list = find_declaration_list(node);

    for (int i = 0; i < list->childrens_count; i++)
    {
        if (list->childrens[i]->childrens[1]->type == AST_IDENTIFIER)
        {
            char *name = list->childrens[i]->childrens[1]->id;
            if (name[0] == '_' && name[1] == 't' && name[2] == 'e' && name[3] == 'm' && name[4] == 'p')
            {
                if (list->childrens[i]->childrens[1]->available)
                {
                    list->childrens[i]->childrens[1]->available = false;
                    return list->childrens[i]->childrens[1];
                }
            }
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

ast_node *get_temporary(ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Look for an available temporary variable
    ast_node *res = find_available_temporary(node);

    // If there is no available temporary variable, create a new one
    if (res == NULL)
    {
        res = ast_add_temporary(node);
    }

    return res;
}

char **get_all_temporaries(ast_node *node, int *count)
{
    if (node == NULL)
    {
        return NULL;
    }

    char **temporaries = (char **)malloc(sizeof(char *));
    int temp_count = 0;

    // If the node is an identifier and starts with "_temp" => add it to the temporaries array
    if (node->type == AST_IDENTIFIER)
    {
        if (node->id[0] == '_' && node->id[1] == 't' && node->id[2] == 'e' && node->id[3] == 'm' && node->id[4] == 'p')
        {
            temporaries[temp_count] = strdup(node->id);
            temp_count++;
        }
    }

    // primary expression  => '(' expression ')'
    // If the node is a primary expression => get all the temporaries from the children
    if (node->type == AST_PRIMARY_EXPRESSION)
    {
        char **temp = get_all_temporaries(node->childrens[0], &temp_count);
        return temp;
    }

    // unary operator => '-' / '*' / '&' expression
    // If the node is a unary operator => get all the temporaries from the children
    if (node->type == AST_UNARY)
    {
        char **temp = get_all_temporaries(node->childrens[1], &temp_count);
        return temp;
    }

    // boolean operator => '<' '>' '<=' '>=' '==' '!='
    // Operator => '+' '-' '*' '/'
    // Boolean logic => '&&' '||'
    // If the node is a boolean operator / operator / boolean logic => get all the temporaries from the left and right children
    // Add all the temporaries to the temporaries array
    if (node->type == AST_BOOL_OP || node->type == AST_OP || node->type == AST_BOOL_LOGIC)
    {
        int left = 0;
        int right = 0;
        char **temp = get_all_temporaries(node->childrens[0], &left);
        char **temp2 = get_all_temporaries(node->childrens[1], &right);

        temporaries = (char **)realloc(temporaries, sizeof(char *) * (temp_count + left + right));
        for (int i = 0; i < left; i++)
        {
            temporaries[temp_count] = temp[i];
            temp_count++;
        }
        for (int i = 0; i < right; i++)
        {
            temporaries[temp_count] = temp2[i];
            temp_count++;
        }

        free(temp);
        free(temp2);
    }

    *count = temp_count;
    return temporaries;
}

////////////////////////////////////////////////////////////////////////////////////////

void available_temporary(ast_node *node, char *name)
{
    if (node == NULL)
    {
        return;
    }

    ast_node *list = find_declaration_list(node);

    for (int i = 0; i < list->childrens_count; i++)
    {
        // If the node is an identifier & name = same as the name we are looking for => mark it as available
        if (list->childrens[i]->childrens[1]->type == AST_IDENTIFIER)
        {
            char *temp_name = list->childrens[i]->childrens[1]->id;
            if (strcmp(temp_name, name) == 0)
            {
                list->childrens[i]->childrens[1]->available = true;
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

ast_node *split_node_into_var(ast_node *node, char *name)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Find a parent : statement list node
    ast_node *parent = find_parent(node, AST_STATEMENT_LIST);

    // Find the last parent before the statement list node
    ast_node *last_parent = find_last_parent_before(node, AST_STATEMENT_LIST);

    ast_node *assignment = ast_create_node(AST_ASSIGNMENT);

    // Create a new identifier node for the assignment node
    ast_node *identifier = create_id_leaf(strdup(name));

    ast_add_child(assignment, identifier);
    ast_add_child(assignment, node);

    ast_add_child_before(parent, assignment, last_parent);

    // (it will be used to replace the node in the parent node)
    ast_node *res = create_id_leaf(strdup(name));

    return res;
}

ast_node *split_node_into_temp_var(ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Get a temporary variable
    ast_node *temp = get_temporary(node);

    // Split the node into a temporary variable
    ast_node *res = split_node_into_var(node, temp->id);

    return res;
}

int split_op(ast_node *node, char *var[], bool used[], int n)
{
    if (node == NULL)
    {
        return -1;
    }

    if (node->type == AST_PRIMARY_EXPRESSION)
    {
        return split_op(node->childrens[0], var, used, n);
    }

    if (node->sethi_ullman > 0 && node->type == AST_OP)
    {
        int left_i = split_op(node->childrens[0], var, used, n);
        int right_i = split_op(node->childrens[1], var, used, n);

        if (left_i != -1)
        {
            used[left_i] = false;
        }

        if (right_i != -1)
        {
            used[right_i] = false;
        }

        // Search for an available temp_index
        int temp_index = -1;
        for (int i = 0; i < n; i++)
        {
            if (used[i] == false)
            {
                temp_index = i;
                used[i] = true;
                break;
            }
        }

        ast_node *assignment = ast_create_node(AST_ASSIGNMENT);
        ast_node *identifier = create_id_leaf(strdup(var[temp_index]));
        ast_node *op = ast_create_node(AST_OP);
        op->id = node->id;

        if (left_i == -1)
        {
            ast_add_child(op, node->childrens[0]);
        }
        else
        {
            ast_node *left = create_id_leaf(strdup(var[left_i]));
            ast_add_child(op, left);
        }

        if (right_i == -1)
        {
            ast_add_child(op, node->childrens[1]);
        }
        else
        {
            ast_node *right = create_id_leaf(strdup(var[right_i]));
            ast_add_child(op, right);
        }

        ast_add_child(assignment, identifier);
        ast_add_child(assignment, op);

        ast_node *statement_list = find_parent(node, AST_STATEMENT_LIST);
        ast_node *before_statement = find_last_parent_before(node, AST_STATEMENT_LIST);

        ast_add_child_before(statement_list, assignment, before_statement);

        return temp_index;
    }
    else
    {
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void sethi_ullman(ast_node *node)
{
    if (node == NULL)
    {
        return;
    }

    switch (node->type)
    {
    case AST_CONSTANT:
    case AST_IDENTIFIER:
    {
        ast_node *parent = node->parent;
        if (parent->childrens[0] == node && parent->childrens[1]->type != AST_PRIMARY_EXPRESSION)
        {
            node->sethi_ullman = 1;
        }
        else
        {
            node->sethi_ullman = 0;
        }
    }
    break;
    case AST_PRIMARY_EXPRESSION:
        sethi_ullman(node->childrens[0]);
        node->sethi_ullman = node->childrens[0]->sethi_ullman;
        break;
    case AST_UNARY:
        sethi_ullman(node->childrens[1]);
        node->sethi_ullman = node->childrens[1]->sethi_ullman;
        break;
    case AST_OP:
        sethi_ullman(node->childrens[0]);
        sethi_ullman(node->childrens[1]);

        if (node->childrens[0]->sethi_ullman == node->childrens[1]->sethi_ullman)
        {
            node->sethi_ullman = node->childrens[0]->sethi_ullman + 1;
        }
        else
        {
            if (node->childrens[0]->sethi_ullman > node->childrens[1]->sethi_ullman)
            {
                node->sethi_ullman = node->childrens[0]->sethi_ullman;
            }
            else
            {
                node->sethi_ullman = node->childrens[1]->sethi_ullman;
            }
        }
        break;
    default:
        node->sethi_ullman = 0;
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

// Transform the AST into TAC (Three Address Code)
void *tac_transformation(ast_node *node)
{
    switch (node->type)
    {
    case AST_ARGUMENT_EXPRESSION_LIST:
        for (int i = 0; i < node->childrens_count; i++)
        {
            tac_transformation(node->childrens[i]);

            // Split the node into a temporary variable if it is not an identifier
            if (node->childrens[i]->type != AST_IDENTIFIER)
            {
                node->childrens[i] = split_node_into_temp_var(node->childrens[i]);
            }
        }

        for (int i = 0; i < node->childrens_count; i++)
        {
            available_temporary(node, node->childrens[i]->id);
        }
        break;
    case AST_BOOL_OP:
        bool split_left = false;
        bool split_right = false;

        tac_transformation(node->childrens[0]);
        tac_transformation(node->childrens[1]);

        // Split if the children are operations

        if (node->childrens[0]->type == AST_OP)
        {
            node->childrens[0] = split_node_into_temp_var(node->childrens[0]);
            split_left = true;
        }
        if (node->childrens[1]->type == AST_OP)
        {
            node->childrens[1] = split_node_into_temp_var(node->childrens[1]);
            split_right = true;
        }

        // Available the temporary variables only if the parent is not a boolean logic node
        // We still need the temporary variables for the boolean logic node
        ast_node *parent = find_parent(node, AST_BOOL_LOGIC);
        if (parent == NULL)
        {
            if (split_left)
            {
                available_temporary(node, node->childrens[0]->id);
            }
            if (split_right)
            {
                available_temporary(node, node->childrens[1]->id);
            }
        }
        break;

    case AST_BOOL_LOGIC:
        tac_transformation(node->childrens[0]);
        tac_transformation(node->childrens[1]);

        ast_node *op = find_parent(node, AST_BOOL_OP);

        // No boolean operator parent -> used the temporary variables
        if (op == NULL)
        {
            int left_count = 0;
            int right_count = 0;

            char **left = get_all_temporaries(node->childrens[0], &left_count);
            char **right = get_all_temporaries(node->childrens[1], &right_count);

            for (int i = 0; i < left_count; i++)
            {
                available_temporary(node, left[i]);
            }

            for (int i = 0; i < right_count; i++)
            {
                available_temporary(node, right[i]);
            }
        }

        break;
    case AST_ASSIGNMENT:
        tac_transformation(node->childrens[0]);
        tac_transformation(node->childrens[1]);

        if (node->childrens[1]->type == AST_ASSIGNMENT)
        {
            node->childrens[1] = split_node_into_var(node->childrens[1]->childrens[1], node->childrens[1]->childrens[0]->id);
        }
        break;
    case AST_OP:
    {
        ast_node *left = node->childrens[0];
        ast_node *right = node->childrens[1];

        if (left->type == AST_OP || right->type == AST_OP || left->type == AST_PRIMARY_EXPRESSION || right->type == AST_PRIMARY_EXPRESSION)
        {
            sethi_ullman(node);

            if (node->sethi_ullman > 0)
            {
                char *var[node->sethi_ullman];
                bool used[node->sethi_ullman];

                for (int i = 0; i < node->sethi_ullman; i++)
                {
                    ast_node *temp = get_temporary(node);
                    var[i] = temp->id;
                    used[i] = false;
                }

                int temp_index = split_op(node, var, used, node->sethi_ullman);

                for (int i = 0; i < node->sethi_ullman; i++)
                {
                    available_temporary(node, var[i]);
                }

                if (temp_index != -1)
                {
                    ast_node *temp = create_id_leaf(strdup(var[temp_index]));
                    temp->parent = node->parent;
                    *node = *temp;
                }
            }
        }
    break;
    }
    default:
        for (int i = 0; i < node->childrens_count; i++)
        {
            tac_transformation(node->childrens[i]);
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void free_ast(ast_node *root)
{
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i < root->childrens_count; i++)
    {
        free_ast(root->childrens[i]);
    }

    free(root);
}

////////////////////////////////////////////////////////////////////////////////////////

void print_complete_ast_helper(ast_node *node, int indent)
{
    if (node == NULL)
    {
        return;
    }

    for (int i = 0; i < indent - 1; i++)
    {
        printf("    ");
    }
    if (indent > 0)
    {
        printf("└── ");
    }

    switch (node->type)
    {
    case AST_PROGRAM:
        printf("Program\n");
        break;
    case AST_FUNCTION_DEFINITION:
        printf("Function Definition\n");
        break;
    case AST_RETURN:
        printf("Return\n");
        break;
    case AST_FOR:
        printf("For\n");
        break;
    case AST_WHILE:
        printf("While\n");
        break;
    case AST_IF:
        printf("If\n");
        break;
    case AST_IF_ELSE:
        printf("If Else\n");
        break;
    case AST_EXPRESSION_STATEMENT:
        printf("Expression Statement\n");
        break;
    case AST_STATEMENT_LIST:
        printf("Statement List\n");
        break;
    case AST_DECLARATION_LIST:
        printf("Declaration List\n");
        break;
    case AST_COMPOUND_STATEMENT:
        printf("Compound Statement\n");
        break;
    case AST_PARAMETER_DECLARATION:
        printf("Parameter Declaration\n");
        break;
    case AST_PARAMETER_LIST:
        printf("Parameter List\n");
        break;
    case AST_DIRECT_DECLARATOR:
        printf("Direct Declarator\n");
        break;
    case AST_STRUCT_DECLARATION:
        printf("Struct Declaration\n");
        break;
    case AST_STRUCT_DECLARATION_LIST:
        printf("Struct Declaration List\n");
        break;
    case AST_STRUCT_SPECIFIER:
        printf("Struct Specifier\n");
        break;
    case AST_TYPE_SPECIFIER:
        printf("Type Specifier : %s\n", node->id);
        break;
    case AST_DECLARATION:
        printf("Declaration\n");
        break;
    case AST_ASSIGNMENT:
        printf("Assignment\n");
        break;
    case AST_BOOL_LOGIC:
        printf("Boolean Logic : %s\n", node->id);
        break;
    case AST_BOOL_OP:
        printf("Boolean Operator : %s\n", node->id);
        break;
    case AST_OP:
        printf("Operator : %s\n", node->id);
        break;
    case AST_UNARY_OPERATOR:
        printf("Unary Operator : %s\n", node->id);
        break;
    case AST_UNARY_OP:
        printf("Unary Operator : %s\n", node->id);
        break;
    case AST_UNARY_SIZEOF:
        printf("Unary Sizeof\n");
        break;
    case AST_UNARY:
        printf("Unary\n");
        break;
    case AST_ARGUMENT_EXPRESSION_LIST:
        printf("Argument Expression List\n");
        break;
    case AST_POSTFIX_POINTER:
        printf("Postfix Pointer\n");
        break;
    case AST_POSTFIX_IDENTIFIER:
        printf("Postfix Identifier\n");
        break;
    case AST_POSTFIX_ARGUMENT:
        printf("Postfix Argument\n");
        break;
    case AST_POSTFIX_NO_ARGUMENT:
        printf("Postfix No Argument\n");
        break;
    case AST_CONSTANT:
        printf("Constant : %d\n", node->value);
        break;
    case AST_IDENTIFIER:
        printf("Identifier : %s\n", node->id);
        break;
    case AST_PRIMARY_EXPRESSION:
        printf("Primary Expression\n");
        break;
    case AST_DECLARATOR:
        printf("Declarator\n");
        break;
    case AST_STAR_DECLARATOR:
        printf("Star Declarator\n");
        break;
    case AST_STRUCT_VARIABLE_SPECIFIER:
        printf("Struct Variable Specifier\n");
        break;
    case AST_EXT_DECLARATION:
        printf("External Declaration\n");
        break;
    default:
        printf("Unknown\n");
        break;
    }

    for (int i = 0; i < node->childrens_count; i++)
    {
        print_complete_ast_helper(node->childrens[i], indent + 1);
    }
}

void print_complete_ast(ast_node *root)
{
    print_complete_ast_helper(root, 0);
}