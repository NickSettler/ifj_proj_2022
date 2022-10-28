#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

                void IsSyntaxTreeCorrect(char *input, int n, ...) {
                    input_fd = fmemopen(input, strlen(input), "r");

                    char output[n + 1];
                    output_fd = fmemopen(output, sizeof(output) / sizeof(char), "w");

                    string_t *check_output = string_init("");

                    va_list tokens;
                    va_start(tokens, n);

                    for (int i = 0; i < n; i++) {
                        int arg_token = va_arg(tokens, int);
                        string_append_char(check_output, arg_token + '0');
                    }

                    va_end(tokens);

                    syntax_abstract_tree_t *tree = load_syntax_tree(input_fd);
                    syntax_abstract_tree_print(output_fd, tree);

                    EXPECT_STREQ(check_output->value, output);
                }
            };

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionBasic) {
                IsSyntaxTreeCorrect("1 + 2;", 4, SYN_NODE_SEQUENCE, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_INTEGER);

                IsSyntaxTreeCorrect("4 - $a;", 4, SYN_NODE_SEQUENCE, SYN_NODE_SUB, SYN_NODE_INTEGER,
                                    SYN_NODE_IDENTIFIER);

                IsSyntaxTreeCorrect("12 * 3;", 4, SYN_NODE_SEQUENCE, SYN_NODE_MUL, SYN_NODE_INTEGER, SYN_NODE_INTEGER);

                IsSyntaxTreeCorrect("33 / 11;", 4, SYN_NODE_SEQUENCE, SYN_NODE_DIV, SYN_NODE_INTEGER, SYN_NODE_INTEGER);
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionAdvanced) {
                IsSyntaxTreeCorrect("1 + 2 * 3 / 12;", 8, SYN_NODE_SEQUENCE, SYN_NODE_ADD, SYN_NODE_INTEGER,
                                    SYN_NODE_DIV, SYN_NODE_MUL, SYN_NODE_INTEGER, SYN_NODE_INTEGER, SYN_NODE_INTEGER);
            }
        }
    }
}