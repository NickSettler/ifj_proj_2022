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
                FILE *input_fd{};
                FILE *output_fd{};

                SyntaxAnalyzerTest() = default;

                ~SyntaxAnalyzerTest() override {
                    fclose(input_fd);
                }

                void IsSyntaxTreeCorrect(char *input, const std::vector<int> &expected) {
                    input_fd = fmemopen(input, strlen(input), "r");

                    char output[100];
                    output_fd = fmemopen(output, sizeof(output) / sizeof(char), "w");

                    std::string expected_str;

                    for (auto &i: expected)
                        expected_str += std::to_string(i) + " ";


                    syntax_abstract_tree_t *tree = load_syntax_tree(input_fd);
                    syntax_abstract_tree_print(output_fd, tree);

                    EXPECT_STREQ(expected_str.c_str(), output);
                }
            };

            TEST_F(SyntaxAnalyzerTest, Assignment) {
                IsSyntaxTreeCorrect("$a = 12 + 32;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER});
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionBasic) {
                IsSyntaxTreeCorrect("1 + 2;", {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("4 - $a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("12 * 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("33 / 11;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionAdvanced) {
                IsSyntaxTreeCorrect("1 + 2 * 3 / 12;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("$a = (1 + 2) / (3 * 4);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER});
            }

            TEST_F(SyntaxAnalyzerTest, IfConditions) {
                IsSyntaxTreeCorrect("if ($a == 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if (1 != 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_NOT_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if (1 < 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if (1 > 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if (1 <= 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if (1 >= 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if ($a == 3) if ($b != 2) $c = 4;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_NOT_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if ($a == 4) {"
                                    "  $b = 1;"
                                    "  $c = 2;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});
            }
        }
    }
}