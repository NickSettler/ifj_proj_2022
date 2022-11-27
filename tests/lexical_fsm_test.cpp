#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwritable-strings"
extern "C" {
#include "../src/lexical_fsm.h"
#include "../src/lexical_fsm.c"
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
                    if (fd != nullptr) {
                        fclose(fd);
                    }
                }

                void SetUp() override {
                }

                void TearDown() override {
                    dispose_symtable();
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

            TEST_F(LexicalAnalyzerTest, PHPBrackets) {
                IsStackCorrect("<?PHP ?>", 2,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {CLOSE_PHP_BRACKET, "?>"}
                );

                IsStackCorrect("<?php  "
                               "$test = \"abc\""
                               " ?>", 5,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {IDENTIFIER, "$test"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"abc\""},
                               (lexical_token_t) {CLOSE_PHP_BRACKET, "?>"}
                );

                IsStackCorrect("<? "
                               "$test = \"abc\""
                               " ?>", 5,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?"},
                               (lexical_token_t) {IDENTIFIER, "$test"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"abc\""},
                               (lexical_token_t) {CLOSE_PHP_BRACKET, "?>"}
                );

                EXPECT_EXIT({
                                IsStackCorrect("<?p  "
                                               "$test = \"abc\""
                                               " ?>", 5,
                                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?p"},
                                               (lexical_token_t) {IDENTIFIER, "$test"},
                                               (lexical_token_t) {ASSIGN, "="},
                                               (lexical_token_t) {STRING, "\"abc\""},
                                               (lexical_token_t) {CLOSE_PHP_BRACKET, "?>"}
                                );
                            }, ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid PHP open bracket");
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
                IsStackCorrect("!$a;", 3,
                               (lexical_token_t) {LOGICAL_NOT, "!"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
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
                IsStackCorrect("( \"a\" * 4 == \"aaaa\")&& (2 + $b===7.5 );", 16,
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {STRING, "\"a\""},
                               (lexical_token_t) {MULTIPLY, "*"},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {EQUAL, "=="},
                               (lexical_token_t) {STRING, "\"aaaa\""},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LOGICAL_AND, "&&"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {INTEGER, "2"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {TYPED_EQUAL, "==="},
                               (lexical_token_t) {FLOAT, "7.5"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, IfElseConditions) {
                IsStackCorrect("if($a === 4) {"
                               "   $b = 5;"
                               "}", 12,
                               (lexical_token_t) {KEYWORD_IF, "if"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {TYPED_EQUAL, "==="},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );
                IsStackCorrect("$cond= $a ==4 &&$c===\"123\";"
                               "if ($cond) {"
                               "   $b = 5;"
                               "}else if ($a === 2.13) {"
                               "   $b = 6.1;"
                               "} else{"
                               "   $b = \"74\";"
                               "}", 40,
                               (lexical_token_t) {IDENTIFIER, "$cond"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {EQUAL, "=="},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {LOGICAL_AND, "&&"},
                               (lexical_token_t) {IDENTIFIER, "$c"},
                               (lexical_token_t) {TYPED_EQUAL, "==="},
                               (lexical_token_t) {STRING, "\"123\""},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {KEYWORD_IF, "if"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$cond"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"},
                               (lexical_token_t) {KEYWORD_ELSE, "else"},
                               (lexical_token_t) {KEYWORD_IF, "if"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {TYPED_EQUAL, "==="},
                               (lexical_token_t) {FLOAT, "2.13"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {FLOAT, "6.1"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"},
                               (lexical_token_t) {KEYWORD_ELSE, "else"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"74\""},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );
            }

            TEST_F(LexicalAnalyzerTest, Loops) {
                IsStackCorrect("while ( $str1 !== \"abcdefgh\") {\n"
                               "    write(\"Wrong string\\n\"); "
                               "    $str1 = reads();"
                               "}", 19,
                               (lexical_token_t) {KEYWORD_WHILE, "while"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$str1"},
                               (lexical_token_t) {TYPED_NOT_EQUAL, "!=="},
                               (lexical_token_t) {STRING, "\"abcdefgh\""},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "write"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {STRING, "\"Wrong string\\n\""},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {IDENTIFIER, "$str1"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {IDENTIFIER, "reads"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );
            }

            TEST_F(LexicalAnalyzerTest, Functions) {
                IsStackCorrect("function factorial(int $n) : void {"
                               "return null;"
                               "}", 13,
                               (lexical_token_t) {KEYWORD_FUNCTION, "function"},
                               (lexical_token_t) {IDENTIFIER, "factorial"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {KEYWORD_INTEGER, "int"},
                               (lexical_token_t) {IDENTIFIER, "$n"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {COLON, ":"},
                               (lexical_token_t) {KEYWORD_VOID, "void"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {KEYWORD_RETURN, "return"},
                               (lexical_token_t) {KEYWORD_NULL, "null"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );
            }

            TEST_F(LexicalAnalyzerTest, CaseSensitiveKeywords) {
                IsStackCorrect("Function foo(int $a){"
                               "    $a = NuLl;"
                               "    IF ($a == NULL) {"
                               "    $a = 5;}", 23,
                               (lexical_token_t) {KEYWORD_FUNCTION, "function"},
                               (lexical_token_t) {IDENTIFIER, "foo"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {KEYWORD_INTEGER, "int"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {KEYWORD_NULL, "null"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {KEYWORD_IF, "if"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {EQUAL, "=="},
                               (lexical_token_t) {KEYWORD_NULL, "null"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );
            }

            TEST_F(LexicalAnalyzerTest, NegativeExpressions) {
                IsStackCorrect("$a = -4 + $b", 6,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {IDENTIFIER, "$b"}
                );
                IsStackCorrect("$a = -sum(1, $c) + $b", 11,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {IDENTIFIER, "sum"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {COMMA, ","},
                               (lexical_token_t) {IDENTIFIER, "$c"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {IDENTIFIER, "$b"}
                );
                IsStackCorrect("$a = -(4 + $b) + (-(-func1()+(-2 * sum($c, 2))))", 30,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {INTEGER, "4"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {IDENTIFIER, "func1"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {PLUS, "+"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {MINUS, "-"},
                               (lexical_token_t) {INTEGER, "2"},
                               (lexical_token_t) {MULTIPLY, "*"},
                               (lexical_token_t) {IDENTIFIER, "sum"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "$c"},
                               (lexical_token_t) {COMMA, ","},
                               (lexical_token_t) {INTEGER, "2"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"}

                );
            }

            TEST_F(LexicalAnalyzerTest, Comments) {
                IsStackCorrect("$a = 5 // double-slash comment\n"
                               "$b = 6 /* comment\n new-line comment*/\n"
                               "$c = 7 # sharp comment", 9,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "5"},
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "6"},
                               (lexical_token_t) {IDENTIFIER, "$c"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "7"}
                );
            }

            TEST_F(LexicalAnalyzerTest, IntegerType) {
                IsStackCorrect("$b = 62345;"
                               " 42  ", 5,
                               (lexical_token_t) {IDENTIFIER, "$b"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "62345"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {INTEGER, "42"}

                );

                EXPECT_EXIT({
                                IsStackCorrect("$a = 5f3;"
                                               "1+4", 7,
                                               (lexical_token_t) {IDENTIFIER, "$a"},
                                               (lexical_token_t) {ASSIGN, "="},
                                               (lexical_token_t) {INTEGER, "5f3"},
                                               (lexical_token_t) {SEMICOLON, ";"},
                                               (lexical_token_t) {INTEGER, "1"},
                                               (lexical_token_t) {PLUS, "+"},
                                               (lexical_token_t) {INTEGER, "4"}

                                );
                            }, ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid integer number format");
            }

            TEST_F(LexicalAnalyzerTest, FloatType) {
                IsStackCorrect("23.56e1;"
                               "12E+24;", 4,
                               (lexical_token_t) {FLOAT, "23.56e1"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {FLOAT, "12E+24"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
                EXPECT_EXIT({
                                IsStackCorrect("23.5612e;"
                                               "12.2.2;"
                                               "56.e2", 5,
                                               (lexical_token_t) {FLOAT, "23.5612e"},
                                               (lexical_token_t) {SEMICOLON, ";"},
                                               (lexical_token_t) {FLOAT, "12.2.2"},
                                               (lexical_token_t) {SEMICOLON, ";"},
                                               (lexical_token_t) {FLOAT, "56.e2"}
                                );
                            }, ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid float number format");

            }

            TEST_F(LexicalAnalyzerTest, StringType) {
                IsStackCorrect("$a = \"abc\";", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"abc\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("$a = \'testing\';", 4,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\'testing\'"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, Concatenation) {
                IsStackCorrect("$a = \"Hello \" . \"world\";", 6,
                               (lexical_token_t) {IDENTIFIER, "$a"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"Hello \""},
                               (lexical_token_t) {CONCATENATION, "."},
                               (lexical_token_t) {STRING, "\"world\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("\"hello\".\"world\";", 4,
                               (lexical_token_t) {STRING, "\"hello\""},
                               (lexical_token_t) {CONCATENATION, "."},
                               (lexical_token_t) {STRING, "\"world\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, StrictTypes) {
                IsStackCorrect("declare(strict_types=1);", 7,
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );
            }

            TEST_F(LexicalAnalyzerTest, InvalidArithmeticOperators) {
                EXPECT_EXIT({
                                IsStackCorrect("$a */ $b"
                                               "$c +* $d"
                                               "$e -*- $f", 9,
                                               (lexical_token_t) {IDENTIFIER, "$a"},
                                               (lexical_token_t) {MULTIPLY, "*"},
                                               (lexical_token_t) {IDENTIFIER, "$b"},
                                               (lexical_token_t) {IDENTIFIER, "$c"},
                                               (lexical_token_t) {PLUS, "+"},
                                               (lexical_token_t) {IDENTIFIER, "$d"},
                                               (lexical_token_t) {IDENTIFIER, "$e"},
                                               (lexical_token_t) {MINUS, "-"},
                                               (lexical_token_t) {IDENTIFIER, "$f"}
                                );
                            }, ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid arithmetic operator");
            }

            TEST_F(LexicalAnalyzerTest, ExternalTests) {
                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);\n"
                               "/* hello there!!!\n"
                               "\n"
                               ":)\n"
                               "\n"
                               "\n"
                               "*/", 8,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                EXPECT_EXIT(IsStackCorrect("<?php\n"
                                           "declare(strict_types=1);\n"
                                           "/* hello there!!!\n"
                                           "\n"
                                           ":)\n"
                                           "", 0),
                            ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Unexpected end of file");

                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);\n"
                               "$x = 04534.1543210e+5655;", 12,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {IDENTIFIER, "$x"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {FLOAT, "04534.1543210e+5655"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                EXPECT_EXIT(IsStackCorrect("<?php\n"
                                           "declare(strict_types=1);\n"
                                           "$x = 0.;", 0),
                            ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid float number");

                EXPECT_EXIT(IsStackCorrect("<?php\n"
                                           "declare(strict_types=1);\n"
                                           "$x = 0.0e;", 0),
                            ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid float number");

                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);\n"
                               "// hello there!!!", 8,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);\n"
                               "while(1!==1){}", 16,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {KEYWORD_WHILE, "while"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {TYPED_NOT_EQUAL, "!=="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {LEFT_CURLY_BRACKETS, "{"},
                               (lexical_token_t) {RIGHT_CURLY_BRACKETS, "}"}
                );

                IsStackCorrect("<?php//this is opening tag, it is needed for ever php script\n"
                               "/* this is something that is needed for compability with IFJ code */declare(/*some parameter here*//*aaaaaaa*/strict_types=/*:)*/1);//bye",
                               8,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);", 8,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("<?php\n"
                               "\n"
                               "\n"
                               "\n"
                               "declare(         strict_types   =\n"
                               "1       )\n"
                               ";", 8,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                IsStackCorrect("<?php\n"
                               "declare(strict_types=1);\n"
                               "$x = \"Hello world\";", 12,
                               (lexical_token_t) {OPEN_PHP_BRACKET, "<?php"},
                               (lexical_token_t) {KEYWORD_DECLARE, "declare"},
                               (lexical_token_t) {LEFT_PARENTHESIS, "("},
                               (lexical_token_t) {IDENTIFIER, "strict_types"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {INTEGER, "1"},
                               (lexical_token_t) {RIGHT_PARENTHESIS, ")"},
                               (lexical_token_t) {SEMICOLON, ";"},
                               (lexical_token_t) {IDENTIFIER, "$x"},
                               (lexical_token_t) {ASSIGN, "="},
                               (lexical_token_t) {STRING, "\"Hello world\""},
                               (lexical_token_t) {SEMICOLON, ";"}
                );

                EXPECT_EXIT(IsStackCorrect("<?php\n"
                                           "declare(strict_types=1);\n"
                                           "$x = \"Hello world;", 0),
                            ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Invalid string format");

                EXPECT_EXIT(IsStackCorrect("<?php\n"
                                           "declare(strict_types=1);\n"
                                           "@", 0),
                            ::testing::ExitedWithCode(LEXICAL_ERROR_CODE),
                            "\\[LEXICAL ERROR\\] Unexpected character: @");
            }
        }
    }
}

#pragma clang diagnostic pop