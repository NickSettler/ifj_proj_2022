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
                    semantic_tree_check(tree);
                    optimize_tree(tree);
                    syntax_abstract_tree_print(output_fd, tree);

                    fseek(output_fd, 0, SEEK_SET);
                    fread(actual, expected_str.length() + 1, 1, output_fd);

                    EXPECT_STREQ(expected_str.c_str(), actual) << "Input: " << input.c_str();
                }
            };

            TEST_F(OptimiserTest, Arithmetic) {
                CheckOptimisedTree("<?php declare(strict_types=1);"
                                   "$a = 1 + 2;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php declare(strict_types=1);"
                                   "$a = 1 * 2 + 3;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php declare(strict_types=1);"
                                   "$a = 1 * 2 + 4 / 2;",
                                   {SYN_NODE_SEQUENCE});
            }

            TEST_F(OptimiserTest, VariablesReplace) {
                CheckOptimisedTree("<?php declare(strict_types=1);"
                                   "$a = 1;"
                                   "$b = $a + 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE,});
            }

            TEST_F(OptimiserTest, UnreachableIfElimination) {
                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(0)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 - 1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 * 1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(0 / 1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 === 1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 !== 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 < 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 <= 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 > 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 >= 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(0 && 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 && 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(0 || 2)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(!1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(!0)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(0)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 === 1)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 !== 1)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 + 12 === 10 + 3)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if(1 + 12 === 10 + 5)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "if ($a === 1)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "if ($a >= 0 + 1)"
                                   "  $a = 1;"
                                   "else {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                    SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "if ($a <= 0)"
                                   "  $a = 1;"
                                   "else if ($a === 0) {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "} else "
                                   "  write(1);",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_CALL, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "if ($a <= 0)"
                                   "  $a = 1;"
                                   "else if ($a + 2 === 0 + 2) {"
                                   "  $a = 2;"
                                   "  $b = 3;"
                                   "} else "
                                   "  write(1);",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_CALL, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if (1) {"
                                   "  if (1)"
                                   "    write(1);"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_CALL, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if (1) {"
                                   "  if (0)"
                                   "    write(1);"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if (0) {"
                                   "  if (1)"
                                   "    write(1);"
                                   "} else {"
                                   "  if(1) write(1);"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_CALL, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "if (0) {"
                                   "  if (1)"
                                   "    write(1);"
                                   "} else {"
                                   "  if(0) write(1);"
                                   "}",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE});
            }

            TEST_F(OptimiserTest, UnreachableWhileElimination) {
                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(0)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(1)"
                                   "  $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE,
                                    SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(1)"
                                   "  while(2)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE,
                                    SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(1)"
                                   "  while(0)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(0)"
                                   "  while(0)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "while(0)"
                                   "  while(1)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "while($a === 0)"
                                   "  while(1)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_SEQUENCE});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "while($a === 1)"
                                   "  while(1)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                    SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "while($a + 1 >= 2)"
                                   "  while(1)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ADD, SYN_NODE_INTEGER,
                                    SYN_NODE_GREATER_EQUAL, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE,
                                    SYN_NODE_INTEGER, SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                    SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                CheckOptimisedTree("<?php"
                                   "declare(strict_types=1);"
                                   "$a = 1;"
                                   "while($a >= 1)"
                                   "  while(0)"
                                   "    $a = 1;",
                                   {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                    SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_GREATER_EQUAL, SYN_NODE_INTEGER,
                                    SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE});
            }
        }
    }
}