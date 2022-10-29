#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "../src/syntax_analyzer.h"
#include "../src/syntax_analyzer.c"
}


namespace ifj {
    namespace tests {
        namespace {
            class SyntaxAnalyzerTest : public ::testing::Test {
            protected:
                FILE *output_fd{};

                SyntaxAnalyzerTest() = default;

                ~SyntaxAnalyzerTest() override {
                    fclose(output_fd);
                }

                void IsSyntaxTreeCorrect(char *input, int n, ...) {
                    string_t *expected_str = string_init("");

                    va_list args;
                    va_start(args, n);

                    for (int i = 0; i < n; i++) {
                        int type = va_arg(args, int);
                        string_append_string(expected_str, "%d", type);
                        string_append_char(expected_str, ' ');
                    }

                    va_end(args);

                    char *actual = (char *) malloc(1000);
                    output_fd = fmemopen(actual, 1000, "w");
                    setbuffer(output_fd, actual, 1000);

                    syntax_abstract_tree_t *tree = load_syntax_tree(test_lex_input(input));
                    syntax_abstract_tree_print(output_fd, tree);

                    EXPECT_STREQ(expected_str->value, actual);
                }
            };

            TEST_F(SyntaxAnalyzerTest, Assignment) {
                IsSyntaxTreeCorrect("$a = 12 + 32;", 6,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_ADD, SYN_NODE_INTEGER);
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionBasic) {
//                IsSyntaxTreeCorrect("1 + 2;", {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("4 - $a;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});
//
//                IsSyntaxTreeCorrect("12 * 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("33 / 11;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionAdvanced) {
//                IsSyntaxTreeCorrect("1 + 2 * 3 / 12;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_MUL,
//                                     SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("$a = (1 + 2) / (3 * 4);",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
//                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER, SYN_NODE_MUL,
//                                     SYN_NODE_INTEGER});
            }

            TEST_F(SyntaxAnalyzerTest, IfConditions) {
//                IsSyntaxTreeCorrect("if ($a == 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if (1 != 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_NOT_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if (1 < 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if (1 > 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if (1 <= 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if (1 >= 2) $a = 3;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if ($a == 3) if ($b != 2) $c = 4;",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_NOT_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
//
//                IsSyntaxTreeCorrect("if ($a == 4) {"
//                                    "  $b = 1;"
//                                    "  $c = 2;"
//                                    "}",
//                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
//                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
//                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
//                                     SYN_NODE_INTEGER});
            }
        }
    }
}