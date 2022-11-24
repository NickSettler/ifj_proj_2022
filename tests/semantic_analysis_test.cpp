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
#include "../src/semantic_analyzer.c"
#include "../src/semantic_analyzer.h"
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
                    semantic_tree_check(tree);
                }

                void CheckSymTableEntries(const std::string &input, const std::vector<tree_node_t> &expected) {
                    ProcessInput(input);

                    for (auto node: expected) {
                        tree_node_t *token = find_token(node.key);
                        EXPECT_EQ(token->defined, node.defined)
                                            << "Token " << node.key << ". Expected defined " << node.defined
                                            << ", got " << token->defined;
                        EXPECT_NE(token, nullptr) << "Token " << node.key << " not found in symtable";
                    }
                    dispose_symtable();
                }
            };

            TEST_F(SemanticAnalysisTest, VariableDefinition_DefinedVariable) {
                CheckSymTableEntries("<?php $a = 1;"
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
                CheckSymTableEntries("<?php $b = 1;"
                                     "if ($b > 2) "
                                     "{"
                                     " $v = 4;"
                                     "} else {"
                                     " $b = $b - 1;"
                                     "}", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "$b",
                                             },
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "$v",
                                             },
                                     });
            }

            TEST_F(SemanticAnalysisTest, VariableDefinition_UndefinedVariable) {
                EXPECT_EXIT(CheckSymTableEntries("<?php $a = 1;"
                                                 "$b = $c + 2;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php $a = $a + 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php if ($b == 2) "
                                                 "{"
                                                 " $a = 3.4;"
                                                 "} else {"
                                                 " $b = $b - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php $b = 1;"
                                                 "if ($b == 2) "
                                                 "{"
                                                 " $b = 4;"
                                                 "} else {"
                                                 " $b = $c - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php $b = 1;"
                                                 "if ($b == 2) "
                                                 "{"
                                                 " $b = $c;"
                                                 "} else {"
                                                 " $b = $b - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php $a = $a + 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php while ($a > 0) {$b = $b + 1; $a = $a - 1;}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php while ($a > 0) $a = $a - 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
            }

            TEST_F(SemanticAnalysisTest, FunctionReturnType) {
                CheckSymTableEntries("<?php function f(int $v, string $g, int $c): float {"
                                     "}", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "f",
                                             },
                                     });

                CheckSymTableEntries("<?php function fork(): int {"
                                     "}", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "fork",
                                             },
                                     });

                CheckSymTableEntries("<?php function f(string $s): string {"
                                     "  return $s;"
                                     "}"
                                     "$str = f(\"1\");", {
                                             (tree_node_t) {
                                                     .type = TYPE_STRING,
                                                     .defined = true,
                                                     .key = "$str",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php function f(int $d) {"
                                     " $d = $d + 1;"
                                     "return ;"
                                     "}", {
                                             (tree_node_t) {
                                                     .type = TYPE_VOID,
                                                     .defined = true,
                                                     .key = "f",

                                             }
                                     }
                );

                CheckSymTableEntries("<?php function fork(): float {"
                                     "return 1 + 1.2;"
                                     "}", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "fork",
                                             },
                                     });

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $f): float {"
                                                 "  return $i + $s;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $a, float $b): float {"
                                                 "  $a = $a + 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_RET_ERROR_CODE),
                            "\\[SEMANTIC FUNC RET ERROR\\] Missing return value in function f");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $f): float {"
                                                 "  return $i;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_RET_ERROR_CODE),
                            "\\[SEMANTIC FUNC RET ERROR\\] Wrong return value in function f");
            }

            TEST_F(SemanticAnalysisTest, FunctionCall) {
                CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                     "return $i + $s;"
                                     "}"
                                     "function foo(int $i) {"
                                     "return $i;"
                                     "}"
                                     "$a = f(1, 2.0);"
                                     "$a = foo(4);",
                                     {
                                             (tree_node_t) {
                                                     .type = TYPE_FLOAT,
                                                     .defined = true,
                                                     .key = "$a",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                     "return $i + $s;"
                                     "}"
                                     "function foo(int $i) {"
                                     "return $i;"
                                     "}"
                                     "$a = foo(4);"
                                     "$a = f(1, 2.0);",
                                     {
                                             (tree_node_t) {
                                                     .type = TYPE_FLOAT,
                                                     .defined = true,
                                                     .key = "$a",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php write(\"Hello world!\");",
                                     {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "write",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php substring(\"Hello world!\", 1, 3);",
                                     {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "substring",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php write(\"a\", \"b\", \"c\");",
                                     {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "write",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php reads();",
                                     {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "reads",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php strlen(\"abc\");",
                                     {
                                             (tree_node_t) {
                                                     .type = TYPE_INT,
                                                     .defined = true,
                                                     .key = "strlen",
                                             }
                                     }
                );

                EXPECT_EXIT(CheckSymTableEntries("<?php strlen(\"a\", 2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php strlen(2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument with value 2");

                EXPECT_EXIT(CheckSymTableEntries("<?php reads(2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                                 "return $i + $s;"
                                                 "}"
                                                 "function foo(int $i) {"
                                                 "return $i;"
                                                 "}"
                                                 "$a = f(1);"
                                                 "$a = foo(4);",
                                                 {
                                                         (tree_node_t) {
                                                                 .type = TYPE_FLOAT,
                                                                 .defined = true,
                                                                 .key = "$a",
                                                         }
                                                 }
                ), ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $f): float {"
                                                 "  return $i + $f;"
                                                 "}"
                                                 "$a = f(\"1\", 2.0);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $f): float {"
                                                 "  return $i + $f;"
                                                 "}"
                                                 "$a = f(2.0, 1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "f(\"1\", 2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "$a = f(1, 2.0, 3);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "$a = f(1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php function f(string $s): string {"
                                                 "  return $s;"
                                                 "}"
                                                 "$str = f(1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");
            }
        }
    }
}
