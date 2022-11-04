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

                void TearDown() override {
                    if (fd != nullptr) {
                        fclose(fd);
                    }

                    dispose_symtable();
                }

                void ProcessInput(const std::string &input) {
                    fd = test_lex_input((char *) input.c_str());

                    tree = load_syntax_tree(fd);
                }

                void CheckSymTableEntries(const std::string &input, const std::vector<tree_node_t> &expected) {
                    ProcessInput(input);

                    for (auto node: expected) {
                        tree_node_t *token = find_token(node.key);
                        EXPECT_NE(token, nullptr) << "Token " << node.key << " not found in symtable";
                        EXPECT_EQ(token->defined, node.defined)
                                            << "Token " << node.key << ". Expected defined " << node.defined
                                            << ", got " << token->defined;
                    }
                }
            };

            TEST_F(SemanticAnalysisTest, VariableDefinition_DefinedVariable) {
                CheckSymTableEntries("$a = 1;"
                                     "$b = $a + 2;", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "$a",
                                             },
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "$b",
                                             },
                                     });
            }

            TEST_F(SemanticAnalysisTest, VariableDefinition_UndefinedVariable) {
                EXPECT_EXIT({
                                fd = test_lex_input("$a = 1;"
                                                    "$b = $c + 2;");
                                tree = load_syntax_tree(fd);
                            },
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
            }
        }
    }
}
