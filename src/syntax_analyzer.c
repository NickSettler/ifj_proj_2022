/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *   xmaroc00, Elena Marochkina
 *
 * @file syntax_analyzer.c
 * @brief 
 * @date 17.10.2022
 */

#include "syntax_analyzer.h"
#include "symtable.h"

struct {
    char *text, *enum_text;
    syntax_tree_token_type token_type;
    bool right_associative, is_binary, is_unary;
    int precedence;
    syntax_tree_node_type node_type;
} attributes[] = {
        {"EOF",      "End_of_file",       SYN_TOKEN_EOF,                  false, false, false, -1, (syntax_tree_node_type) -1},
        {"ID",       "Identifier",        SYN_TOKEN_IDENTIFIER,           false, false, false, -1, SYN_NODE_IDENTIFIER},
        {"STRING",   "String",            SYN_TOKEN_STRING,               false, false, false, -1, SYN_NODE_STRING},
        {"INTEGER",  "Integer",           SYN_TOKEN_INTEGER,              false, false, false, -1, SYN_NODE_INTEGER},
        {"FLOAT",    "Float",             SYN_TOKEN_FLOAT,                false, false, false, -1, SYN_NODE_FLOAT},
        {"+",        "Op_add",            SYN_TOKEN_ADD,                  false, true,  false, 12, SYN_NODE_ADD},
        {"-",        "Op_sub",            SYN_TOKEN_SUB,                  false, true,  false, 12, SYN_NODE_SUB},
        {"*",        "Op_multiply",       SYN_TOKEN_MUL,                  false, true,  false, 13, SYN_NODE_MUL},
        {"/",        "Op_divide",         SYN_TOKEN_DIV,                  false, true,  false, 13, SYN_NODE_DIV},
        {"-",        "Op_negate",         SYN_TOKEN_NEGATE,               false, false, true,  14, SYN_NODE_NEGATE},
        {"<",        "Op_less",           SYN_TOKEN_LESS,                 false, true,  false, 10, SYN_NODE_LESS},
        {"<=",       "Op_lessequal",      SYN_TOKEN_LESS_EQUAL,           false, true,  false, 10, SYN_NODE_LESS_EQUAL},
        {">",        "Op_greater",        SYN_TOKEN_GREATER,              false, true,  false, 10, SYN_NODE_GREATER},
        {">=",       "Op_greaterequal",   SYN_TOKEN_GREATER_EQUAL,        false, true,  false, 10, SYN_NODE_GREATER_EQUAL},
        {"==",       "Op_equal",          SYN_TOKEN_EQUAL,                false, true,  false, 9,  SYN_NODE_EQUAL},
        {"!=",       "Op_not_equal",      SYN_TOKEN_NOT_EQUAL,            false, true,  false, 9,  SYN_NODE_NOT_EQUAL},
        {"=",        "Op_Assign",         SYN_TOKEN_ASSIGN,               false, false, false, -1, SYN_NODE_ASSIGN},
        {";",        "Semicolon",         SYN_TOKEN_SEMICOLON,            false, false, false, -1, (syntax_tree_node_type) -1},
        {":",        "Colon",             SYN_TOKEN_COLON,                false, false, false, -1, (syntax_tree_node_type) -1},
        {",",        "Comma",             SYN_TOKEN_COMMA,                false, false, false, -1, (syntax_tree_node_type) -1},
        {"if",       "Keyword_IF",        SYN_TOKEN_KEYWORD_IF,           false, false, false, -1, SYN_NODE_KEYWORD_IF},
        {"else",     "Keyword_ELSE",      SYN_TOKEN_KEYWORD_ELSE,         false, false, false, -1, (syntax_tree_node_type) -1},
        {"function", "Keyword_FUNCTION",  SYN_TOKEN_KEYWORD_FUNCTION,     false, false, false, -1, (syntax_tree_node_type) -1},
        {"int",      "Keyword_INT",       SYN_TOKEN_KEYWORD_INT,          false, false, false, -1, (syntax_tree_node_type) -1},
        {"float",    "Keyword_FLOAT",     SYN_TOKEN_KEYWORD_FLOAT,        false, false, false, -1, (syntax_tree_node_type) -1},
        {"string",   "Keyword_STRING",    SYN_TOKEN_KEYWORD_STRING,       false, false, false, -1, (syntax_tree_node_type) -1},
        {"void",     "Keyword_VOID",      SYN_TOKEN_KEYWORD_VOID,         false, false, false, -1, (syntax_tree_node_type) -1},
        {"(",        "LeftParenthesis",   SYN_TOKEN_LEFT_PARENTHESIS,     false, false, false, -1, (syntax_tree_node_type) -1},
        {")",        "RightParenthesis",  SYN_TOKEN_RIGHT_PARENTHESIS,    false, false, false, -1, (syntax_tree_node_type) -1},
        {"{",        "LeftCurlyBracket",  SYN_TOKEN_RIGHT_CURLY_BRACKETS, false, false, false, -1, (syntax_tree_node_type) -1},
        {"}",        "RightCurlyBracket", SYN_TOKEN_RIGHT_CURLY_BRACKETS, false, false, false, -1, (syntax_tree_node_type) -1}
};

syntax_abstract_tree_t *
make_binary_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node")
    }

    syntax_abstract_tree_attr_t *attrs = (syntax_abstract_tree_attr_t *) malloc(sizeof(syntax_abstract_tree_attr_t));
    if (attrs == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node attributes")
    }

    tree->type = type;
    tree->left = left;
    tree->middle = NULL;
    tree->right = right;
    tree->attrs = attrs;

    return tree;
}

syntax_abstract_tree_t *
make_ternary_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *middle,
                  syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node")
    }

    syntax_abstract_tree_attr_t *attrs = (syntax_abstract_tree_attr_t *) malloc(sizeof(syntax_abstract_tree_attr_t));
    if (attrs == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node attributes")
    }

    tree->type = type;
    tree->left = left;
    tree->middle = middle;
    tree->right = right;
    tree->attrs = attrs;

    return tree;
}

syntax_abstract_tree_t *make_binary_leaf(syntax_tree_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = make_binary_node(type, NULL, NULL);

    tree->value = value;

    return tree;
}

syntax_abstract_tree_t *make_ternary_leaf(syntax_tree_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = make_ternary_node(type, NULL, NULL, NULL);

    tree->value = value;

    return tree;
}

void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree) {
    if (!tree) return;

    syntax_abstract_tree_print(output, tree->left);
    fprintf(output, "%d ", tree->type);
    syntax_abstract_tree_print(output, tree->middle);
    syntax_abstract_tree_print(output, tree->right);
}

bool check_tree_using(syntax_abstract_tree_t *tree, bool (*check)(syntax_abstract_tree_t *)) {
    if (!tree) return true;
    return check(tree) &&
           check_tree_using(tree->left, check) &&
           check_tree_using(tree->middle, check) &&
           check_tree_using(tree->right, check);
}

syntax_abstract_tree_t *get_from_tree_using(syntax_abstract_tree_t *tree, bool (*check)(syntax_abstract_tree_t *)) {
    if (!tree) return NULL;
    if (check(tree)) return tree;
    syntax_abstract_tree_t *node = get_from_tree_using(tree->left, check);
    if (node) return node;
    node = get_from_tree_using(tree->middle, check);
    if (node) return node;
    node = get_from_tree_using(tree->right, check);
    if (node) return node;
    return NULL;
}

bool is_defined(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_IDENTIFIER) {
        tree_node_t *node = find_token(tree->value->value);
        if (!node) return false;

        return node->defined == true;
    }

    return true;
}

bool is_undefined(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_IDENTIFIER) {
        tree_node_t *node = find_token(tree->value->value);
        if (!node) return true;

        return node->defined == false;
    }

    return false;
}

bool is_correct_if(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_KEYWORD_IF) {
        syntax_abstract_tree_t *condition = tree->left;
        syntax_abstract_tree_t *body = tree->middle;
        syntax_abstract_tree_t *else_body = tree->right;

        if (!condition || !body) return false;
        if (else_body && else_body->type == SYN_NODE_KEYWORD_IF) return is_correct_if(else_body);

        return true;
    }

    return true;
}

void expect_token(const char *msg, syntax_tree_token_type type) {
    if (get_token_type(lexical_token->type) != type) {
        SYNTAX_ERROR("%s Expecting %s, found: %s\n", msg, attributes[type].text,
                     attributes[get_token_type(lexical_token->type)].text);
    }
}

syntax_abstract_tree_t *f_args(FILE *fd, syntax_abstract_tree_t *args) {
    lexical_token = get_token(fd);
    syntax_tree_token_type type = get_token_type(lexical_token->type);
    if (type == SYN_TOKEN_RIGHT_PARENTHESIS || type == SYN_TOKEN_COMMA) {
        lexical_token = get_token(fd);
        if (type == SYN_TOKEN_RIGHT_PARENTHESIS) return args;
    } else if (type != SYN_TOKEN_IDENTIFIER && type != SYN_TOKEN_KEYWORD_STRING && type != SYN_TOKEN_KEYWORD_INT &&
               type != SYN_TOKEN_KEYWORD_FLOAT && type == SYN_TOKEN_KEYWORD_VOID) {
        SYNTAX_ERROR("Expecting ',' or ')', found: %s\n", attributes[get_token_type(lexical_token->type)].text)
    }

    if (args == NULL)
        args = make_binary_node(SYN_NODE_FUNCTION_ARG, NULL, NULL);

    args->left = make_binary_leaf(SYN_NODE_IDENTIFIER, string_init(""));

    type = get_token_type(lexical_token->type);
    switch (type) {
        case SYN_TOKEN_KEYWORD_VOID:
        case SYN_TOKEN_KEYWORD_INT:
        case SYN_TOKEN_KEYWORD_FLOAT:
        case SYN_TOKEN_KEYWORD_STRING: {
            args->left->attrs->token_type = type;
            lexical_token = get_token(fd);
            expect_token("Function argument declaration", SYN_TOKEN_IDENTIFIER);
            args->left->value = string_init(lexical_token->value);
            // TODO: Move all semantic analysis to separate file
            find_token(lexical_token->value)->defined = true;
            break;
        }
        case SYN_TOKEN_IDENTIFIER: {
            args->left->value = string_init(lexical_token->value);
            args->left->attrs->token_type = SYN_TOKEN_KEYWORD_VOID;
            // TODO: Move all semantic analysis to separate file
            find_token(lexical_token->value)->defined = true;
            break;
        }
        default: {
            SYNTAX_ERROR("Expecting type or identifier, found: %s\n", attributes[type].text)
        }
    }

    args->right = f_args(fd, args->right);
    return args;
}

syntax_abstract_tree_t *f_dec_stats(FILE *fd) {
    syntax_abstract_tree_t *func;

    expect_token("Function", SYN_TOKEN_KEYWORD_FUNCTION);
    lexical_token = get_token(fd);
    expect_token("Identifier", SYN_TOKEN_IDENTIFIER);
    func = make_binary_node(SYN_NODE_FUNCTION_DECLARATION,
                            make_binary_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value)),
                            NULL);
    lexical_token = get_token(fd);
    expect_token("Left parenthesis", SYN_TOKEN_LEFT_PARENTHESIS);
    func->middle = f_args(fd, NULL);

    if (get_token_type(lexical_token->type) == SYN_TOKEN_COLON) {
        lexical_token = get_token(fd);
        syntax_tree_token_type type = get_token_type(lexical_token->type);
        if (type != SYN_TOKEN_KEYWORD_VOID && type != SYN_TOKEN_KEYWORD_INT &&
            type != SYN_TOKEN_KEYWORD_FLOAT && type != SYN_TOKEN_KEYWORD_STRING) {
            SYNTAX_ERROR("Expecting function return type, found: %s\n", attributes[type].text)
        }
        func->attrs->token_type = type;
        lexical_token = get_token(fd);
    }

    expect_token("Left curly brackets", SYN_TOKEN_LEFT_CURLY_BRACKETS);

    func->right = stmt(fd);

    return func;
}

syntax_abstract_tree_t *parenthesis_expression(FILE *fd) {
    expect_token("Left parenthesis", SYN_TOKEN_LEFT_PARENTHESIS);
    lexical_token = get_token(fd);
    syntax_abstract_tree_t *tree = expression(fd, 0);
    expect_token("Right parenthesis", SYN_TOKEN_RIGHT_PARENTHESIS);
    lexical_token = get_token(fd);
    return tree;
}

syntax_abstract_tree_t *expression(FILE *fd, int precedence) {
    syntax_abstract_tree_t *x = NULL, *node;
    syntax_tree_token_type op;

    switch (lexical_token->type) {
        case LEFT_PARENTHESIS:
            x = parenthesis_expression(fd);
            break;
        case MINUS:
        case PLUS:
            op = get_token_type(lexical_token->type);
            lexical_token = get_token(fd);
            node = expression(fd, attributes[SYN_TOKEN_NEGATE].precedence);
            x = (op == SYN_TOKEN_SUB) ? make_binary_node(SYN_NODE_NEGATE, node, NULL) : node;
            break;
        case IDENTIFIER:
            x = make_binary_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        case INTEGER:
            x = make_binary_leaf(SYN_NODE_INTEGER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        default: {
            SYNTAX_ERROR("Expected expression, got: %s\n", get_readable_error_char(lexical_token->value))
        }
    }


    while (attributes[get_token_type(lexical_token->type)].is_binary &&
           attributes[get_token_type(lexical_token->type)].precedence >= precedence) {
        syntax_tree_token_type internal_op = get_token_type(lexical_token->type);

        lexical_token = get_token(fd);

        int q = attributes[internal_op].precedence;
        if (!attributes[internal_op].right_associative) q++;

        node = expression(fd, q);
        x = make_binary_node(attributes[internal_op].node_type, x, node);
    }

    return x;
}

syntax_abstract_tree_t *args(FILE *fd) {
    syntax_abstract_tree_t *tree = NULL, *node;

    if (get_token_type(lexical_token->type) == SYN_TOKEN_RIGHT_PARENTHESIS) {
        lexical_token = get_token(fd);
        return NULL;
    }

    tree = make_binary_node(SYN_NODE_ARGS, expression(fd, 0), NULL);

    while (get_token_type(lexical_token->type) == SYN_TOKEN_COMMA) {
        expect_token("Comma", SYN_TOKEN_COMMA);
        lexical_token = get_token(fd);
        node = expression(fd, 0);
        tree = make_binary_node(SYN_NODE_ARGS, node, tree);
    }

    lexical_token = get_token(fd);

    return tree;
}

syntax_abstract_tree_t *stmt(FILE *fd) {
    syntax_abstract_tree_t *tree = NULL, *v, *e, *s, *s2;

    switch (lexical_token->type) {
        case IDENTIFIER: {
            v = make_binary_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            bool is_variable = lexical_token->value[0] == '$';
            lexical_token = get_token(fd);
            expect_token("Assignment", is_variable ? SYN_TOKEN_ASSIGN : SYN_TOKEN_LEFT_PARENTHESIS);
            lexical_token = get_token(fd);
            e = is_variable ? expression(fd, 0) : args(fd);
            tree = make_binary_node(is_variable ? SYN_NODE_ASSIGN : SYN_NODE_CALL, v, e);
            expect_token("Semicolon", SYN_TOKEN_SEMICOLON);
            // TODO: Move all semantic analysis to separate file
            find_token(v->value->value)->defined = true;
            if (!check_tree_using(tree, is_defined)) {
                syntax_abstract_tree_t *undefined_node = get_from_tree_using(tree, is_undefined);
                if (is_variable) {
                    SEMANTIC_UNDEF_VAR_ERROR("Variable %s used before declaration", undefined_node->value->value)
                } else {
                    SEMANTIC_UNDEF_FUNC_ERROR("Function %s used before declaration", undefined_node->value->value)
                }
            }
            lexical_token = get_token(fd);
            break;
        }
        case INTEGER: {
            tree = expression(fd, 0);
            expect_token("Semicolon", SYN_TOKEN_SEMICOLON);
            lexical_token = get_token(fd);
            break;
        }
        case KEYWORD_IF: {
            lexical_token = get_token(fd);
            e = parenthesis_expression(fd);
            s = stmt(fd);
            s2 = NULL;
            if (lexical_token->type == KEYWORD_ELSE) {
                lexical_token = get_token(fd);
                s2 = stmt(fd);

                if (s2 == NULL) {
                    SYNTAX_ERROR("Expected statement after else\n")
                }
            }
            tree = make_ternary_node(SYN_NODE_KEYWORD_IF, e, s, s2);
            if (!check_tree_using(tree, is_correct_if)) {
                SYNTAX_ERROR("Incorrect if statement\n")
            }
            break;
        }
        case KEYWORD_WHILE: {
            lexical_token = get_token(fd);
            e = parenthesis_expression(fd);
            s = stmt(fd);
            tree = make_binary_node(SYN_NODE_KEYWORD_WHILE, e, s);
            if (s == NULL) {
                SYNTAX_ERROR("Expected statement after while\n")
            }
            break;
        }
        case KEYWORD_FUNCTION: {
            tree = f_dec_stats(fd);
            break;
        }
        case LEFT_CURLY_BRACKETS: {
            expect_token("Left curly brackets", SYN_TOKEN_LEFT_CURLY_BRACKETS);
            lexical_token = get_token(fd);
            while (lexical_token->type != RIGHT_CURLY_BRACKETS && lexical_token->type != END_OF_FILE) {
                tree = make_binary_node(SYN_NODE_SEQUENCE, tree, stmt(fd));
            }
            expect_token("Right curly brackets", SYN_TOKEN_RIGHT_CURLY_BRACKETS);
            lexical_token = get_token(fd);
            break;
        }
        case END_OF_FILE: {
            break;
        }
        default: {
            SYNTAX_ERROR("Expected statement, got: %s\n", lexical_token->value)
        }
    }

    return tree;
}

syntax_abstract_tree_t *load_syntax_tree(FILE *fd) {
    lexical_token = get_token(fd);

    syntax_abstract_tree_t *tree = NULL;

    while (lexical_token->type != END_OF_FILE) {
        tree = make_binary_node(SYN_NODE_SEQUENCE, tree, stmt(fd));
    }

    return tree;
}

syntax_tree_token_type get_token_type(LEXICAL_FSM_TOKENS token) {
    switch (token) {
        case END_OF_FILE:
            return SYN_TOKEN_EOF;
        case IDENTIFIER:
            return SYN_TOKEN_IDENTIFIER;
        case STRING:
            return SYN_TOKEN_STRING;
        case INTEGER:
            return SYN_TOKEN_INTEGER;
        case FLOAT:
            return SYN_TOKEN_FLOAT;
        case PLUS:
            return SYN_TOKEN_ADD;
        case MINUS:
            return SYN_TOKEN_SUB;
        case MULTIPLY:
            return SYN_TOKEN_MUL;
        case DIVIDE:
            return SYN_TOKEN_DIV;
        case INCREMENT:
        SYNTAX_ERROR("Increment operator is not supported")
        case DECREMENT:
        SYNTAX_ERROR("Decrement operator is not supported")
        case LESS:
            return SYN_TOKEN_LESS;
        case LESS_EQUAL:
            return SYN_TOKEN_LESS_EQUAL;
        case GREATER:
            return SYN_TOKEN_GREATER;
        case GREATER_EQUAL:
            return SYN_TOKEN_GREATER_EQUAL;
        case EQUAL:
            return SYN_TOKEN_EQUAL;
        case NOT_EQUAL:
            return SYN_TOKEN_NOT_EQUAL;
        case ASSIGN:
            return SYN_TOKEN_ASSIGN;
        case SEMICOLON:
            return SYN_TOKEN_SEMICOLON;
        case COLON:
            return SYN_TOKEN_COLON;
        case COMMA:
            return SYN_TOKEN_COMMA;
        case LEFT_PARENTHESIS:
            return SYN_TOKEN_LEFT_PARENTHESIS;
        case RIGHT_PARENTHESIS:
            return SYN_TOKEN_RIGHT_PARENTHESIS;
        case LEFT_CURLY_BRACKETS:
            return SYN_TOKEN_LEFT_CURLY_BRACKETS;
        case RIGHT_CURLY_BRACKETS:
            return SYN_TOKEN_RIGHT_CURLY_BRACKETS;
        case KEYWORD_IF:
            return SYN_TOKEN_KEYWORD_IF;
        case KEYWORD_ELSE:
            return SYN_TOKEN_KEYWORD_ELSE;
        case KEYWORD_FUNCTION:
            return SYN_TOKEN_KEYWORD_FUNCTION;
        case KEYWORD_INTEGER:
            return SYN_TOKEN_KEYWORD_INT;
        case KEYWORD_FLOAT:
            return SYN_TOKEN_KEYWORD_FLOAT;
        case KEYWORD_STRING:
            return SYN_TOKEN_KEYWORD_STRING;
        case KEYWORD_VOID:
            return SYN_TOKEN_KEYWORD_VOID;
        default:
            return (syntax_tree_token_type) -1;
    }
}


