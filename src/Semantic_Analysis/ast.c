#include "ast.h"

int var_count = 0;

Ast_node *ast_create_node(Ast_type type)
{
    Ast_node *node = (Ast_node *)malloc(sizeof(Ast_node));

    node->type = type;
    node->type_name = NULL;

    node->parent = NULL;
    node->children = NULL;
    node->children_count = 0;

    node->value = -1;
    node->id = NULL;

    node->size = -1;
    node->offset = -1;

    node->false_label = -1;
    node->true_label = -1;

    node->available = true;

    node->sethi_ullman = 0;

    node->struct_name = NULL;
    node->pointer = false;

    return node;
}

Ast_node *create_int_leaf(int value)
{
    Ast_node *node = ast_create_node(AST_CONSTANT);

    node->value = value;
    return node;
}

Ast_node *create_id_leaf(char *name)
{
    Ast_node *node = ast_create_node(AST_IDENTIFIER);

    node->id = name;
    return node;
}

////////////////////////////////////////////////////////////////////////////////////////

void ast_add_child(Ast_node *parent, Ast_node *child)
{
    child->parent = parent;

    if (parent->children == NULL)
    {
        parent->children = (Ast_node **)malloc(sizeof(Ast_node *));
    }
    else
    {
        parent->children = (Ast_node **)realloc(parent->children, sizeof(Ast_node *) * (parent->children_count + 1));
    }

    parent->children[parent->children_count] = child;
    parent->children_count++;
}

void ast_add_child_front(Ast_node *parent, Ast_node *child)
{
    child->parent = parent;

    if (parent->children == NULL)
    {
        parent->children = (Ast_node **)malloc(sizeof(Ast_node *));
    }
    else
    {
        parent->children = (Ast_node **)realloc(parent->children, sizeof(Ast_node *) * (parent->children_count + 1));
    }

    // Shift all the elements to the right
    for (int i = parent->children_count; i > 0; i--)
    {
        parent->children[i] = parent->children[i - 1];
    }

    parent->children[0] = child;
    parent->children_count++;
}

// Add a child before a specific child (before is the specific child)
void ast_add_child_before(Ast_node *parent, Ast_node *child, Ast_node *before)
{
    child->parent = parent;

    if (parent->children == NULL)
    {
        parent->children = (Ast_node **)malloc(sizeof(Ast_node *));
    }
    else
    {
        parent->children = (Ast_node **)realloc(parent->children, sizeof(Ast_node *) * (parent->children_count + 1));
    }

    // Find the index of before
    int i = 0;
    for (i = 0; i < parent->children_count; i++)
    {
        if (parent->children[i] == before)
        {
            break;
        }
    }

    // Shift all the elements to the right
    for (int j = parent->children_count; j > i; j--)
    {
        parent->children[j] = parent->children[j - 1];
    }

    parent->children[i] = child;
    parent->children_count++;
}

Ast_node *ast_add_temporary(Ast_node *node, char *type, bool pointer)
{
    Ast_node *list = find_declaration_list(node);

    Ast_node *declaration = ast_create_node(AST_DECLARATION);

    Ast_node *specifier = ast_create_node(AST_TYPE_SPECIFIER);

    specifier->id = type;

    char *name ;
    asprintf(&name, "_temp_%d", var_count++);
    // identifier node for the declaration list

    Ast_node *identifier = create_id_leaf(strdup(name));

    ast_add_child(declaration, specifier);

    if (pointer)
    {
        Ast_node *star = ast_create_node(AST_STAR_DECLARATOR);
        ast_add_child(star, identifier);
        ast_add_child(declaration, star);
    }
    else
    {
        ast_add_child(declaration, identifier);
    }

    identifier->available = false;

    ast_add_child(list, declaration);

    return identifier;
}

////////////////////////////////////////////////////////////////////////////////////////

Ast_node *find_declaration_list(Ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Find a compound statement node
    Ast_node *parent = find_parent(node, AST_COMPOUND_STATEMENT);

    for (int i = 0; i < parent->children_count; i++)
    {
        // If the node is a declaration list
        if (parent->children[i]->type == AST_DECLARATION_LIST)
        {
            return parent->children[i];
        }
    }

    // If there is no declaration list, create a new one
    Ast_node *list = ast_create_node(AST_DECLARATION_LIST);
    ast_add_child_front(parent, list);
    return list;
}

Ast_node *find_parent(Ast_node *node, Ast_type type)
{
    if (node == NULL)
    {
        return NULL;
    }

    // node parent
    Ast_node *parent = node->parent;

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

Ast_node *find_last_parent_before(Ast_node *node, Ast_type type)
{
    if (node == NULL || node->parent == NULL)
    {
        return NULL;
    }

    Ast_node *last_parent = node;

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

Ast_node *find_available_temporary(Ast_node *node, char *type, bool pointer)
{
    if (node == NULL)
    {
        return NULL;
    }

    Ast_node *list = find_declaration_list(node);

    for (int i = 0; i < list->children_count; i++)
    {
        if (strcmp(list->children[i]->children[0]->id, type) != 0)
        {
            continue;
        }

        if ((list->children[i]->children[1]->type == AST_IDENTIFIER && !pointer) || (pointer && list->children[i]->children[1]->type == AST_STAR_DECLARATOR))
        {
            char *name = find_first_identifier(list->children[i]->children[1]);
            if (name[0] == '_' && name[1] == 't' && name[2] == 'e' && name[3] == 'm' && name[4] == 'p')
            {
                if (list->children[i]->children[1]->available)
                {
                    list->children[i]->children[1]->available = false;
                    return list->children[i]->children[1];
                }
            }
        }
    }
    return NULL;
}

char *find_first_identifier(Ast_node *node)
{
    if (node == NULL)
    {
        return NULL;
    }

    if (node->type == AST_IDENTIFIER)
    {
        return node->id;
    }

    for (int i = 0; i < node->children_count; i++)
    {
        char *res = find_first_identifier(node->children[i]);
        if (res != NULL)
        {
            return res;
        }
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

Ast_node *get_temporary(Ast_node *node, char *type, bool pointer)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Look for an available temporary variable
    Ast_node *res = find_available_temporary(node, type, pointer);

    // If there is no available temporary variable, create a new one
    if (res == NULL)
    {
        res = ast_add_temporary(node, type, pointer);
    }

    return res;
}

char **get_all_temporaries(Ast_node *node, int *count)
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
        char **temp = get_all_temporaries(node->children[0], &temp_count);
        return temp;
    }

    // unary operator => '-' / '*' / '&' expression
    // If the node is a unary operator => get all the temporaries from the children
    if (node->type == AST_UNARY)
    {
        char **temp = get_all_temporaries(node->children[1], &temp_count);
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
        char **temp = get_all_temporaries(node->children[0], &left);
        char **temp2 = get_all_temporaries(node->children[1], &right);

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

void available_temporary(Ast_node *node, char *name)
{
    if (node == NULL)
    {
        return;
    }

    Ast_node *list = find_declaration_list(node);

    for (int i = 0; i < list->children_count; i++)
    {
        // If the node is an identifier & name = same as the name we are looking for => mark it as available
        if (list->children[i]->type == AST_DECLARATION)
        {
            char *temp_name = find_first_identifier(list->children[i]->children[1]);
            if (strcmp(temp_name, name) == 0)
            {
                list->children[i]->children[1]->available = true;
                return;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

Ast_node *split_node_into_var(Ast_node *node, char *name)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Find a parent : statement list node
    Ast_node *parent = find_parent(node, AST_STATEMENT_LIST);

    // Find the last parent before the statement list node
    Ast_node *last_parent = find_last_parent_before(node, AST_STATEMENT_LIST);

    Ast_node *assignment = ast_create_node(AST_ASSIGNMENT);

    // Create a new identifier node for the assignment node
    Ast_node *identifier = create_id_leaf(strdup(name));

    ast_add_child(assignment, identifier);
    ast_add_child(assignment, node);

    ast_add_child_before(parent, assignment, last_parent);

    // (it will be used to replace the node in the parent node)
    Ast_node *res = create_id_leaf(strdup(name));

    return res;
}

Ast_node *split_node_into_temp_var(Ast_node *node, char *type, bool pointer)
{
    if (node == NULL)
    {
        return NULL;
    }

    // Get a temporary variable
    Ast_node *temp = get_temporary(node, type, pointer);

    char *id = find_first_identifier(temp);

    // Split the node into a temporary variable
    Ast_node *res = split_node_into_var(node, id);

    return res;
}

int split_op(Ast_node *node, char *var[], bool used[], int n)
{
    if (node == NULL)
    {
        return -1;
    }

    if (node->type == AST_PRIMARY_EXPRESSION)
    {
        return split_op(node->children[0], var, used, n);
    }

    if (node->type == AST_UNARY)
    {
        int split = split_op(node->children[1], var, used, n);

        if (split != -1)
        {
            used[split] = false;
        }

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

        Ast_node *assignment = ast_create_node(AST_ASSIGNMENT);
        Ast_node *identifier = create_id_leaf(strdup(var[temp_index]));
        Ast_node *unary = ast_create_node(AST_UNARY);
        Ast_node *op = ast_create_node(AST_UNARY_OP);

        op->id = node->children[0]->id;

        ast_add_child(unary, op);

        if (split == -1)
        {
            ast_add_child(unary, node->children[1]);
        }
        else
        {
            Ast_node *temp = create_id_leaf(strdup(var[split]));
            ast_add_child(unary, temp);
        }

        ast_add_child(assignment, identifier);
        ast_add_child(assignment, unary);

        Ast_node *statement_list = find_parent(node, AST_STATEMENT_LIST);
        Ast_node *before_statement = find_last_parent_before(node, AST_STATEMENT_LIST);

        ast_add_child_before(statement_list, assignment, before_statement);

        return temp_index;
    }

    if (node->sethi_ullman > 0 && node->type == AST_OP)
    {
        int left_i = split_op(node->children[0], var, used, n);
        int right_i = split_op(node->children[1], var, used, n);

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

        Ast_node *assignment = ast_create_node(AST_ASSIGNMENT);
        Ast_node *identifier = create_id_leaf(strdup(var[temp_index]));
        Ast_node *op = ast_create_node(AST_OP);

        op->id = node->id;

        if (left_i == -1)
        {
            ast_add_child(op, node->children[0]);
        }
        else
        {
            Ast_node *left = create_id_leaf(strdup(var[left_i]));
            ast_add_child(op, left);
        }

        if (right_i == -1)
        {
            ast_add_child(op, node->children[1]);
        }
        else
        {
            Ast_node *right = create_id_leaf(strdup(var[right_i]));
            ast_add_child(op, right);
        }

        ast_add_child(assignment, identifier);
        ast_add_child(assignment, op);

        Ast_node *statement_list = find_parent(node, AST_STATEMENT_LIST);
        Ast_node *before_statement = find_last_parent_before(node, AST_STATEMENT_LIST);

        ast_add_child_before(statement_list, assignment, before_statement);

        return temp_index;
    }
    else
    {
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void sethi_ullman(Ast_node *node)
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
        Ast_node *parent = node->parent;
        if (parent->children[0] == node && parent->children[1]->type != AST_PRIMARY_EXPRESSION)
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
        sethi_ullman(node->children[0]);
        node->sethi_ullman = node->children[0]->sethi_ullman;
        break;
    case AST_UNARY:
        sethi_ullman(node->children[1]);
        node->sethi_ullman = node->children[1]->sethi_ullman;
        break;
    case AST_OP:
        sethi_ullman(node->children[0]);
        sethi_ullman(node->children[1]);

        if (node->children[0]->sethi_ullman == node->children[1]->sethi_ullman)
        {
            node->sethi_ullman = node->children[0]->sethi_ullman + 1;
        }
        else
        {
            if (node->children[0]->sethi_ullman > node->children[1]->sethi_ullman)
            {
                node->sethi_ullman = node->children[0]->sethi_ullman;
            }
            else
            {
                node->sethi_ullman = node->children[1]->sethi_ullman;
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
void *tac_transformation(Ast_node *node)
{
    switch (node->type)
    {
    case AST_ARGUMENT_EXPRESSION_LIST:
        for (int i = 0; i < node->children_count; i++)
        {
            tac_transformation(node->children[i]);

            // Split the node into a temporary variable if it is not an identifier
            if (node->children[i]->type != AST_IDENTIFIER)
            {
                node->children[i] = split_node_into_temp_var(node->children[i], "int", false);
            }
        }

        for (int i = 0; i < node->children_count; i++)
        {
            available_temporary(node, node->children[i]->id);
        }
        break;
    case AST_BOOL_OP:
        bool split_left = false;
        bool split_right = false;

        tac_transformation(node->children[0]);
        tac_transformation(node->children[1]);

        // Split if the children are operations

        if (node->children[0]->type == AST_OP || node->children[0]->type == AST_UNARY)
        {
            node->children[0] = split_node_into_temp_var(node->children[0], "int", false);
            split_left = true;
        }
        if (node->children[1]->type == AST_OP || node->children[1]->type == AST_UNARY)
        {
            node->children[1] = split_node_into_temp_var(node->children[1], "int", false);
            split_right = true;
        }

        // Available the temporary variables only if the parent is not a boolean logic node
        // We still need the temporary variables for the boolean logic node
        Ast_node *parent = find_parent(node, AST_BOOL_LOGIC);
        if (parent == NULL)
        {
            if (split_left)
            {
                available_temporary(node, node->children[0]->id);
            }
            if (split_right)
            {
                available_temporary(node, node->children[1]->id);
            }
        }
        break;

    case AST_BOOL_LOGIC:
        tac_transformation(node->children[0]);
        tac_transformation(node->children[1]);

        Ast_node *op = find_parent(node, AST_BOOL_OP);

        // No boolean operator parent -> used the temporary variables
        if (op == NULL)
        {
            int left_count = 0;
            int right_count = 0;

            char **left = get_all_temporaries(node->children[0], &left_count);
            char **right = get_all_temporaries(node->children[1], &right_count);

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
        tac_transformation(node->children[0]);
        tac_transformation(node->children[1]);

        if (node->children[1]->type == AST_ASSIGNMENT)
        {
            node->children[1] = split_node_into_var(node->children[1]->children[1], node->children[1]->children[0]->id);
        }

        if (node->children[0]->type == AST_STAR_DECLARATOR && node->children[1]->type != AST_CONSTANT && node->children[1]->type != AST_IDENTIFIER)
        {
            node->children[1] = split_node_into_temp_var(node->children[1], "int", false);
        }

        break;
    case AST_OP:
    {
        Ast_node *left = node->children[0];
        Ast_node *right = node->children[1];

        if (left->type == AST_OP || right->type == AST_OP 
            || left->type == AST_PRIMARY_EXPRESSION 
                || right->type == AST_PRIMARY_EXPRESSION || left->type == AST_UNARY 
                    || right->type == AST_UNARY || left->type == AST_POSTFIX
                        || right->type == AST_POSTFIX)
        {
            if (left->type == AST_POSTFIX)
            {
                tac_transformation(left);

                node->children[0] = split_node_into_temp_var(left, left->type_name, left->pointer);

                node->children[0]->parent = node;
            }

            if (right->type == AST_POSTFIX)
            {
                tac_transformation(right);

                node->children[1] = split_node_into_temp_var(right, right->type_name, right->pointer);

                node->children[1]->parent = node;
            }

            if (left->type == AST_POSTFIX)
            {
                available_temporary(node, node->children[0]->id);
            }

            if (right->type == AST_POSTFIX)
            {
                available_temporary(node, node->children[1]->id);
            }

            sethi_ullman(node);

            if (node->sethi_ullman > 0)
            {
                char *var[node->sethi_ullman];
                bool used[node->sethi_ullman];

                for (int i = 0; i < node->sethi_ullman; i++)
                {
                    Ast_node *temp = get_temporary(node, "int", false);
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
                    Ast_node *temp = create_id_leaf(strdup(var[temp_index]));
                    temp->parent = node->parent;
                    *node = *temp;
                }
            }
        }
        break;
    }
    case AST_POSTFIX_POINTER:
    {
       Ast_node *declarator;

        if (strcmp(node->type_name, "struct") == 0)
        {
            declarator = get_temporary(node, "void", true);
        }
        else
        {
            declarator = get_temporary(node, node->type_name, true);
        }

        declarator->offset = node->offset;

        declarator->size = node->size;

        char *name = find_first_identifier(declarator);

        if (node->offset != -1)
        {
            Ast_node *statement = find_parent(node, AST_STATEMENT_LIST);
            Ast_node *before_statement = find_last_parent_before(node, AST_STATEMENT_LIST);

            Ast_node *assignment = ast_create_node(AST_ASSIGNMENT);
            Ast_node *identifier = create_id_leaf(strdup(name));

            ast_add_child(assignment, identifier);

            Ast_node *op = ast_create_node(AST_OP);
            op->id = "+";

            char *name_2 = find_first_identifier(node->children[0]);
            Ast_node *identifier_2 = create_id_leaf(strdup(name_2));

            ast_add_child(op, identifier_2);

            Ast_node *constant = create_int_leaf(declarator->offset);

            ast_add_child(op, constant);
            ast_add_child(assignment, op);
            ast_add_child_before(statement, assignment, before_statement);
        }

        node->type = AST_STAR_DECLARATOR;

        Ast_node *identifier_3 = create_id_leaf(strdup(name));

        node->children[0] = identifier_3;

        available_temporary(node, name);

        break;
    }
    default:
        for (int i = 0; i < node->children_count; i++)
        {
            tac_transformation(node->children[i]);
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void free_ast(Ast_node *root)
{
    if (root == NULL)
    {
        return;
    }

    for (int i = 0; i < root->children_count; i++)
    {
        free_ast(root->children[i]);
    }

    free(root);
}

////////////////////////////////////////////////////////////////////////////////////////

void print_complete_ast_helper(Ast_node *node, int indent)
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
    case AST_DIRECT_DECLARATOR_END:
        printf("Direct Declarator End\n");
        break;
    case AST_STRUCT_DECLARATION_LIST:
        printf("Struct Declaration List\n");
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
    case AST_POSTFIX:
        printf("Postfix\n");
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
    case AST_EXT_DECLARATION:
        printf("External Declaration\n");
        break;
    case AST_STRUCT:
        printf("Struct\n");
        break;
    default:
        printf("Unknown\n");
        break;
    }

    for (int i = 0; i < node->children_count; i++)
    {
        print_complete_ast_helper(node->children[i], indent + 1);
    }
}

void print_complete_ast(Ast_node *root)
{
    print_complete_ast_helper(root, 0);
}