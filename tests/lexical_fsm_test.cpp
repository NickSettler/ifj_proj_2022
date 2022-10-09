#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern "C" {
#include "../src/lexical_fsm.h"
#include "../src/lexical_fsm.c"
#include "../src/lexical_token.h"
#include "../src/lexical_token.c"
}

namespace ifj {
    namespace tests {
        namespace {
            class LexicalAnalyzerTest : public ::testing::Test {
            protected:
                lexical_token_stack_t *token_stack;
                string_t *token;
                FILE *fd{};

                LexicalAnalyzerTest() {
                    token_stack = lexical_token_stack_init();
                    token = string_init("");
                }

                ~LexicalAnalyzerTest() override {
                    fclose(fd);
                }

                void SetUp() override {
                }

                void IsStackCorrect(char *input, int n, ...) {
                    lexical_token_stack_t *correct_stack = lexical_token_stack_init();
                    va_list tokens;
                    va_start(tokens, n);

                    for (int i = 0; i < n; i++) {
                        lexical_token_t arg_token = va_arg(tokens, lexical_token_t);
                        lexical_token_stack_push(correct_stack, arg_token);
                    }

                    va_end(tokens);

                    fd = fmemopen(input, strlen(input), "r");
                    LEXICAL_FSM_TOKENS token_type = get_next_token(fd, token);

                    while (token_type != END_OF_FILE) {
                        string_t *token_value = string_init(token->value);
                        lexical_token_stack_push(token_stack, (lexical_token_t) {token_type, token_value->value});
                        token_type = get_next_token(fd, token);
                    }

                    while (!lexical_token_stack_empty(token_stack)) {
                        lexical_token_t input_token = lexical_token_stack_pop(token_stack);
                        lexical_token_t correct_token = lexical_token_stack_pop(correct_stack);
                        EXPECT_EQ(input_token.type, correct_token.type);
                        EXPECT_STREQ(input_token.value, correct_token.value);
                    }
                }
            };

            TEST_F(LexicalAnalyzerTest, VariableDeclaration) {
                IsStackCorrect("int $a = 5;", 5,
                               (lexical_token_t) {KEYWORD_INTEGER, "int"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("?int $a = 5;", 5,
                               (lexical_token_t) {KEYWORD_INTEGER, "?int"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a = 5;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("string $a = \"abcdef\";", 5,
                               (lexical_token_t) {KEYWORD_STRING, "string"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"abcdef\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("float $a = 12.33;", 5,
                               (lexical_token_t) {KEYWORD_FLOAT, "float"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {FLOAT, "12.33"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, ArithmeticOperators) {
                IsStackCorrect("$a+3;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {INTEGER, "3"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("5 - 3;", 4,
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {INTEGER, "3"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$b * 3.23;", 4,
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {MULTIPLY, "*"},
                               (lexical_token_t) {FLOAT, "3.23"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$b / $a;", 4,
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {DIVIDE, "/"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, EqualOperators) {
                IsStackCorrect("$a == $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {EQUAL, "=="},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a=== 2;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {TYPED_EQUAL, "==="},
                               (lexical_token_t) {INTEGER, "2"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a != 4;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {NOT_EQUAL, "!="},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a !== \"abc\";", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {TYPED_NOT_EQUAL, "!=="},
                               (lexical_token_t) {STRING, "\"abc\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, LogicalOperators) {
                IsStackCorrect("$a && $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {LOGICAL_AND, "&&"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a || $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {LOGICAL_OR, "||"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, RelationalOperators) {
                IsStackCorrect("$a < $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {LESS, "<"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a > $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {GREATER, ">"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a <= $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {LESS_EQUAL, "<="},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                IsStackCorrect("$a >= $b;", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {GREATER_EQUAL, ">="},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, ArithmeticExpressions) {
                IsStackCorrect(" (1 + $b ) /3.25;", 8,
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {DIVIDE, "/"},
                               (lexical_token_t) {FLOAT, "3.25"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
//                IsStackCorrect("( \"string\" * 4) &&", 8,
//                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
//                               (lexical_token_t) {INTEGER, "1"},
//                               (lexical_token_t) {PLUS, "+"},
//                               (lexical_token_t) {IDENTIFIER, "$b"},
//                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
//                               (lexical_token_t) {DIVIDE, "/"},
//                               (lexical_token_t) {FLOAT, "3.25"},
//                               (lexical_token_t) {SEMICOLON, ";"}
//                );
            }
        }
    }
}
