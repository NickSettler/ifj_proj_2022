#include <gtest/gtest.h>

extern "C" {
#include "../src/symtable.h"
#include "../src/symtable.c"
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
                    if (output_fd) {
                        fclose(output_fd);
                    }
                }

                void TearDown() override {
                    dispose_symtable();
                }

                void IsSyntaxTreeCorrect(const std::string &input, const std::vector<int> &expected_output) {
                    std::string expected_str;

                    for (auto &str: expected_output)
                        expected_str += std::to_string(str) + " ";

                    char *actual = (char *) malloc(expected_str.length() + 1);
                    output_fd = fmemopen(actual, expected_str.length() + 1, "w");

                    syntax_abstract_tree_t *tree = load_syntax_tree(test_lex_input((char *) input.c_str()));
                    syntax_abstract_tree_print(output_fd, tree);

                    fseek(output_fd, 0, SEEK_SET);
                    fread(actual, expected_str.length() + 1, 1, output_fd);

                    EXPECT_STREQ(expected_str.c_str(), actual);
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