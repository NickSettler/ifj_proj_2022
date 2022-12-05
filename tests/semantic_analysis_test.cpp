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
                                            << ", got " << token->defined << std::endl << input;
                        if (node.type)
                            EXPECT_EQ(token->type, node.type) << "Token " << node.key << ". Expected type " << node.type
                                                              << ", got " << token->type << std::endl << input;

                        if (node.argument_type)
                            EXPECT_EQ(token->argument_type, node.argument_type)
                                                << "Token " << node.key << ". Expected argument type "
                                                << node.argument_type << ", got " << token->argument_type << std::endl
                                                << input;

                        if (node.argument_count)
                            EXPECT_EQ(token->argument_count, node.argument_count)
                                                << "Token " << node.key << ". Expected argument count "
                                                << node.argument_count << ", got " << token->argument_count << std::endl
                                                << input;

                        EXPECT_NE(token, nullptr)
                                            << "Token " << node.key << " not found in symtable" << std::endl << input;
                    }
                    dispose_symtable();
                }
            };

            TEST_F(SemanticAnalysisTest, VariableDefinition_DefinedVariable) {
                CheckSymTableEntries("<?php declare(strict_types=1); $a = 1;"
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
                CheckSymTableEntries("<?php declare(strict_types=1); $b = 1;"
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
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); $a = 1;"
                                                 "$b = $c + 2;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); $a = $a + 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); if ($b === 2) "
                                                 "{"
                                                 " $a = 3.4;"
                                                 "} else {"
                                                 " $b = $b - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); $b = 1;"
                                                 "if ($b === 2) "
                                                 "{"
                                                 " $b = 4;"
                                                 "} else {"
                                                 " $b = $c - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); $b = 1;"
                                                 "if ($b === 2) "
                                                 "{"
                                                 " $b = $c;"
                                                 "} else {"
                                                 " $b = $b - 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); $a = $a + 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries(
                        "<?php declare(strict_types=1); while ($a > 0) {$b = $b + 1; $a = $a - 1;}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); while ($a > 0) $a = $a - 1;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");
            }

            TEST_F(SemanticAnalysisTest, FunctionReturnType) {
                EXPECT_EXIT(CheckSymTableEntries(
                        "<?php declare(strict_types=1); function f(int $v, string $g, int $c): float {"
                        "}", {
                                (tree_node_t) {
                                        .defined = true,
                                        .key = "f",
                                },
                        }),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Function has no return");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function fork(): int {"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Function has no return");

                CheckSymTableEntries("<?php declare(strict_types=1); function f(string $s): string {"
                                     "  return $s;"
                                     "}"
                                     "$str = f(\"1\");", {
                                             (tree_node_t) {
                                                     .type = TYPE_STRING,
                                                     .argument_type = TYPE_STRING,
                                                     .argument_count = 1,
                                                     .defined = true,
                                                     .key = "f",
                                             },
                                             (tree_node_t) {
                                                     .type = TYPE_STRING,
                                                     .defined = true,
                                                     .key = "$str",
                                             },
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); function f(int $d) {"
                                     " $d = $d + 1;"
                                     "return ;"
                                     "}", {
                                             (tree_node_t) {
                                                     .type = TYPE_VOID,
                                                     .argument_type = TYPE_INT,
                                                     .argument_count = 1,
                                                     .defined = true,
                                                     .key = "f",

                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); function fork(): float {"
                                     "return 1 + 1.2;"
                                     "}", {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "fork",
                                             },
                                     });

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $f): float {"
                                                 "  return $i + $s;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$[A-Za-z_][A-Za-z0-9_]* used before declaration");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $a, float $b): float {"
                                                 "  $a = $a + 1;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_RET_ERROR_CODE),
                            "\\[SEMANTIC FUNC RET ERROR\\] Missing return value in function f");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $f): float {"
                                                 "  return $i;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Missing return value in function f");
            }

            TEST_F(SemanticAnalysisTest, FunctionCall) {
                CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
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
                                                     .argument_type = (data_type) (TYPE_INT | TYPE_FLOAT),
                                                     .argument_count = 2,
                                                     .defined = true,
                                                     .key = "f",
                                             },
                                             (tree_node_t) {
                                                     .type = TYPE_INT,
                                                     .argument_type = TYPE_INT,
                                                     .argument_count = 1,
                                                     .defined = true,
                                                     .key = "foo",
                                             },
                                             (tree_node_t) {
                                                     .type = TYPE_INT,
                                                     .defined = true,
                                                     .key = "$a",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
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
                                                     .argument_type = (data_type) (TYPE_INT | TYPE_FLOAT),
                                                     .argument_count = 2,
                                                     .defined = true,
                                                     .key = "f",
                                             },
                                             (tree_node_t) {
                                                     .type = TYPE_INT,
                                                     .argument_type = TYPE_INT,
                                                     .argument_count = 1,
                                                     .defined = true,
                                                     .key = "foo",
                                             },
                                             (tree_node_t) {
                                                     .type = TYPE_FLOAT,
                                                     .defined = true,
                                                     .key = "$a",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); write(\"Hello world!\");",
                                     {
                                             (tree_node_t) {
                                                     .defined = true,
                                                     .key = "write",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); substring(\"Hello world!\", 1, 3);",
                                     {
                                             (tree_node_t) {
                                                     .type = (data_type) (TYPE_STRING | TYPE_NULL),
                                                     .argument_type = (data_type) (TYPE_STRING | TYPE_INT),
                                                     .argument_count = 3,
                                                     .defined = true,
                                                     .key = "substring",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); write(\"a\", \"b\", \"c\");",
                                     {
                                             (tree_node_t) {
                                                     .argument_count = 0,
                                                     .defined = true,
                                                     .key = "write",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); reads();",
                                     {
                                             (tree_node_t) {
                                                     .type = (data_type) (TYPE_STRING | TYPE_NULL),
                                                     .defined = true,
                                                     .key = "reads",
                                             }
                                     }
                );

                CheckSymTableEntries("<?php declare(strict_types=1); strlen(\"abc\");",
                                     {
                                             (tree_node_t) {
                                                     .type = TYPE_INT,
                                                     .defined = true,
                                                     .key = "strlen",
                                             }
                                     }
                );

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); strlen(\"a\", 2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); strlen(2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument with value 2");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); reads(2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
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
                                                                 .argument_type = (data_type) (TYPE_INT | TYPE_FLOAT),
                                                                 .defined = true,
                                                                 .key = "f",
                                                         },
                                                         (tree_node_t) {
                                                                 .type = TYPE_INT,
                                                                 .argument_type = TYPE_INT,
                                                                 .defined = true,
                                                                 .key = "f00",
                                                         },
                                                         (tree_node_t) {
                                                                 .type = TYPE_FLOAT,
                                                                 .defined = true,
                                                                 .key = "$a",
                                                         },
                                                 }
                ), ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $f): float {"
                                                 "  return $i + $f;"
                                                 "}"
                                                 "$a = f(\"1\", 2.0);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $f): float {"
                                                 "  return $i + $f;"
                                                 "}"
                                                 "$a = f(2.0, 1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "f(\"1\", 2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "$a = f(1, 2.0, 3);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(int $i, float $s): float {"
                                                 "  return $i + $s;"
                                                 "}"
                                                 "$a = f(1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php declare(strict_types=1); function f(string $s): string {"
                                                 "  return $s;"
                                                 "}"
                                                 "$str = f(1);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");
            }

            TEST_F(SemanticAnalysisTest, ExternalTests) {
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function readi() : int {"
                                                 "  return 5;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_UNDEF_ERROR_CODE),
                            "\\[SEMANTIC UNDEF FUNC ERROR\\] Function is already declared");

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function hello() : int {"
                                                 "  return 5;"
                                                 "}"
                                                 ""
                                                 "function hello() : int {"
                                                 "    return 6;"
                                                 "  }", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_UNDEF_ERROR_CODE),
                            "\\[SEMANTIC UNDEF FUNC ERROR\\] Function is already declared");

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function f(int $x) : void {"
                                                 "    write($x, \"\n\");"
                                                 "}"
                                                 "f(\"1\");", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong type of argument");

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "function f(int $x) : void {"
                                     "    write($x, \"\n\");"
                                     "}"
                                     "f(1);", {
                                             (tree_node_t) {
                                                     .type = TYPE_VOID,
                                                     .argument_type = TYPE_INT,
                                                     .defined = true,
                                                     .key = "f",
                                             }
                                     });

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function f(int $x) : void {"
                                                 "    write($x, \"\n\");"
                                                 "}"
                                                 "f();", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function f(int $x) : void {"
                                                 "    write($x, \"\n\");"
                                                 "}"
                                                 "f(1, 2);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Wrong number of arguments");

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "write($x);", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$x used before declaration");

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "$x;", {}),
                            ::testing::ExitedWithCode(SEMANTIC_UNDEF_VAR_ERROR_CODE),
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$x used before declaration");

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "while(0 === 1) {"
                                     "    write(\"BAD\n\");"
                                     "}"
                                     "write(\"GOOD\n\");", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "while(\"\") {"
                                     "    write(\"BAD\n\");"
                                     "}"
                                     "write(\"GOOD\n\");", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "while(0.0) {"
                                     "    write(\"BAD\n\");"
                                     "}"
                                     "write(\"GOOD\n\");", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "$i = 0;"
                                     "while($i < 3) {"
                                     "    write($i, \"\n\");"
                                     "    $i = $i + 1;"
                                     "}"
                                     "write(\"GOOD\n\");", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "while(null) {"
                                     "    write(\"BAD\n\");"
                                     "}"
                                     "write(\"GOOD\n\");", {});

                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);\n"
                                                 "function f() : int {\n"
                                                 "}\n"
                                                 "f();", {}),
                            ::testing::ExitedWithCode(SEMANTIC_FUNC_ARG_ERROR_CODE),
                            "\\[SEMANTIC FUNC ARG ERROR\\] Function has no return");

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);\n"
                                     "if (0-1 > 0-1) { write(\"fail 0-1 > 0-1\\n\"); } else {}\n"
                                     "if (0-1 > 0) { write(\"fail 0-1 > 0\\n\"); } else {}\n"
                                     "if (0-1 > 1) { write(\"fail 0-1 > 1\\n\"); } else {}\n"
                                     "if (0-1 > 0-1.0) { write(\"fail 0-1 > 0-1.0\\n\"); } else {}\n"
                                     "if (0-1 > 0-0.5) { write(\"fail 0-1 > 0-0.5\\n\"); } else {}\n"
                                     "if (0-1 > 0.0) { write(\"fail 0-1 > 0.0\\n\"); } else {}\n"
                                     "if (0-1 > 0.5) { write(\"fail 0-1 > 0.5\\n\"); } else {}\n"
                                     "if (0-1 > 1.0) { write(\"fail 0-1 > 1.0\\n\"); } else {}\n"
                                     "if (0-1 > \"\") {} else { write(\"fail 0-1 > \\\"\\\"\\n\"); }\n"
                                     "if (0-1 > \"aa\") { write(\"fail 0-1 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-1 > \"ab\") { write(\"fail 0-1 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0 > 0-1) {} else { write(\"fail 0 > 0-1\\n\"); }\n"
                                     "if (0 > 0) { write(\"fail 0 > 0\\n\"); } else {}\n"
                                     "if (0 > 1) { write(\"fail 0 > 1\\n\"); } else {}\n"
                                     "if (0 > 0-1.0) {} else { write(\"fail 0 > 0-1.0\\n\"); }\n"
                                     "if (0 > 0-0.5) {} else { write(\"fail 0 > 0-0.5\\n\"); }\n"
                                     "if (0 > 0.0) { write(\"fail 0 > 0.0\\n\"); } else {}\n"
                                     "if (0 > 0.5) { write(\"fail 0 > 0.5\\n\"); } else {}\n"
                                     "if (0 > 1.0) { write(\"fail 0 > 1.0\\n\"); } else {}\n"
                                     "if (0 > \"\") {} else { write(\"fail 0 > \\\"\\\"\\n\"); }\n"
                                     "if (0 > \"aa\") { write(\"fail 0 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0 > \"ab\") { write(\"fail 0 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (1 > 0-1) {} else { write(\"fail 1 > 0-1\\n\"); }\n"
                                     "if (1 > 0) {} else { write(\"fail 1 > 0\\n\"); }\n"
                                     "if (1 > 1) { write(\"fail 1 > 1\\n\"); } else {}\n"
                                     "if (1 > 0-1.0) {} else { write(\"fail 1 > 0-1.0\\n\"); }\n"
                                     "if (1 > 0-0.5) {} else { write(\"fail 1 > 0-0.5\\n\"); }\n"
                                     "if (1 > 0.0) {} else { write(\"fail 1 > 0.0\\n\"); }\n"
                                     "if (1 > 0.5) {} else { write(\"fail 1 > 0.5\\n\"); }\n"
                                     "if (1 > 1.0) { write(\"fail 1 > 1.0\\n\"); } else {}\n"
                                     "if (1 > \"\") {} else { write(\"fail 1 > \\\"\\\"\\n\"); }\n"
                                     "if (1 > \"aa\") { write(\"fail 1 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (1 > \"ab\") { write(\"fail 1 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 > 0-1) { write(\"fail 0-1.0 > 0-1\\n\"); } else {}\n"
                                     "if (0-1.0 > 0) { write(\"fail 0-1.0 > 0\\n\"); } else {}\n"
                                     "if (0-1.0 > 1) { write(\"fail 0-1.0 > 1\\n\"); } else {}\n"
                                     "if (0-1.0 > 0-1.0) { write(\"fail 0-1.0 > 0-1.0\\n\"); } else {}\n"
                                     "if (0-1.0 > 0-0.5) { write(\"fail 0-1.0 > 0-0.5\\n\"); } else {}\n"
                                     "if (0-1.0 > 0.0) { write(\"fail 0-1.0 > 0.0\\n\"); } else {}\n"
                                     "if (0-1.0 > 0.5) { write(\"fail 0-1.0 > 0.5\\n\"); } else {}\n"
                                     "if (0-1.0 > 1.0) { write(\"fail 0-1.0 > 1.0\\n\"); } else {}\n"
                                     "if (0-1.0 > \"\") {} else { write(\"fail 0-1.0 > \\\"\\\"\\n\"); }\n"
                                     "if (0-1.0 > \"aa\") { write(\"fail 0-1.0 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 > \"ab\") { write(\"fail 0-1.0 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 > 0-1) {} else { write(\"fail 0-0.5 > 0-1\\n\"); }\n"
                                     "if (0-0.5 > 0) { write(\"fail 0-0.5 > 0\\n\"); } else {}\n"
                                     "if (0-0.5 > 1) { write(\"fail 0-0.5 > 1\\n\"); } else {}\n"
                                     "if (0-0.5 > 0-1.0) {} else { write(\"fail 0-0.5 > 0-1.0\\n\"); }\n"
                                     "if (0-0.5 > 0-0.5) { write(\"fail 0-0.5 > 0-0.5\\n\"); } else {}\n"
                                     "if (0-0.5 > 0.0) { write(\"fail 0-0.5 > 0.0\\n\"); } else {}\n"
                                     "if (0-0.5 > 0.5) { write(\"fail 0-0.5 > 0.5\\n\"); } else {}\n"
                                     "if (0-0.5 > 1.0) { write(\"fail 0-0.5 > 1.0\\n\"); } else {}\n"
                                     "if (0-0.5 > \"\") {} else { write(\"fail 0-0.5 > \\\"\\\"\\n\"); }\n"
                                     "if (0-0.5 > \"aa\") { write(\"fail 0-0.5 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 > \"ab\") { write(\"fail 0-0.5 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0.0 > 0-1) {} else { write(\"fail 0.0 > 0-1\\n\"); }\n"
                                     "if (0.0 > 0) { write(\"fail 0.0 > 0\\n\"); } else {}\n"
                                     "if (0.0 > 1) { write(\"fail 0.0 > 1\\n\"); } else {}\n"
                                     "if (0.0 > 0-1.0) {} else { write(\"fail 0.0 > 0-1.0\\n\"); }\n"
                                     "if (0.0 > 0-0.5) {} else { write(\"fail 0.0 > 0-0.5\\n\"); }\n"
                                     "if (0.0 > 0.0) { write(\"fail 0.0 > 0.0\\n\"); } else {}\n"
                                     "if (0.0 > 0.5) { write(\"fail 0.0 > 0.5\\n\"); } else {}\n"
                                     "if (0.0 > 1.0) { write(\"fail 0.0 > 1.0\\n\"); } else {}\n"
                                     "if (0.0 > \"\") {} else { write(\"fail 0.0 > \\\"\\\"\\n\"); }\n"
                                     "if (0.0 > \"aa\") { write(\"fail 0.0 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0.0 > \"ab\") { write(\"fail 0.0 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0.5 > 0-1) {} else { write(\"fail 0.5 > 0-1\\n\"); }\n"
                                     "if (0.5 > 0) {} else { write(\"fail 0.5 > 0\\n\"); }\n"
                                     "if (0.5 > 1) { write(\"fail 0.5 > 1\\n\"); } else {}\n"
                                     "if (0.5 > 0-1.0) {} else { write(\"fail 0.5 > 0-1.0\\n\"); }\n"
                                     "if (0.5 > 0-0.5) {} else { write(\"fail 0.5 > 0-0.5\\n\"); }\n"
                                     "if (0.5 > 0.0) {} else { write(\"fail 0.5 > 0.0\\n\"); }\n"
                                     "if (0.5 > 0.5) { write(\"fail 0.5 > 0.5\\n\"); } else {}\n"
                                     "if (0.5 > 1.0) { write(\"fail 0.5 > 1.0\\n\"); } else {}\n"
                                     "if (0.5 > \"\") {} else { write(\"fail 0.5 > \\\"\\\"\\n\"); }\n"
                                     "if (0.5 > \"aa\") { write(\"fail 0.5 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0.5 > \"ab\") { write(\"fail 0.5 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (1.0 > 0-1) {} else { write(\"fail 1.0 > 0-1\\n\"); }\n"
                                     "if (1.0 > 0) {} else { write(\"fail 1.0 > 0\\n\"); }\n"
                                     "if (1.0 > 1) { write(\"fail 1.0 > 1\\n\"); } else {}\n"
                                     "if (1.0 > 0-1.0) {} else { write(\"fail 1.0 > 0-1.0\\n\"); }\n"
                                     "if (1.0 > 0-0.5) {} else { write(\"fail 1.0 > 0-0.5\\n\"); }\n"
                                     "if (1.0 > 0.0) {} else { write(\"fail 1.0 > 0.0\\n\"); }\n"
                                     "if (1.0 > 0.5) {} else { write(\"fail 1.0 > 0.5\\n\"); }\n"
                                     "if (1.0 > 1.0) { write(\"fail 1.0 > 1.0\\n\"); } else {}\n"
                                     "if (1.0 > \"\") {} else { write(\"fail 1.0 > \\\"\\\"\\n\"); }\n"
                                     "if (1.0 > \"aa\") { write(\"fail 1.0 > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (1.0 > \"ab\") { write(\"fail 1.0 > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"\" > 0-1) { write(\"fail \\\"\\\" > 0-1\\n\"); } else {}\n"
                                     "if (\"\" > 0) { write(\"fail \\\"\\\" > 0\\n\"); } else {}\n"
                                     "if (\"\" > 1) { write(\"fail \\\"\\\" > 1\\n\"); } else {}\n"
                                     "if (\"\" > 0-1.0) { write(\"fail \\\"\\\" > 0-1.0\\n\"); } else {}\n"
                                     "if (\"\" > 0-0.5) { write(\"fail \\\"\\\" > 0-0.5\\n\"); } else {}\n"
                                     "if (\"\" > 0.0) { write(\"fail \\\"\\\" > 0.0\\n\"); } else {}\n"
                                     "if (\"\" > 0.5) { write(\"fail \\\"\\\" > 0.5\\n\"); } else {}\n"
                                     "if (\"\" > 1.0) { write(\"fail \\\"\\\" > 1.0\\n\"); } else {}\n"
                                     "if (\"\" > \"\") { write(\"fail \\\"\\\" > \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"\" > \"aa\") { write(\"fail \\\"\\\" > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"\" > \"ab\") { write(\"fail \\\"\\\" > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" > 0-1) {} else { write(\"fail \\\"aa\\\" > 0-1\\n\"); }\n"
                                     "if (\"aa\" > 0) {} else { write(\"fail \\\"aa\\\" > 0\\n\"); }\n"
                                     "if (\"aa\" > 1) {} else { write(\"fail \\\"aa\\\" > 1\\n\"); }\n"
                                     "if (\"aa\" > 0-1.0) {} else { write(\"fail \\\"aa\\\" > 0-1.0\\n\"); }\n"
                                     "if (\"aa\" > 0-0.5) {} else { write(\"fail \\\"aa\\\" > 0-0.5\\n\"); }\n"
                                     "if (\"aa\" > 0.0) {} else { write(\"fail \\\"aa\\\" > 0.0\\n\"); }\n"
                                     "if (\"aa\" > 0.5) {} else { write(\"fail \\\"aa\\\" > 0.5\\n\"); }\n"
                                     "if (\"aa\" > 1.0) {} else { write(\"fail \\\"aa\\\" > 1.0\\n\"); }\n"
                                     "if (\"aa\" > \"\") {} else { write(\"fail \\\"aa\\\" > \\\"\\\"\\n\"); }\n"
                                     "if (\"aa\" > \"aa\") { write(\"fail \\\"aa\\\" > \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" > \"ab\") { write(\"fail \\\"aa\\\" > \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" > 0-1) {} else { write(\"fail \\\"ab\\\" > 0-1\\n\"); }\n"
                                     "if (\"ab\" > 0) {} else { write(\"fail \\\"ab\\\" > 0\\n\"); }\n"
                                     "if (\"ab\" > 1) {} else { write(\"fail \\\"ab\\\" > 1\\n\"); }\n"
                                     "if (\"ab\" > 0-1.0) {} else { write(\"fail \\\"ab\\\" > 0-1.0\\n\"); }\n"
                                     "if (\"ab\" > 0-0.5) {} else { write(\"fail \\\"ab\\\" > 0-0.5\\n\"); }\n"
                                     "if (\"ab\" > 0.0) {} else { write(\"fail \\\"ab\\\" > 0.0\\n\"); }\n"
                                     "if (\"ab\" > 0.5) {} else { write(\"fail \\\"ab\\\" > 0.5\\n\"); }\n"
                                     "if (\"ab\" > 1.0) {} else { write(\"fail \\\"ab\\\" > 1.0\\n\"); }\n"
                                     "if (\"ab\" > \"\") {} else { write(\"fail \\\"ab\\\" > \\\"\\\"\\n\"); }\n"
                                     "if (\"ab\" > \"aa\") {} else { write(\"fail \\\"ab\\\" > \\\"aa\\\"\\n\"); }\n"
                                     "if (\"ab\" > \"ab\") { write(\"fail \\\"ab\\\" > \\\"ab\\\"\\n\"); } else {}\n"
                                     "write(\"done\");", {});

                CheckSymTableEntries("<?php\n"
                                     "declare(strict_types=1);\n"
                                     "if (0-1 >= 0-1) {} else { write(\"fail 0-1 >= 0-1\\n\"); }\n"
                                     "if (0-1 >= 0) { write(\"fail 0-1 >= 0\\n\"); } else {}\n"
                                     "if (0-1 >= 1) { write(\"fail 0-1 >= 1\\n\"); } else {}\n"
                                     "if (0-1 >= 0-1.0) {} else { write(\"fail 0-1 >= 0-1.0\\n\"); }\n"
                                     "if (0-1 >= 0-0.5) { write(\"fail 0-1 >= 0-0.5\\n\"); } else {}\n"
                                     "if (0-1 >= 0.0) { write(\"fail 0-1 >= 0.0\\n\"); } else {}\n"
                                     "if (0-1 >= 0.5) { write(\"fail 0-1 >= 0.5\\n\"); } else {}\n"
                                     "if (0-1 >= 1.0) { write(\"fail 0-1 >= 1.0\\n\"); } else {}\n"
                                     "if (0-1 >= \"\") {} else { write(\"fail 0-1 >= \\\"\\\"\\n\"); }\n"
                                     "if (0-1 >= \"aa\") { write(\"fail 0-1 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-1 >= \"ab\") { write(\"fail 0-1 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0 >= 0-1) {} else { write(\"fail 0 >= 0-1\\n\"); }\n"
                                     "if (0 >= 0) {} else { write(\"fail 0 >= 0\\n\"); }\n"
                                     "if (0 >= 1) { write(\"fail 0 >= 1\\n\"); } else {}\n"
                                     "if (0 >= 0-1.0) {} else { write(\"fail 0 >= 0-1.0\\n\"); }\n"
                                     "if (0 >= 0-0.5) {} else { write(\"fail 0 >= 0-0.5\\n\"); }\n"
                                     "if (0 >= 0.0) {} else { write(\"fail 0 >= 0.0\\n\"); }\n"
                                     "if (0 >= 0.5) { write(\"fail 0 >= 0.5\\n\"); } else {}\n"
                                     "if (0 >= 1.0) { write(\"fail 0 >= 1.0\\n\"); } else {}\n"
                                     "if (0 >= \"\") {} else { write(\"fail 0 >= \\\"\\\"\\n\"); }\n"
                                     "if (0 >= \"aa\") { write(\"fail 0 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0 >= \"ab\") { write(\"fail 0 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (1 >= 0-1) {} else { write(\"fail 1 >= 0-1\\n\"); }\n"
                                     "if (1 >= 0) {} else { write(\"fail 1 >= 0\\n\"); }\n"
                                     "if (1 >= 1) {} else { write(\"fail 1 >= 1\\n\"); }\n"
                                     "if (1 >= 0-1.0) {} else { write(\"fail 1 >= 0-1.0\\n\"); }\n"
                                     "if (1 >= 0-0.5) {} else { write(\"fail 1 >= 0-0.5\\n\"); }\n"
                                     "if (1 >= 0.0) {} else { write(\"fail 1 >= 0.0\\n\"); }\n"
                                     "if (1 >= 0.5) {} else { write(\"fail 1 >= 0.5\\n\"); }\n"
                                     "if (1 >= 1.0) {} else { write(\"fail 1 >= 1.0\\n\"); }\n"
                                     "if (1 >= \"\") {} else { write(\"fail 1 >= \\\"\\\"\\n\"); }\n"
                                     "if (1 >= \"aa\") { write(\"fail 1 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (1 >= \"ab\") { write(\"fail 1 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 >= 0-1) {} else { write(\"fail 0-1.0 >= 0-1\\n\"); }\n"
                                     "if (0-1.0 >= 0) { write(\"fail 0-1.0 >= 0\\n\"); } else {}\n"
                                     "if (0-1.0 >= 1) { write(\"fail 0-1.0 >= 1\\n\"); } else {}\n"
                                     "if (0-1.0 >= 0-1.0) {} else { write(\"fail 0-1.0 >= 0-1.0\\n\"); }\n"
                                     "if (0-1.0 >= 0-0.5) { write(\"fail 0-1.0 >= 0-0.5\\n\"); } else {}\n"
                                     "if (0-1.0 >= 0.0) { write(\"fail 0-1.0 >= 0.0\\n\"); } else {}\n"
                                     "if (0-1.0 >= 0.5) { write(\"fail 0-1.0 >= 0.5\\n\"); } else {}\n"
                                     "if (0-1.0 >= 1.0) { write(\"fail 0-1.0 >= 1.0\\n\"); } else {}\n"
                                     "if (0-1.0 >= \"\") {} else { write(\"fail 0-1.0 >= \\\"\\\"\\n\"); }\n"
                                     "if (0-1.0 >= \"aa\") { write(\"fail 0-1.0 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 >= \"ab\") { write(\"fail 0-1.0 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 >= 0-1) {} else { write(\"fail 0-0.5 >= 0-1\\n\"); }\n"
                                     "if (0-0.5 >= 0) { write(\"fail 0-0.5 >= 0\\n\"); } else {}\n"
                                     "if (0-0.5 >= 1) { write(\"fail 0-0.5 >= 1\\n\"); } else {}\n"
                                     "if (0-0.5 >= 0-1.0) {} else { write(\"fail 0-0.5 >= 0-1.0\\n\"); }\n"
                                     "if (0-0.5 >= 0-0.5) {} else { write(\"fail 0-0.5 >= 0-0.5\\n\"); }\n"
                                     "if (0-0.5 >= 0.0) { write(\"fail 0-0.5 >= 0.0\\n\"); } else {}\n"
                                     "if (0-0.5 >= 0.5) { write(\"fail 0-0.5 >= 0.5\\n\"); } else {}\n"
                                     "if (0-0.5 >= 1.0) { write(\"fail 0-0.5 >= 1.0\\n\"); } else {}\n"
                                     "if (0-0.5 >= \"\") {} else { write(\"fail 0-0.5 >= \\\"\\\"\\n\"); }\n"
                                     "if (0-0.5 >= \"aa\") { write(\"fail 0-0.5 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 >= \"ab\") { write(\"fail 0-0.5 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0.0 >= 0-1) {} else { write(\"fail 0.0 >= 0-1\\n\"); }\n"
                                     "if (0.0 >= 0) {} else { write(\"fail 0.0 >= 0\\n\"); }\n"
                                     "if (0.0 >= 1) { write(\"fail 0.0 >= 1\\n\"); } else {}\n"
                                     "if (0.0 >= 0-1.0) {} else { write(\"fail 0.0 >= 0-1.0\\n\"); }\n"
                                     "if (0.0 >= 0-0.5) {} else { write(\"fail 0.0 >= 0-0.5\\n\"); }\n"
                                     "if (0.0 >= 0.0) {} else { write(\"fail 0.0 >= 0.0\\n\"); }\n"
                                     "if (0.0 >= 0.5) { write(\"fail 0.0 >= 0.5\\n\"); } else {}\n"
                                     "if (0.0 >= 1.0) { write(\"fail 0.0 >= 1.0\\n\"); } else {}\n"
                                     "if (0.0 >= \"\") {} else { write(\"fail 0.0 >= \\\"\\\"\\n\"); }\n"
                                     "if (0.0 >= \"aa\") { write(\"fail 0.0 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0.0 >= \"ab\") { write(\"fail 0.0 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (0.5 >= 0-1) {} else { write(\"fail 0.5 >= 0-1\\n\"); }\n"
                                     "if (0.5 >= 0) {} else { write(\"fail 0.5 >= 0\\n\"); }\n"
                                     "if (0.5 >= 1) { write(\"fail 0.5 >= 1\\n\"); } else {}\n"
                                     "if (0.5 >= 0-1.0) {} else { write(\"fail 0.5 >= 0-1.0\\n\"); }\n"
                                     "if (0.5 >= 0-0.5) {} else { write(\"fail 0.5 >= 0-0.5\\n\"); }\n"
                                     "if (0.5 >= 0.0) {} else { write(\"fail 0.5 >= 0.0\\n\"); }\n"
                                     "if (0.5 >= 0.5) {} else { write(\"fail 0.5 >= 0.5\\n\"); }\n"
                                     "if (0.5 >= 1.0) { write(\"fail 0.5 >= 1.0\\n\"); } else {}\n"
                                     "if (0.5 >= \"\") {} else { write(\"fail 0.5 >= \\\"\\\"\\n\"); }\n"
                                     "if (0.5 >= \"aa\") { write(\"fail 0.5 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (0.5 >= \"ab\") { write(\"fail 0.5 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (1.0 >= 0-1) {} else { write(\"fail 1.0 >= 0-1\\n\"); }\n"
                                     "if (1.0 >= 0) {} else { write(\"fail 1.0 >= 0\\n\"); }\n"
                                     "if (1.0 >= 1) {} else { write(\"fail 1.0 >= 1\\n\"); }\n"
                                     "if (1.0 >= 0-1.0) {} else { write(\"fail 1.0 >= 0-1.0\\n\"); }\n"
                                     "if (1.0 >= 0-0.5) {} else { write(\"fail 1.0 >= 0-0.5\\n\"); }\n"
                                     "if (1.0 >= 0.0) {} else { write(\"fail 1.0 >= 0.0\\n\"); }\n"
                                     "if (1.0 >= 0.5) {} else { write(\"fail 1.0 >= 0.5\\n\"); }\n"
                                     "if (1.0 >= 1.0) {} else { write(\"fail 1.0 >= 1.0\\n\"); }\n"
                                     "if (1.0 >= \"\") {} else { write(\"fail 1.0 >= \\\"\\\"\\n\"); }\n"
                                     "if (1.0 >= \"aa\") { write(\"fail 1.0 >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (1.0 >= \"ab\") { write(\"fail 1.0 >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"\" >= 0-1) { write(\"fail \\\"\\\" >= 0-1\\n\"); } else {}\n"
                                     "if (\"\" >= 0) { write(\"fail \\\"\\\" >= 0\\n\"); } else {}\n"
                                     "if (\"\" >= 1) { write(\"fail \\\"\\\" >= 1\\n\"); } else {}\n"
                                     "if (\"\" >= 0-1.0) { write(\"fail \\\"\\\" >= 0-1.0\\n\"); } else {}\n"
                                     "if (\"\" >= 0-0.5) { write(\"fail \\\"\\\" >= 0-0.5\\n\"); } else {}\n"
                                     "if (\"\" >= 0.0) { write(\"fail \\\"\\\" >= 0.0\\n\"); } else {}\n"
                                     "if (\"\" >= 0.5) { write(\"fail \\\"\\\" >= 0.5\\n\"); } else {}\n"
                                     "if (\"\" >= 1.0) { write(\"fail \\\"\\\" >= 1.0\\n\"); } else {}\n"
                                     "if (\"\" >= \"\") {} else { write(\"fail \\\"\\\" >= \\\"\\\"\\n\"); }\n"
                                     "if (\"\" >= \"aa\") { write(\"fail \\\"\\\" >= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"\" >= \"ab\") { write(\"fail \\\"\\\" >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" >= 0-1) {} else { write(\"fail \\\"aa\\\" >= 0-1\\n\"); }\n"
                                     "if (\"aa\" >= 0) {} else { write(\"fail \\\"aa\\\" >= 0\\n\"); }\n"
                                     "if (\"aa\" >= 1) {} else { write(\"fail \\\"aa\\\" >= 1\\n\"); }\n"
                                     "if (\"aa\" >= 0-1.0) {} else { write(\"fail \\\"aa\\\" >= 0-1.0\\n\"); }\n"
                                     "if (\"aa\" >= 0-0.5) {} else { write(\"fail \\\"aa\\\" >= 0-0.5\\n\"); }\n"
                                     "if (\"aa\" >= 0.0) {} else { write(\"fail \\\"aa\\\" >= 0.0\\n\"); }\n"
                                     "if (\"aa\" >= 0.5) {} else { write(\"fail \\\"aa\\\" >= 0.5\\n\"); }\n"
                                     "if (\"aa\" >= 1.0) {} else { write(\"fail \\\"aa\\\" >= 1.0\\n\"); }\n"
                                     "if (\"aa\" >= \"\") {} else { write(\"fail \\\"aa\\\" >= \\\"\\\"\\n\"); }\n"
                                     "if (\"aa\" >= \"aa\") {} else { write(\"fail \\\"aa\\\" >= \\\"aa\\\"\\n\"); }\n"
                                     "if (\"aa\" >= \"ab\") { write(\"fail \\\"aa\\\" >= \\\"ab\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" >= 0-1) {} else { write(\"fail \\\"ab\\\" >= 0-1\\n\"); }\n"
                                     "if (\"ab\" >= 0) {} else { write(\"fail \\\"ab\\\" >= 0\\n\"); }\n"
                                     "if (\"ab\" >= 1) {} else { write(\"fail \\\"ab\\\" >= 1\\n\"); }\n"
                                     "if (\"ab\" >= 0-1.0) {} else { write(\"fail \\\"ab\\\" >= 0-1.0\\n\"); }\n"
                                     "if (\"ab\" >= 0-0.5) {} else { write(\"fail \\\"ab\\\" >= 0-0.5\\n\"); }\n"
                                     "if (\"ab\" >= 0.0) {} else { write(\"fail \\\"ab\\\" >= 0.0\\n\"); }\n"
                                     "if (\"ab\" >= 0.5) {} else { write(\"fail \\\"ab\\\" >= 0.5\\n\"); }\n"
                                     "if (\"ab\" >= 1.0) {} else { write(\"fail \\\"ab\\\" >= 1.0\\n\"); }\n"
                                     "if (\"ab\" >= \"\") {} else { write(\"fail \\\"ab\\\" >= \\\"\\\"\\n\"); }\n"
                                     "if (\"ab\" >= \"aa\") {} else { write(\"fail \\\"ab\\\" >= \\\"aa\\\"\\n\"); }\n"
                                     "if (\"ab\" >= \"ab\") {} else { write(\"fail \\\"ab\\\" >= \\\"ab\\\"\\n\"); }\n"
                                     "write(\"done\");", {});

                CheckSymTableEntries("<?php\n"
                                     "declare(strict_types=1);\n"
                                     "if (0-1 < 0-1) { write(\"fail 0-1 < 0-1\\n\"); } else {}\n"
                                     "if (0-1 < 0) {} else { write(\"fail 0-1 < 0\\n\"); }\n"
                                     "if (0-1 < 1) {} else { write(\"fail 0-1 < 1\\n\"); }\n"
                                     "if (0-1 < 0-1.0) { write(\"fail 0-1 < 0-1.0\\n\"); } else {}\n"
                                     "if (0-1 < 0-0.5) {} else { write(\"fail 0-1 < 0-0.5\\n\"); }\n"
                                     "if (0-1 < 0.0) {} else { write(\"fail 0-1 < 0.0\\n\"); }\n"
                                     "if (0-1 < 0.5) {} else { write(\"fail 0-1 < 0.5\\n\"); }\n"
                                     "if (0-1 < 1.0) {} else { write(\"fail 0-1 < 1.0\\n\"); }\n"
                                     "if (0-1 < \"\") { write(\"fail 0-1 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-1 < \"aa\") {} else { write(\"fail 0-1 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0-1 < \"ab\") {} else { write(\"fail 0-1 < \\\"ab\\\"\\n\"); }\n"
                                     "if (0 < 0-1) { write(\"fail 0 < 0-1\\n\"); } else {}\n"
                                     "if (0 < 0) { write(\"fail 0 < 0\\n\"); } else {}\n"
                                     "if (0 < 1) {} else { write(\"fail 0 < 1\\n\"); }\n"
                                     "if (0 < 0-1.0) { write(\"fail 0 < 0-1.0\\n\"); } else {}\n"
                                     "if (0 < 0-0.5) { write(\"fail 0 < 0-0.5\\n\"); } else {}\n"
                                     "if (0 < 0.0) { write(\"fail 0 < 0.0\\n\"); } else {}\n"
                                     "if (0 < 0.5) {} else { write(\"fail 0 < 0.5\\n\"); }\n"
                                     "if (0 < 1.0) {} else { write(\"fail 0 < 1.0\\n\"); }\n"
                                     "if (0 < \"\") { write(\"fail 0 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0 < \"aa\") {} else { write(\"fail 0 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0 < \"ab\") {} else { write(\"fail 0 < \\\"ab\\\"\\n\"); }\n"
                                     "if (1 < 0-1) { write(\"fail 1 < 0-1\\n\"); } else {}\n"
                                     "if (1 < 0) { write(\"fail 1 < 0\\n\"); } else {}\n"
                                     "if (1 < 1) { write(\"fail 1 < 1\\n\"); } else {}\n"
                                     "if (1 < 0-1.0) { write(\"fail 1 < 0-1.0\\n\"); } else {}\n"
                                     "if (1 < 0-0.5) { write(\"fail 1 < 0-0.5\\n\"); } else {}\n"
                                     "if (1 < 0.0) { write(\"fail 1 < 0.0\\n\"); } else {}\n"
                                     "if (1 < 0.5) { write(\"fail 1 < 0.5\\n\"); } else {}\n"
                                     "if (1 < 1.0) { write(\"fail 1 < 1.0\\n\"); } else {}\n"
                                     "if (1 < \"\") { write(\"fail 1 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (1 < \"aa\") {} else { write(\"fail 1 < \\\"aa\\\"\\n\"); }\n"
                                     "if (1 < \"ab\") {} else { write(\"fail 1 < \\\"ab\\\"\\n\"); }\n"
                                     "if (0-1.0 < 0-1) { write(\"fail 0-1.0 < 0-1\\n\"); } else {}\n"
                                     "if (0-1.0 < 0) {} else { write(\"fail 0-1.0 < 0\\n\"); }\n"
                                     "if (0-1.0 < 1) {} else { write(\"fail 0-1.0 < 1\\n\"); }\n"
                                     "if (0-1.0 < 0-1.0) { write(\"fail 0-1.0 < 0-1.0\\n\"); } else {}\n"
                                     "if (0-1.0 < 0-0.5) {} else { write(\"fail 0-1.0 < 0-0.5\\n\"); }\n"
                                     "if (0-1.0 < 0.0) {} else { write(\"fail 0-1.0 < 0.0\\n\"); }\n"
                                     "if (0-1.0 < 0.5) {} else { write(\"fail 0-1.0 < 0.5\\n\"); }\n"
                                     "if (0-1.0 < 1.0) {} else { write(\"fail 0-1.0 < 1.0\\n\"); }\n"
                                     "if (0-1.0 < \"\") { write(\"fail 0-1.0 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 < \"aa\") {} else { write(\"fail 0-1.0 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0-1.0 < \"ab\") {} else { write(\"fail 0-1.0 < \\\"ab\\\"\\n\"); }\n"
                                     "if (0-0.5 < 0-1) { write(\"fail 0-0.5 < 0-1\\n\"); } else {}\n"
                                     "if (0-0.5 < 0) {} else { write(\"fail 0-0.5 < 0\\n\"); }\n"
                                     "if (0-0.5 < 1) {} else { write(\"fail 0-0.5 < 1\\n\"); }\n"
                                     "if (0-0.5 < 0-1.0) { write(\"fail 0-0.5 < 0-1.0\\n\"); } else {}\n"
                                     "if (0-0.5 < 0-0.5) { write(\"fail 0-0.5 < 0-0.5\\n\"); } else {}\n"
                                     "if (0-0.5 < 0.0) {} else { write(\"fail 0-0.5 < 0.0\\n\"); }\n"
                                     "if (0-0.5 < 0.5) {} else { write(\"fail 0-0.5 < 0.5\\n\"); }\n"
                                     "if (0-0.5 < 1.0) {} else { write(\"fail 0-0.5 < 1.0\\n\"); }\n"
                                     "if (0-0.5 < \"\") { write(\"fail 0-0.5 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 < \"aa\") {} else { write(\"fail 0-0.5 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0-0.5 < \"ab\") {} else { write(\"fail 0-0.5 < \\\"ab\\\"\\n\"); }\n"
                                     "if (0.0 < 0-1) { write(\"fail 0.0 < 0-1\\n\"); } else {}\n"
                                     "if (0.0 < 0) { write(\"fail 0.0 < 0\\n\"); } else {}\n"
                                     "if (0.0 < 1) {} else { write(\"fail 0.0 < 1\\n\"); }\n"
                                     "if (0.0 < 0-1.0) { write(\"fail 0.0 < 0-1.0\\n\"); } else {}\n"
                                     "if (0.0 < 0-0.5) { write(\"fail 0.0 < 0-0.5\\n\"); } else {}\n"
                                     "if (0.0 < 0.0) { write(\"fail 0.0 < 0.0\\n\"); } else {}\n"
                                     "if (0.0 < 0.5) {} else { write(\"fail 0.0 < 0.5\\n\"); }\n"
                                     "if (0.0 < 1.0) {} else { write(\"fail 0.0 < 1.0\\n\"); }\n"
                                     "if (0.0 < \"\") { write(\"fail 0.0 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0.0 < \"aa\") {} else { write(\"fail 0.0 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0.0 < \"ab\") {} else { write(\"fail 0.0 < \\\"ab\\\"\\n\"); }\n"
                                     "if (0.5 < 0-1) { write(\"fail 0.5 < 0-1\\n\"); } else {}\n"
                                     "if (0.5 < 0) { write(\"fail 0.5 < 0\\n\"); } else {}\n"
                                     "if (0.5 < 1) {} else { write(\"fail 0.5 < 1\\n\"); }\n"
                                     "if (0.5 < 0-1.0) { write(\"fail 0.5 < 0-1.0\\n\"); } else {}\n"
                                     "if (0.5 < 0-0.5) { write(\"fail 0.5 < 0-0.5\\n\"); } else {}\n"
                                     "if (0.5 < 0.0) { write(\"fail 0.5 < 0.0\\n\"); } else {}\n"
                                     "if (0.5 < 0.5) { write(\"fail 0.5 < 0.5\\n\"); } else {}\n"
                                     "if (0.5 < 1.0) {} else { write(\"fail 0.5 < 1.0\\n\"); }\n"
                                     "if (0.5 < \"\") { write(\"fail 0.5 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (0.5 < \"aa\") {} else { write(\"fail 0.5 < \\\"aa\\\"\\n\"); }\n"
                                     "if (0.5 < \"ab\") {} else { write(\"fail 0.5 < \\\"ab\\\"\\n\"); }\n"
                                     "if (1.0 < 0-1) { write(\"fail 1.0 < 0-1\\n\"); } else {}\n"
                                     "if (1.0 < 0) { write(\"fail 1.0 < 0\\n\"); } else {}\n"
                                     "if (1.0 < 1) { write(\"fail 1.0 < 1\\n\"); } else {}\n"
                                     "if (1.0 < 0-1.0) { write(\"fail 1.0 < 0-1.0\\n\"); } else {}\n"
                                     "if (1.0 < 0-0.5) { write(\"fail 1.0 < 0-0.5\\n\"); } else {}\n"
                                     "if (1.0 < 0.0) { write(\"fail 1.0 < 0.0\\n\"); } else {}\n"
                                     "if (1.0 < 0.5) { write(\"fail 1.0 < 0.5\\n\"); } else {}\n"
                                     "if (1.0 < 1.0) { write(\"fail 1.0 < 1.0\\n\"); } else {}\n"
                                     "if (1.0 < \"\") { write(\"fail 1.0 < \\\"\\\"\\n\"); } else {}\n"
                                     "if (1.0 < \"aa\") {} else { write(\"fail 1.0 < \\\"aa\\\"\\n\"); }\n"
                                     "if (1.0 < \"ab\") {} else { write(\"fail 1.0 < \\\"ab\\\"\\n\"); }\n"
                                     "if (\"\" < 0-1) {} else { write(\"fail \\\"\\\" < 0-1\\n\"); }\n"
                                     "if (\"\" < 0) {} else { write(\"fail \\\"\\\" < 0\\n\"); }\n"
                                     "if (\"\" < 1) {} else { write(\"fail \\\"\\\" < 1\\n\"); }\n"
                                     "if (\"\" < 0-1.0) {} else { write(\"fail \\\"\\\" < 0-1.0\\n\"); }\n"
                                     "if (\"\" < 0-0.5) {} else { write(\"fail \\\"\\\" < 0-0.5\\n\"); }\n"
                                     "if (\"\" < 0.0) {} else { write(\"fail \\\"\\\" < 0.0\\n\"); }\n"
                                     "if (\"\" < 0.5) {} else { write(\"fail \\\"\\\" < 0.5\\n\"); }\n"
                                     "if (\"\" < 1.0) {} else { write(\"fail \\\"\\\" < 1.0\\n\"); }\n"
                                     "if (\"\" < \"\") { write(\"fail \\\"\\\" < \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"\" < \"aa\") {} else { write(\"fail \\\"\\\" < \\\"aa\\\"\\n\"); }\n"
                                     "if (\"\" < \"ab\") {} else { write(\"fail \\\"\\\" < \\\"ab\\\"\\n\"); }\n"
                                     "if (\"aa\" < 0-1) { write(\"fail \\\"aa\\\" < 0-1\\n\"); } else {}\n"
                                     "if (\"aa\" < 0) { write(\"fail \\\"aa\\\" < 0\\n\"); } else {}\n"
                                     "if (\"aa\" < 1) { write(\"fail \\\"aa\\\" < 1\\n\"); } else {}\n"
                                     "if (\"aa\" < 0-1.0) { write(\"fail \\\"aa\\\" < 0-1.0\\n\"); } else {}\n"
                                     "if (\"aa\" < 0-0.5) { write(\"fail \\\"aa\\\" < 0-0.5\\n\"); } else {}\n"
                                     "if (\"aa\" < 0.0) { write(\"fail \\\"aa\\\" < 0.0\\n\"); } else {}\n"
                                     "if (\"aa\" < 0.5) { write(\"fail \\\"aa\\\" < 0.5\\n\"); } else {}\n"
                                     "if (\"aa\" < 1.0) { write(\"fail \\\"aa\\\" < 1.0\\n\"); } else {}\n"
                                     "if (\"aa\" < \"\") { write(\"fail \\\"aa\\\" < \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" < \"aa\") { write(\"fail \\\"aa\\\" < \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" < \"ab\") {} else { write(\"fail \\\"aa\\\" < \\\"ab\\\"\\n\"); }\n"
                                     "if (\"ab\" < 0-1) { write(\"fail \\\"ab\\\" < 0-1\\n\"); } else {}\n"
                                     "if (\"ab\" < 0) { write(\"fail \\\"ab\\\" < 0\\n\"); } else {}\n"
                                     "if (\"ab\" < 1) { write(\"fail \\\"ab\\\" < 1\\n\"); } else {}\n"
                                     "if (\"ab\" < 0-1.0) { write(\"fail \\\"ab\\\" < 0-1.0\\n\"); } else {}\n"
                                     "if (\"ab\" < 0-0.5) { write(\"fail \\\"ab\\\" < 0-0.5\\n\"); } else {}\n"
                                     "if (\"ab\" < 0.0) { write(\"fail \\\"ab\\\" < 0.0\\n\"); } else {}\n"
                                     "if (\"ab\" < 0.5) { write(\"fail \\\"ab\\\" < 0.5\\n\"); } else {}\n"
                                     "if (\"ab\" < 1.0) { write(\"fail \\\"ab\\\" < 1.0\\n\"); } else {}\n"
                                     "if (\"ab\" < \"\") { write(\"fail \\\"ab\\\" < \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" < \"aa\") { write(\"fail \\\"ab\\\" < \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" < \"ab\") { write(\"fail \\\"ab\\\" < \\\"ab\\\"\\n\"); } else {}\n"
                                     "write(\"done\");", {});

                CheckSymTableEntries("<?php\n"
                                     "declare(strict_types=1);\n"
                                     "if (0-1 <= 0-1) {} else { write(\"fail 0-1 <= 0-1\\n\"); }\n"
                                     "if (0-1 <= 0) {} else { write(\"fail 0-1 <= 0\\n\"); }\n"
                                     "if (0-1 <= 1) {} else { write(\"fail 0-1 <= 1\\n\"); }\n"
                                     "if (0-1 <= 0-1.0) {} else { write(\"fail 0-1 <= 0-1.0\\n\"); }\n"
                                     "if (0-1 <= 0-0.5) {} else { write(\"fail 0-1 <= 0-0.5\\n\"); }\n"
                                     "if (0-1 <= 0.0) {} else { write(\"fail 0-1 <= 0.0\\n\"); }\n"
                                     "if (0-1 <= 0.5) {} else { write(\"fail 0-1 <= 0.5\\n\"); }\n"
                                     "if (0-1 <= 1.0) {} else { write(\"fail 0-1 <= 1.0\\n\"); }\n"
                                     "if (0-1 <= \"\") { write(\"fail 0-1 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-1 <= \"aa\") {} else { write(\"fail 0-1 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0-1 <= \"ab\") {} else { write(\"fail 0-1 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (0 <= 0-1) { write(\"fail 0 <= 0-1\\n\"); } else {}\n"
                                     "if (0 <= 0) {} else { write(\"fail 0 <= 0\\n\"); }\n"
                                     "if (0 <= 1) {} else { write(\"fail 0 <= 1\\n\"); }\n"
                                     "if (0 <= 0-1.0) { write(\"fail 0 <= 0-1.0\\n\"); } else {}\n"
                                     "if (0 <= 0-0.5) { write(\"fail 0 <= 0-0.5\\n\"); } else {}\n"
                                     "if (0 <= 0.0) {} else { write(\"fail 0 <= 0.0\\n\"); }\n"
                                     "if (0 <= 0.5) {} else { write(\"fail 0 <= 0.5\\n\"); }\n"
                                     "if (0 <= 1.0) {} else { write(\"fail 0 <= 1.0\\n\"); }\n"
                                     "if (0 <= \"\") { write(\"fail 0 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0 <= \"aa\") {} else { write(\"fail 0 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0 <= \"ab\") {} else { write(\"fail 0 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (1 <= 0-1) { write(\"fail 1 <= 0-1\\n\"); } else {}\n"
                                     "if (1 <= 0) { write(\"fail 1 <= 0\\n\"); } else {}\n"
                                     "if (1 <= 1) {} else { write(\"fail 1 <= 1\\n\"); }\n"
                                     "if (1 <= 0-1.0) { write(\"fail 1 <= 0-1.0\\n\"); } else {}\n"
                                     "if (1 <= 0-0.5) { write(\"fail 1 <= 0-0.5\\n\"); } else {}\n"
                                     "if (1 <= 0.0) { write(\"fail 1 <= 0.0\\n\"); } else {}\n"
                                     "if (1 <= 0.5) { write(\"fail 1 <= 0.5\\n\"); } else {}\n"
                                     "if (1 <= 1.0) {} else { write(\"fail 1 <= 1.0\\n\"); }\n"
                                     "if (1 <= \"\") { write(\"fail 1 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (1 <= \"aa\") {} else { write(\"fail 1 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (1 <= \"ab\") {} else { write(\"fail 1 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (0-1.0 <= 0-1) {} else { write(\"fail 0-1.0 <= 0-1\\n\"); }\n"
                                     "if (0-1.0 <= 0) {} else { write(\"fail 0-1.0 <= 0\\n\"); }\n"
                                     "if (0-1.0 <= 1) {} else { write(\"fail 0-1.0 <= 1\\n\"); }\n"
                                     "if (0-1.0 <= 0-1.0) {} else { write(\"fail 0-1.0 <= 0-1.0\\n\"); }\n"
                                     "if (0-1.0 <= 0-0.5) {} else { write(\"fail 0-1.0 <= 0-0.5\\n\"); }\n"
                                     "if (0-1.0 <= 0.0) {} else { write(\"fail 0-1.0 <= 0.0\\n\"); }\n"
                                     "if (0-1.0 <= 0.5) {} else { write(\"fail 0-1.0 <= 0.5\\n\"); }\n"
                                     "if (0-1.0 <= 1.0) {} else { write(\"fail 0-1.0 <= 1.0\\n\"); }\n"
                                     "if (0-1.0 <= \"\") { write(\"fail 0-1.0 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-1.0 <= \"aa\") {} else { write(\"fail 0-1.0 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0-1.0 <= \"ab\") {} else { write(\"fail 0-1.0 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (0-0.5 <= 0-1) { write(\"fail 0-0.5 <= 0-1\\n\"); } else {}\n"
                                     "if (0-0.5 <= 0) {} else { write(\"fail 0-0.5 <= 0\\n\"); }\n"
                                     "if (0-0.5 <= 1) {} else { write(\"fail 0-0.5 <= 1\\n\"); }\n"
                                     "if (0-0.5 <= 0-1.0) { write(\"fail 0-0.5 <= 0-1.0\\n\"); } else {}\n"
                                     "if (0-0.5 <= 0-0.5) {} else { write(\"fail 0-0.5 <= 0-0.5\\n\"); }\n"
                                     "if (0-0.5 <= 0.0) {} else { write(\"fail 0-0.5 <= 0.0\\n\"); }\n"
                                     "if (0-0.5 <= 0.5) {} else { write(\"fail 0-0.5 <= 0.5\\n\"); }\n"
                                     "if (0-0.5 <= 1.0) {} else { write(\"fail 0-0.5 <= 1.0\\n\"); }\n"
                                     "if (0-0.5 <= \"\") { write(\"fail 0-0.5 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0-0.5 <= \"aa\") {} else { write(\"fail 0-0.5 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0-0.5 <= \"ab\") {} else { write(\"fail 0-0.5 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (0.0 <= 0-1) { write(\"fail 0.0 <= 0-1\\n\"); } else {}\n"
                                     "if (0.0 <= 0) {} else { write(\"fail 0.0 <= 0\\n\"); }\n"
                                     "if (0.0 <= 1) {} else { write(\"fail 0.0 <= 1\\n\"); }\n"
                                     "if (0.0 <= 0-1.0) { write(\"fail 0.0 <= 0-1.0\\n\"); } else {}\n"
                                     "if (0.0 <= 0-0.5) { write(\"fail 0.0 <= 0-0.5\\n\"); } else {}\n"
                                     "if (0.0 <= 0.0) {} else { write(\"fail 0.0 <= 0.0\\n\"); }\n"
                                     "if (0.0 <= 0.5) {} else { write(\"fail 0.0 <= 0.5\\n\"); }\n"
                                     "if (0.0 <= 1.0) {} else { write(\"fail 0.0 <= 1.0\\n\"); }\n"
                                     "if (0.0 <= \"\") { write(\"fail 0.0 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0.0 <= \"aa\") {} else { write(\"fail 0.0 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0.0 <= \"ab\") {} else { write(\"fail 0.0 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (0.5 <= 0-1) { write(\"fail 0.5 <= 0-1\\n\"); } else {}\n"
                                     "if (0.5 <= 0) { write(\"fail 0.5 <= 0\\n\"); } else {}\n"
                                     "if (0.5 <= 1) {} else { write(\"fail 0.5 <= 1\\n\"); }\n"
                                     "if (0.5 <= 0-1.0) { write(\"fail 0.5 <= 0-1.0\\n\"); } else {}\n"
                                     "if (0.5 <= 0-0.5) { write(\"fail 0.5 <= 0-0.5\\n\"); } else {}\n"
                                     "if (0.5 <= 0.0) { write(\"fail 0.5 <= 0.0\\n\"); } else {}\n"
                                     "if (0.5 <= 0.5) {} else { write(\"fail 0.5 <= 0.5\\n\"); }\n"
                                     "if (0.5 <= 1.0) {} else { write(\"fail 0.5 <= 1.0\\n\"); }\n"
                                     "if (0.5 <= \"\") { write(\"fail 0.5 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (0.5 <= \"aa\") {} else { write(\"fail 0.5 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (0.5 <= \"ab\") {} else { write(\"fail 0.5 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (1.0 <= 0-1) { write(\"fail 1.0 <= 0-1\\n\"); } else {}\n"
                                     "if (1.0 <= 0) { write(\"fail 1.0 <= 0\\n\"); } else {}\n"
                                     "if (1.0 <= 1) {} else { write(\"fail 1.0 <= 1\\n\"); }\n"
                                     "if (1.0 <= 0-1.0) { write(\"fail 1.0 <= 0-1.0\\n\"); } else {}\n"
                                     "if (1.0 <= 0-0.5) { write(\"fail 1.0 <= 0-0.5\\n\"); } else {}\n"
                                     "if (1.0 <= 0.0) { write(\"fail 1.0 <= 0.0\\n\"); } else {}\n"
                                     "if (1.0 <= 0.5) { write(\"fail 1.0 <= 0.5\\n\"); } else {}\n"
                                     "if (1.0 <= 1.0) {} else { write(\"fail 1.0 <= 1.0\\n\"); }\n"
                                     "if (1.0 <= \"\") { write(\"fail 1.0 <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (1.0 <= \"aa\") {} else { write(\"fail 1.0 <= \\\"aa\\\"\\n\"); }\n"
                                     "if (1.0 <= \"ab\") {} else { write(\"fail 1.0 <= \\\"ab\\\"\\n\"); }\n"
                                     "if (\"\" <= 0-1) {} else { write(\"fail \\\"\\\" <= 0-1\\n\"); }\n"
                                     "if (\"\" <= 0) {} else { write(\"fail \\\"\\\" <= 0\\n\"); }\n"
                                     "if (\"\" <= 1) {} else { write(\"fail \\\"\\\" <= 1\\n\"); }\n"
                                     "if (\"\" <= 0-1.0) {} else { write(\"fail \\\"\\\" <= 0-1.0\\n\"); }\n"
                                     "if (\"\" <= 0-0.5) {} else { write(\"fail \\\"\\\" <= 0-0.5\\n\"); }\n"
                                     "if (\"\" <= 0.0) {} else { write(\"fail \\\"\\\" <= 0.0\\n\"); }\n"
                                     "if (\"\" <= 0.5) {} else { write(\"fail \\\"\\\" <= 0.5\\n\"); }\n"
                                     "if (\"\" <= 1.0) {} else { write(\"fail \\\"\\\" <= 1.0\\n\"); }\n"
                                     "if (\"\" <= \"\") {} else { write(\"fail \\\"\\\" <= \\\"\\\"\\n\"); }\n"
                                     "if (\"\" <= \"aa\") {} else { write(\"fail \\\"\\\" <= \\\"aa\\\"\\n\"); }\n"
                                     "if (\"\" <= \"ab\") {} else { write(\"fail \\\"\\\" <= \\\"ab\\\"\\n\"); }\n"
                                     "if (\"aa\" <= 0-1) { write(\"fail \\\"aa\\\" <= 0-1\\n\"); } else {}\n"
                                     "if (\"aa\" <= 0) { write(\"fail \\\"aa\\\" <= 0\\n\"); } else {}\n"
                                     "if (\"aa\" <= 1) { write(\"fail \\\"aa\\\" <= 1\\n\"); } else {}\n"
                                     "if (\"aa\" <= 0-1.0) { write(\"fail \\\"aa\\\" <= 0-1.0\\n\"); } else {}\n"
                                     "if (\"aa\" <= 0-0.5) { write(\"fail \\\"aa\\\" <= 0-0.5\\n\"); } else {}\n"
                                     "if (\"aa\" <= 0.0) { write(\"fail \\\"aa\\\" <= 0.0\\n\"); } else {}\n"
                                     "if (\"aa\" <= 0.5) { write(\"fail \\\"aa\\\" <= 0.5\\n\"); } else {}\n"
                                     "if (\"aa\" <= 1.0) { write(\"fail \\\"aa\\\" <= 1.0\\n\"); } else {}\n"
                                     "if (\"aa\" <= \"\") { write(\"fail \\\"aa\\\" <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"aa\" <= \"aa\") {} else { write(\"fail \\\"aa\\\" <= \\\"aa\\\"\\n\"); }\n"
                                     "if (\"aa\" <= \"ab\") {} else { write(\"fail \\\"aa\\\" <= \\\"ab\\\"\\n\"); }\n"
                                     "if (\"ab\" <= 0-1) { write(\"fail \\\"ab\\\" <= 0-1\\n\"); } else {}\n"
                                     "if (\"ab\" <= 0) { write(\"fail \\\"ab\\\" <= 0\\n\"); } else {}\n"
                                     "if (\"ab\" <= 1) { write(\"fail \\\"ab\\\" <= 1\\n\"); } else {}\n"
                                     "if (\"ab\" <= 0-1.0) { write(\"fail \\\"ab\\\" <= 0-1.0\\n\"); } else {}\n"
                                     "if (\"ab\" <= 0-0.5) { write(\"fail \\\"ab\\\" <= 0-0.5\\n\"); } else {}\n"
                                     "if (\"ab\" <= 0.0) { write(\"fail \\\"ab\\\" <= 0.0\\n\"); } else {}\n"
                                     "if (\"ab\" <= 0.5) { write(\"fail \\\"ab\\\" <= 0.5\\n\"); } else {}\n"
                                     "if (\"ab\" <= 1.0) { write(\"fail \\\"ab\\\" <= 1.0\\n\"); } else {}\n"
                                     "if (\"ab\" <= \"\") { write(\"fail \\\"ab\\\" <= \\\"\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" <= \"aa\") { write(\"fail \\\"ab\\\" <= \\\"aa\\\"\\n\"); } else {}\n"
                                     "if (\"ab\" <= \"ab\") {} else { write(\"fail \\\"ab\\\" <= \\\"ab\\\"\\n\"); }\n"
                                     "write(\"done\");", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "$x = 1.0;"
                                     "$i = 0;"
                                     "while($i < 10000) {"
                                     "$x = ($x + 2.0/$x)/2.0;"
                                     "$i = $i + 1;"
                                     "}"
                                     "write(\"Square root of 2 is: \", $x);", {});

                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);", {});

                CheckSymTableEntries("<?php\n"
                                     "\n"
                                     "\n"
                                     "\n"
                                     "declare(         strict_types   =\n"
                                     "1       )\n"
                                     ";", {});

                CheckSymTableEntries("<?php\n"
                                     "declare(strict_types=1);\n"
                                     "$x = 04534.1543210e+5655;", {});

            }

            TEST_F(SemanticAnalysisTest, ExternalTestsParser) {

                // Assignment

                // Assignment nowhere
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "5;", {});
                // Okay
                CheckSymTableEntries("<?php"
                                     " declare(strict_types=1);"
                                     "$x = 5;", {});

                // Function

                // Builtin
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "reads();"
                                     "readi();"
                                     "readf();"
                                     "write(5);"
                                     "write(\"hello world\", 99);", {});

                // Call without parenthesis
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "function hello() : string {"
                                                 "        write(\"Hello world\n\");"
                                                 "        return \"This is not good\n\";"
                                                 "}"
                                                 "write(hello);", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left parenthesis Expecting \\(, found: \\)");

                // Custom function
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"

                                     "function f() : void {"
                                     "  return;"
                                     "}"

                                     "function g() : int {"
                                     "  return 5;"
                                     "}"

                                     "f();"
                                     "g();", {});

                // Keyword function else
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"

                                                 "function else() : void {"
                                                 "  return;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Identifier Expecting ID, found: else");

                // Keyword function return
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"

                                                 "function return() : void {"
                                                 "  return;"
                                                 "}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Identifier Expecting ID, found: return");

                // Trailing comma
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "write(\"hello world\", 99,);", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                // Trailing commas
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 "write(\"hello world\", 99,,);", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ,");

                // If

                // Empty if
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "if(1===1){}else{}", {});

                // Multiple statements
                CheckSymTableEntries("""<?php"
                                     "declare(strict_types=1);"
                                     "if(1===1) {"
                                     "    $x = 5;"
                                     "    $y = 7;"
                                     "} else {"
                                     "    $x = 6;"
                                     "    $y = 8;"
                                     "}", {});

                // Single statement
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "if(1===1) {"
                                     "    $x = 5;"
                                     "} else {"
                                     "    $x = 6;"
                                     "}", {});

                // Random

                // Equals 10
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 ""
                                                 "=== 10;", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ===");

                // Return

                // Global empty return
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "return;", {});

                // Global return constant
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "return 0;", {});

                // Global return expression
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "return 1+2;", {});

                // Solo semicolon
                EXPECT_EXIT(CheckSymTableEntries("<?php"
                                                 "declare(strict_types=1);"
                                                 ";", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ;");

                // While

                // Empty
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     "while(1===0){}", {});
            }

            TEST_F(SemanticAnalysisTest, RosettaCode) {
                // Mandelbrot fixed
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"

                                     "// source: https://rosettacode.org/wiki/Mandelbrot_set#Fixed_point_16_bit_arithmetic"

                                     "/**"
                                     "  ascii Mandelbrot using 16 bits of fixed point integer maths with a selectable fractional precision in bits."

                                     "  This is still only 16 bits mathc and allocating more than 6 bits of fractional precision leads to an overflow that adds noise to the plot.."
                                     ""
                                     "  This code frequently casts to short to ensure we're not accidentally benefitting from GCC promotion from short 16 bits to int."

                                     "  gcc fixedPoint.c  -lm"

                                     " */"

                                     "// convenient casting"
                                     "function si(int $i) : int {"
                                     "    $ret = intval($i);"
                                     "    $param = $ret / 65536;"
                                     "    $intval = intval($param);"
                                     "    $ret = $ret - $intval * 65536;"
                                     "    return $ret;"
                                     "  }"

                                     "  function sf(float $i) : int {"
                                     "    $ret = intval($i);"
                                     "    $param = $ret / 65536;"
                                     "    $intval = intval($param);"
                                     "    $ret = $ret - $intval * 65536;"
                                     "    return $ret;"
                                     "  }"

                                     "function pow(int $x, int $y) : int {"
                                     "    $ret = 1;"
                                     "    $i = 0;"
                                     "    while($i < $y) {"
                                     "        $ret = $ret * $x;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    return $ret;"
                                     "}"

                                     "function toPrec(float $f, int $bitsPrecision) : int {"
                                     "    $p = pow(2,$bitsPrecision);"
                                     "    $i = intval($f);"
                                     "    $whole = $i * $p;"
                                     "    $r = ($f-$i)* $p;"
                                     "    $part = intval($r);"
                                     "    $ret = $whole + $part;"
                                     "    return $ret;"
                                     "}"

                                     "function shiftRight(int $n, int $bits) : int {"
                                     "    $i = 0;"
                                     "    $ret = $n;"
                                     "    while($i < $bits) {"
                                     "        $ret = $ret / 2;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    $ret = intval($ret);"
                                     "    return $ret;"
                                     "}"

                                     "// chosen to match https://www.youtube.com/watch?v=DC5wi6iv9io"
                                     "$width = 32; // basic width of a zx81"
                                     "$height = 22; // basic width of a zx81"
                                     "$zoom=1;  // bigger with finer detail ie a smaller step size - leave at 1 for 32x22"

                                     "// params"
                                     "$bitsPrecision = 6;"
                                     "write(\"PRECISION=\", $bitsPrecision, \"\\n\");"
                                     "$p = toPrec(3.5,$bitsPrecision);"
                                     "$val = $p / $zoom;"
                                     "$X1 = intval($val);"
                                     "$X2 = toPrec(2.25,$bitsPrecision) ;"
                                     "$p = toPrec(3.0,$bitsPrecision);"
                                     "$val = $p/$zoom;"
                                     "$Y1 = intval($val) ;   // horiz pos"
                                     "$Y2 = toPrec(1.5,$bitsPrecision) ; // vert pos"
                                     "$LIMIT = toPrec(4.0,$bitsPrecision);"


                                     "// fractal"
                                     "//char * chr = \".:-=X$#@.\";"
                                     "$chr = \"abcdefghijklmnopqr \";"
                                     "//char * chr = \".,'~=+:;[/<&?oxOX#.\";"
                                     "$maxIters = strlen($chr);"

                                     "$py=0;"
                                     "$runy = 1;"
                                     "while ($py < $height*$zoom) {"
                                     "$px=0;"
                                     "while ($px < $width*$zoom) {"
                                     "    $six_arg = $px*$X1;"
                                     "    $siy_arg = $py*$Y1;"
                                     "    $six = si($six_arg);"
                                     "    $siy = si($siy_arg);"
                                     "    $sfx_arg = $six / $width;"
                                     "    $sfy_arg = $siy / $height;"
                                     "    $sfx = sf($sfx_arg);"
                                     "    $sfy = sf($sfy_arg);"
                                     "    $x0 = $sfx - $X2;"
                                     "    $y0 = $sfy - $Y2;"

                                     "    $x=0;"
                                     "    $y=0;"

                                     "    $i=0;"

                                     "    $xSqr = 0;"
                                     "    $ySqr = 0;"
                                     "    $maxItersT = $maxIters;"
                                     "    while ($i < $maxItersT) {"
                                     "        $six_arg = $x * $x;"
                                     "        $siy_arg = $y * $y;"
                                     "        $six = si($six_arg);"
                                     "        $siy = si($siy_arg);"
                                     "        $xSqr = shiftRight($six, $bitsPrecision);"
                                     "        $ySqr = shiftRight($siy, $bitsPrecision);"

                                     "        // Breakout if sum is > the limit OR breakout also if sum is negative which indicates overflow of the addition has occurred"
                                     "        // The overflow check is only needed for precisions of over 6 bits because for 7 and above the sums come out overflowed and negative therefore we always run to maxIters and we see nothing."
                                     "        // By including the overflow break out we can see the fractal again though with noise."
                                     "        if (($xSqr + $ySqr) >= $LIMIT) {"
                                     "            $maxItersT = 0;"
                                     "        } else {}"
                                     "        if(($xSqr+$ySqr) < 0) {"
                                     "            $maxItersT = 0;"
                                     "        } else {}"

                                     "        if($maxItersT !== 0) {"
                                     "            $xt = $xSqr - $ySqr + $x0;"
                                     "            $xy = $x * $y;"
                                     "            $si = si($xy);"
                                     "            $sr = shiftRight($si, $bitsPrecision);"
                                     "            $si = si($sr);"
                                     "            $si = $si * 2;"
                                     "            $si = si($si);"
                                     "            $si = $si + $y0;"
                                     "            $y = si($si);"
                                     "            $x=$xt;"

                                     "            $i = $i + 1;"
                                     "        } else {}"
                                     "    }"
                                     "    $i = $i - 1;"
                                     "    $i2 = $i+1;"
                                     "    $substr = substring($chr, $i, $i2);"
                                     "    write($substr);"

                                     "    $px = $px + 1;"
                                     "}"

                                     "write(\"\\n\");"
                                     "$py = $py + 1;"
                                     "}", {});

                // Mandelbrot float
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"

                                     "// source: https://rosettacode.org/wiki/Mandelbrot_set#AWK"

                                     "$XSize=59; $YSize=21;"
                                     "$MinIm=0-1.0; $MaxIm=1.0;$MinRe=0-2.0; $MaxRe=1.0;"
                                     "$StepX=($MaxRe-$MinRe)/$XSize; $StepY=($MaxIm-$MinIm)/$YSize;"
                                     "$y=0;"
                                     "while($y<$YSize){"
                                     "    $Im=$MinIm+$StepY*$y;"
                                     "    $x = 0;"
                                     "    while($x<$XSize) {"
                                     "        $Re=$MinRe+$StepX*$x; $Zr=$Re; $Zi=$Im;"
                                     "        $n = 0;"
                                     "        $aaa = 0;"
                                     "        while($aaa+$n<30) {"
                                     "            $a=$Zr*$Zr; $b=$Zi*$Zi;"
                                     "            if($a+$b>4.0) {"
                                     "                $aaa = 999;"
                                     "            } else {"
                                     "                $Zi=2*$Zr*$Zi+$Im; $Zr=$a-$b+$Re;"
                                     "                $n = $n + 1;"
                                     "            }"
                                     "        }"
                                     "        $code = 62-$n;"
                                     "        $chr = chr($code);"
                                     "        write($chr);"
                                     "        $x = $x + 1;"
                                     "    }"
                                     "    write(\"\\n\");"
                                     "    $y = $y + 1;"
                                     "}", {});

                // Proper divisors
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"

                                     "// source: https://rosettacode.org/wiki/Proper_divisors#C"

                                     "function proper_divisors(int $n, int $print_flag) : int"
                                     "{"
                                     "    $count = 0;"

                                     "    $i = 1;"
                                     "    while($i < $n) {"
                                     "        $intval_arg = $n / $i;"
                                     "        $intval = intval($intval_arg);"
                                     "        if ($n - $intval * $i === 0) {"
                                     "            $count = $count + 1;"
                                     "            if ($print_flag) {"
                                     "                write($i, \" \");"
                                     "            } else {}"
                                     "        } else {}"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    if($print_flag) {"
                                     "        write(\"\\n\");"
                                     "    } else {}"

                                     "    return $count;"
                                     "}"

                                     "$i = 1;"
                                     "while ($i <= 10) {"
                                     "    write($i, \": \");"
                                     "    proper_divisors($i, 1);"
                                     "    $i = $i + 1;"
                                     "}"

                                     "$i = 1;"
                                     "$max = 0;"
                                     "$max_i = 0;"
                                     "while($i<=100) {"
                                     "    $count = proper_divisors($i, 0);"
                                     "    if($count > $max) {"
                                     "        $max = $count;"
                                     "        $max_i = $i;"
                                     "    } else {}"
                                     "    $i = $i + 1;"
                                     "}"
                                     "write($max_i, \" with \", $max, \" divisors\\n\");", {});

                // ROT13 - fix
//                CheckSymTableEntries("<?php"
//                                     "declare(strict_types=1);"
//
//                                     "function strtr(string $str, string $from, string $to) : string {"
//                                     "    $ret = \"\";"
//                                     "    $i = 0;"
//                                     "$len = strlen($str);"
//                                     "while($i < $len) {"
//                                     "        $i1 = $i + 1;"
//                                     "        $nextChar = substring($str, $i, $i1);"
//                                     "        $translatedChar = $nextChar;"
//                                     "        $j = 0;"
//                                     "        $len1 = strlen($from);"
//                                     "        while($j < $len1) {"
//                                     "            $j1 = $j + 1;"
//                                     "            $nextChar1 = substring($from, $j, $j1);"
//                                     "            if($nextChar1 === $nextChar) {"
//                                     "                $translatedChar = substring($to, $j, $j1);"
//                                     "            } else {}"
//                                     "            $j = $j1;"
//                                     "        }"
//                                     "        $ret = $ret . $translatedChar;"
//                                     "        $i = $i + 1;"
//                                     "    }"
//                                     "    return $ret;"
//                                     "}"
//
//                                     "// source: https://rosettacode.org/wiki/Rot-13#PHP"
//
//                                     "function rot13(string $s) : string {"
//                                     "    $ret = strtr($s, \"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\","
//                                     "        \"NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm\");"
//                                     "    return $ret;"
//                                     "}"
//
//                                     "$in = reads();"
//                                     "$out = rot13($in);"
//                                     "write($out, \"\\n\");", {});

                // Sierpinski carpet
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"

                                     "// source: https://rosettacode.org/wiki/Sierpinski_carpet#Python"

                                     "function in_carpet(int $x, int $y) : int {"
                                     "    while(1===1) {"
                                     "        if($x === 0) {"
                                     "            return 1;"
                                     "        } else {}"
                                     "        if($y === 0) {"
                                     "            return 1;"
                                     "        } else {}"
                                     "        $x3 = $x / 3;"
                                     "        $ival = intval($x3);"
                                     "        if($x - $ival*3 === 1) {"
                                     "            $y3 = $y / 3;"
                                     "            $ival = intval($y3);"
                                     "            if($y - $ival * 3 === 1) {"
                                     "                return 0;"
                                     "            } else {}"
                                     "        } else {}"
                                     "        "
                                     "        $x = $x / 3;"
                                     "        $x = intval($x);"
                                     "        $y = $y / 3;"
                                     "        $y = intval($y);"
                                     "    }"
                                     "}"

                                     "function carpet(int $n) : void {"
                                     "    $i = 0;"
                                     "    while($i < $n) {"
                                     "        $j = 0;"
                                     "        while($j < $n) {"
                                     "            $inc = in_carpet($i, $j);"
                                     "            if($inc) {"
                                     "                write(\"*\");"
                                     "            } else {"
                                     "                write(" ");"
                                     "            }"
                                     "            $j = $j + 1;"
                                     "        }"
                                     "        write(\"\\n\");"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "}"

                                     "carpet(27);", {});

                // Sierpinski triangle
                CheckSymTableEntries("<?php"
                                     "declare(strict_types=1);"
                                     ""
                                     "// source: https://rosettacode.org/wiki/Sierpinski_triangle#Python"

                                     "function append(string $in, string $s) : string {"
                                     "    return $in . $s . \";\";"
                                     "}"

                                     "function get(string $in, int $id) : ?string {"
                                     "    $i = 0;"
                                     "    $pos = 0;"
                                     "    while ($i < $id) {"
                                     "        $p1 = $pos + 1;"
                                     "        $ch = substring($in, $pos, $p1);"
                                     "        while($ch !== \";\") {"
                                     "            $pos = $pos + 1;"
                                     "            $p1 = $pos + 1;"
                                     "            $ch = substring($in, $pos, $p1);"
                                     "        }"
                                     "        $pos = $pos + 1;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    $pos_end = $pos;"
                                     "    $p1 = $pos_end + 1;"
                                     "    $ch = substring($in, $pos_end, $p1);"
                                     "    while($ch !== \";\") {"
                                     "        $pos_end = $pos_end + 1;"
                                     "        $p1 = $pos_end + 1;"
                                     "        $ch = substring($in, $pos_end, $p1);"
                                     "    }"
                                     "    $ret = substring($in, $pos, $pos_end);"
                                     "    return $ret;"
                                     "}"

                                     "function set(string $in, int $id, string $s) : string {"
                                     "    $i = 0;"
                                     "    $pos = 0;"
                                     "    while ($i < $id) {"
                                     "        $p1 = $pos + 1;"
                                     "        $ch = substring($in, $pos, $p1);"
                                     "        while($ch !== \";\") {"
                                     "            $pos = $pos + 1;"
                                     "            $p1 = $pos + 1;"
                                     "            $ch = substring($in, $pos, $p1);"
                                     "        }"
                                     "        $pos = $pos + 1;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    $pos_end = $pos;"
                                     "    $p1 = $pos_end + 1;"
                                     "    $ch = substring($in, $pos_end, $p1);"
                                     "    while($ch !== \";\") {"
                                     "        $pos_end = $pos_end + 1;"
                                     "        $p1 = $pos_end + 1;"
                                     "        $ch = substring($in, $pos_end, $p1);"
                                     "    }"
                                     "    $before = substring($in, 0, $pos);"
                                     "    $slen = strlen($in);"
                                     "    $after = substring($in, $pos_end, $slen);"
                                     "    return $before . $s . $after;"
                                     "}"

                                     "function length(string $in) : int {"
                                     "    $i = 0;"
                                     "    $pos = 0;"
                                     "    $p1 = $pos + 1;"
                                     "    $ch = substring($in, $pos, $p1);"
                                     "    while($ch !== null) {"
                                     "        $pos = $pos + 1;"
                                     "        $p1 = $pos + 1;"
                                     "        $ch = substring($in, $pos, $p1);"
                                     "        if ($ch === \";\") {"
                                     "            $i = $i + 1;"
                                     "        } else {}"
                                     "    }"
                                     "    return $i;"
                                     "}"

                                     "function pow(int $x, int $y) : int {"
                                     "    $i = 0;"
                                     "    $ret = 1;"
                                     "    while($i < $y) {"
                                     "        $ret = $ret * $x;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    return $ret;"
                                     "}"

                                     "function sierpinski(int $n) : string {"
                                     "    $d = "";"
                                     "    $d = append($d, \" * \");"
                                     "    $i = 0;"
                                     "    while($i < $n) {"
                                     "        $j = 0;"
                                     "        $sp = "";"
                                     "        $p = pow(2, $i);"
                                     "        while($j < $p) {"
                                     "            $sp = $sp . " ";"
                                     "            $j = $j + 1;"
                                     "        }"
                                     "        $j = 0;"
                                     "        $len = length($d);"
                                     "        $d2 = "";"
                                     "        while($j < $len) {"
                                     "            $s = get($d, $j);"
                                     "            $t2 = $sp . $s . $sp;"
                                     "            $d2 = append($d2, $t2);"
                                     "            $j = $j + 1;"
                                     "        }"
                                     "        $j=0;"
                                     "        $d3 = "";"
                                     "        while($j < $len) {"
                                     "            $s = get($d, $j);"
                                     "            $t3 = $s . " " . $s;"
                                     "            $d3 = append($d3, $t3);"
                                     "            $j = $j + 1;"
                                     "        }"
                                     "        $d = $d2 . $d3;"
                                     "        $i = $i + 1;"
                                     "    }"
                                     "    return $d;"
                                     "}"

                                     "$tri = sierpinski(4);"
                                     "$i = 0;"
                                     "$len = length($tri);"
                                     "while($i < $len) {"
                                     "    $res = get($tri, $i);"
                                     "    write($res);"
                                     "    write(\"\\n\");"
                                     "    $i = $i + 1;"
                                     "    $len = length($tri);"
                                     "}", {});
            }
        }
    }
}
