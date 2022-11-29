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
                            "\\[SEMANTIC UNDEF VAR ERROR\\] Variable \\$x is not defined");

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
        }
    }
}
