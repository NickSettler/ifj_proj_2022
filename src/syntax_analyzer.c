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
        {"EOF",     "End_of_file",       SYN_TOKEN_EOF,                  false, false, false, -1, (syntax_tree_node_type) -1},
        {"ID",      "Identifier",        SYN_TOKEN_IDENTIFIER,           false, false, false, -1, SYN_NODE_IDENTIFIER},
        {"STRING",  "String",            SYN_TOKEN_STRING,               false, false, false, -1, SYN_NODE_STRING},
        {"INTEGER", "Integer",           SYN_TOKEN_INTEGER,              false, false, false, -1, SYN_NODE_INTEGER},
        {"FLOAT",   "Float",             SYN_TOKEN_FLOAT,                false, false, false, -1, SYN_NODE_FLOAT},
        {"+",       "Op_add",            SYN_TOKEN_ADD,                  false, true,  false, 12, SYN_NODE_ADD},
        {"-",       "Op_sub",            SYN_TOKEN_SUB,                  false, true,  false, 12, SYN_NODE_SUB},
        {"*",       "Op_multiply",       SYN_TOKEN_MUL,                  false, true,  false, 13, SYN_NODE_MUL},
        {"/",       "Op_divide",         SYN_TOKEN_DIV,                  false, true,  false, 13, SYN_NODE_DIV},
        {"-",       "Op_negate",         SYN_TOKEN_NEGATE,               false, false, true,  14, SYN_NODE_NEGATE},
        {"<",       "Op_less",           SYN_TOKEN_LESS,                 false, true,  false, 10, SYN_NODE_LESS},
        {"<=",      "Op_lessequal",      SYN_TOKEN_LESS_EQUAL,           false, true,  false, 10, SYN_NODE_LESS_EQUAL},
        {">",       "Op_greater",        SYN_TOKEN_GREATER,              false, true,  false, 10, SYN_NODE_GREATER},
        {">=",      "Op_greaterequal",   SYN_TOKEN_GREATER_EQUAL,        false, true,  false, 10, SYN_NODE_GREATER_EQUAL},
        {"==",      "Op_equal",          SYN_TOKEN_EQUAL,                false, true,  false, 9,  SYN_NODE_EQUAL},
        {"!=",      "Op_not_equal",      SYN_TOKEN_NOT_EQUAL,            false, true,  false, 9,  SYN_NODE_NOT_EQUAL},
        {"=",       "Op_Assign",         SYN_TOKEN_ASSIGN,               false, false, false, -1, SYN_NODE_ASSIGN},
        {";",       "Semicolon",         SYN_TOKEN_SEMICOLON,            false, false, false, -1, (syntax_tree_node_type) -1},
        {"if",      "Keyword_IF",        SYN_TOKEN_KEYWORD_IF,           false, false, false, -1, SYN_NODE_KEYWORD_IF},
        {"(",       "LeftParenthesis",   SYN_TOKEN_LEFT_PARENTHESIS,     false, false, false, -1, (syntax_tree_node_type) -1},
        {")",       "RightParenthesis",  SYN_TOKEN_RIGHT_PARENTHESIS,    false, false, false, -1, (syntax_tree_node_type) -1},
        {"{",       "LeftCurlyBracket",  SYN_TOKEN_RIGHT_CURLY_BRACKETS, false, false, false, -1, (syntax_tree_node_type) -1},
        {"}",       "RightCurlyBracket", SYN_TOKEN_RIGHT_CURLY_BRACKETS, false, false, false, -1, (syntax_tree_node_type) -1}
};

syntax_abstract_tree_t *
make_node(syntax_tree_node_type type, syntax_abstract_tree_t *left, syntax_abstract_tree_t *right) {
    syntax_abstract_tree_t *tree = (syntax_abstract_tree_t *) malloc(sizeof(syntax_abstract_tree_t));
    if (tree == NULL) {
        INTERNAL_ERROR("Failed to allocate memory for syntax tree node");
    }

    tree->type = type;
    tree->left = left;
    tree->right = right;

    return tree;
}

syntax_abstract_tree_t *make_leaf(syntax_tree_node_type type, string_t *value) {
    syntax_abstract_tree_t *tree = make_node(type, NULL, NULL);

    tree->value = value;

    return tree;
}

void syntax_abstract_tree_print(FILE *output, syntax_abstract_tree_t *tree) {
    if (!tree) return;

    syntax_abstract_tree_print(output, tree->left);
    fprintf(output, "%d ", tree->type);
    syntax_abstract_tree_print(output, tree->right);
}

bool check_tree_using(syntax_abstract_tree_t *tree, bool (*check)(syntax_abstract_tree_t *)) {
    if (!tree) return true;
    return check(tree) && check_tree_using(tree->left, check) && check_tree_using(tree->right, check);
}

bool is_leaf(syntax_abstract_tree_t *tree) {
    return tree->left == NULL && tree->right == NULL;
}

bool is_defined(syntax_abstract_tree_t *tree) {
    if (tree->type == SYN_NODE_IDENTIFIER) {
        return find_element(symtable, tree->value->value)->defined;
    }
    return true;
}

void expect_token(const char *msg, syntax_tree_token_type type) {
    if (get_token_type(lexical_token->type) != type) {
        SYNTAX_ERROR("%s Expecting %s, found: %s\n", msg, attributes[type].text,
                     attributes[get_token_type(lexical_token->type)].text);
    }
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

    switch (lexical_token->type) {
        case LEFT_PARENTHESIS:
            x = parenthesis_expression(fd);
            break;
        case IDENTIFIER:
            x = make_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        case INTEGER:
            x = make_leaf(SYN_NODE_INTEGER, string_init(lexical_token->value));
            lexical_token = get_token(fd);
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s\n", lexical_token->value);
    }


    while (attributes[get_token_type(lexical_token->type)].is_binary &&
           attributes[get_token_type(lexical_token->type)].precedence >= precedence) {
        syntax_tree_token_type op = get_token_type(lexical_token->type);

        lexical_token = get_token(fd);

        int q = attributes[op].precedence;
        if (!attributes[op].right_associative) {
            q++;
        }

        node = expression(fd, q);
        x = make_node(attributes[op].node_type, x, node);
    }

    return x;
}

syntax_abstract_tree_t *stmt(FILE *fd) {
    syntax_abstract_tree_t *tree = NULL, *v, *e, *s, *s2;

    switch (lexical_token->type) {
        case IDENTIFIER:
            v = make_leaf(SYN_NODE_IDENTIFIER, string_init(lexical_token->value));
            find_element(symtable, lexical_token->value)->defined = true;
            lexical_token = get_token(fd);
            expect_token("Expected assignment operator", SYN_TOKEN_ASSIGN);
            lexical_token = get_token(fd);
            e = expression(fd, 0);
            tree = make_node(SYN_NODE_ASSIGN, v, e);
            expect_token("Expected semicolon", SYN_TOKEN_SEMICOLON);
            if (!check_tree_using(tree, is_defined)) {
                SEMANTIC_UNDEF_VAR_ERROR("Variable used before definition")
            }
            lexical_token = get_token(fd);
            break;
        case INTEGER:
            tree = expression(fd, 0);
            expect_token("Expected semicolon", SYN_TOKEN_SEMICOLON);
            lexical_token = get_token(fd);
            break;
        case KEYWORD_IF:
            lexical_token = get_token(fd);
            e = parenthesis_expression(fd);
            s = stmt(fd);
            tree = make_node(SYN_NODE_KEYWORD_IF, e, s);
            break;
        case LEFT_CURLY_BRACKETS:
            expect_token("Right curly brackets", SYN_TOKEN_LEFT_CURLY_BRACKETS);
            lexical_token = get_token(fd);
            while (lexical_token->type != RIGHT_CURLY_BRACKETS && lexical_token->type != END_OF_FILE) {
                tree = make_node(SYN_NODE_SEQUENCE, tree, stmt(fd));
            }
            expect_token("Right curly brackets", SYN_TOKEN_RIGHT_CURLY_BRACKETS);
            lexical_token = get_token(fd);
            break;
        case END_OF_FILE:
            break;
        default:
        SYNTAX_ERROR("Expected expression, got: %s\n", lexical_token->value);
    }

    bool is_root_leaf = check_tree_using(tree, is_leaf);
    bool is_left_leaf = check_tree_using(tree->left, is_leaf);

    return tree;
}

syntax_abstract_tree_t *load_syntax_tree(FILE *fd) {
    lexical_token = get_token(fd);

    syntax_abstract_tree_t *tree = NULL;

    while (lexical_token->type != END_OF_FILE) {
        tree = make_node(SYN_NODE_SEQUENCE, tree, stmt(fd));
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
        default:
            return (syntax_tree_token_type) -1;
    }
}

