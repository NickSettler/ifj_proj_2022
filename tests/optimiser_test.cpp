#include <gtest/gtest.h>

extern "C" {
#include "../src/errors.h"
#include "../src/symtable.h"
#include "../src/syntax_analyzer.h"
#include "../src/optimiser.h"
#include "../src/optimiser.c"
}


namespace ifj {
    namespace tests {
        namespace {
            class OptimiserTest : public ::testing::Test {
            protected:
                FILE *output_fd{};

                OptimiserTest() = default;

                ~OptimiserTest() override {
                    if (output_fd) {
                        fclose(output_fd);
                    }
                }

                void TearDown() override {
                    dispose_symtable();
                }

                void CheckOptimisedTree(const std::string &input, const std::vector<int> &expected_output) {
                    std::string expected_str;

                    for (auto &str: expected_output)
                        expected_str += std::to_string(str) + " ";

                    char *actual = (char *) malloc(expected_str.length() + 1000);
                    output_fd = fmemopen(actual, expected_str.length() + 1000, "w");

                    syntax_abstract_tree_t *tree = load_syntax_tree(test_lex_input((char *) input.c_str()));
                    optimize_tree(tree);
                    syntax_abstract_tree_print(output_fd, tree);

                    fseek(output_fd, 0, SEEK_SET);
                    fread(actual, expected_str.length() + 1, 1, output_fd);

                    EXPECT_STREQ(expected_str.c_str(), actual) << "Input: " << input;
                }
            };

            TEST_F(OptimiserTest, Arithmetic) {
                CheckOptimisedTree("<?php"
                                   "$a = 1 + 2;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "$a = 1 * 2 + 3;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "$a = 1 * 2 + 4 / 2;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_FLOAT});
            }

            TEST_F(OptimiserTest, VariablesReplace) {
                CheckOptimisedTree("<?php"
                                   "$a = 1;"
                                   "$b = $a + 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});
            }
        }
    }
}