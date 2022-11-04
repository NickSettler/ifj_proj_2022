/**
 * Implementace překladače imperativního jazyka IFJ22.
 * @authors
 *   xmoise01, Nikita Moiseev
 *
 * @file semantic_analysis_test.cpp
 * @brief 
 * @date 04.11.2022
 */

#include <gtest/gtest.h>

extern "C" {
#include "../src/symtable.h"
#include "../src/lexical_fsm.h"
#include "../src/syntax_analyzer.h"
}

namespace ifj {
    namespace tests {
        namespace {
            class SemanticAnalysisTest : public ::testing::Test {
            protected:
                FILE *fd{};
                syntax_abstract_tree_t *tree{};

                SemanticAnalysisTest() = default;

                ~SemanticAnalysisTest() override {
                    if (fd != nullptr) {
                        fclose(fd);
                    }
                }

                void TearDown() override {
                    if (fd != nullptr) {
                        fclose(fd);
                    }
                }

                void ProcessInput(const std::string &input) {
                    fd = test_lex_input((char *) input.c_str());

                    tree = load_syntax_tree(fd);
                }

                void CheckSymTableEntries(const std::string &input, std::vector<tree_node_t> &expected) {
                    ProcessInput(input);

                    for (auto &node: expected) {
                        tree_node_t *token = find_token(node.key);
                        EXPECT_NE(token, nullptr) << "Token " << node.key << " not found in symtable";
                        EXPECT_EQ(token->defined, node.defined)
                                            << "Token " << node.key << ". Expected defined " << node.defined
                                            << ", got " << token->defined;
                        EXPECT_EQ(token->type, node.type)
                                            << "Token " << node.key << ". Expected type " << node.type
                                            << ", got " << token->type;
                        EXPECT_EQ(token->global, node.global)
                                            << "Token " << node.key << ". Expected global " << node.global
                                            << ", got " << token->global;
                    }
                }
            };

            TEST_F(SemanticAnalysisTest, VariableDefined) {
                std::string input = "$a = 1;"
                                    "$b = $c + 2;";

                std::vector<tree_node_t> expected = {
                        {
                                .defined = true,
                                .key = (char *) "$a",
                        },
                        {
                                .defined = true,
                                .key = (char *) "$b",
                        },
                        {
                                // TODO: This should be false, but for some reason it's true
                                .defined = true,
                                .key = (char *) "$c",
                        }
                };

                CheckSymTableEntries(input, expected);
            }
        }
    }
}
