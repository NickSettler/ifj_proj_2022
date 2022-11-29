#include <gtest/gtest.h>

extern "C" {
#include "../src/errors.h"
#include "../src/errors.c"
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

                    char *actual = (char *) malloc(expected_str.length() + 1000);
                    output_fd = fmemopen(actual, expected_str.length() + 1000, "w");

                    syntax_abstract_tree_t *tree = load_syntax_tree(test_lex_input((char *) input.c_str()));
                    syntax_abstract_tree_print(output_fd, tree);

                    fseek(output_fd, 0, SEEK_SET);
                    fread(actual, expected_str.length() + 1, 1, output_fd);

                    EXPECT_STREQ(expected_str.c_str(), actual) << "Input: " << input;
                }

                void SyntaxTreeWithError(const std::string &input) {
                    syntax_abstract_tree_t *tree = load_syntax_tree(test_lex_input((char *) input.c_str()));
                    EXPECT_EQ(tree, nullptr);
                }
            };

            TEST_F(SyntaxAnalyzerTest, Assignment) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 12 + 32;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER});
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 12.2 + 32;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_FLOAT,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = \"Hello world!\";",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_STRING});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = null;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_KEYWORD_NULL});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = "),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = );"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = 2 3;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: INTEGER");
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionBasic) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 1 + 2;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 4 - $a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 4 - +$a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 12 * 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 12 * -3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER,
                                     SYN_NODE_NEGATE});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 33 / 11;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = 2 +"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = 2 +;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = 2 ++;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Increment operator is not supported");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = 2 --;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Decrement operator is not supported");
            }

            TEST_F(SyntaxAnalyzerTest, StringTest) {
                IsSyntaxTreeCorrect(R"(<?php declare(strict_types=1); $a = "Hello world";)",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_STRING});

                IsSyntaxTreeCorrect(R"(<?php declare(strict_types=1); $a = "Hello world" . "Hello world";)",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_STRING,
                                     SYN_NODE_CONCAT, SYN_NODE_STRING});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = \"Hello world\" . ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = . ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: .");
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionAdvanced) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); 1 + 2 * 3 / 12;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = (1 + 2) / (3 * 4);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = (2 + 4) /;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); $a = $a + 1 -;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");
            }

            TEST_F(SyntaxAnalyzerTest, IfConditions) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a !== 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_NOT_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if (1 < 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if (1 > 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if (1 <= 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_LESS_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if (1 >= 2) $a = 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_GREATER_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 3) if ($b !== 2) $c = 4;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_TYPED_NOT_EQUAL, SYN_NODE_INTEGER, SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 4) {"
                                    "  $b = 1;"
                                    "  $c = 2;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if("),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right parenthesis Expecting ), found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a)"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Incorrect if statement");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a) {"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right curly brackets Expecting }, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a) }"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: }");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a) {$a}"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected assignment or semicolon, got: }");
            }

            TEST_F(SyntaxAnalyzerTest, IfElseConditions) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 2) $a = 3; else $a = 4;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 2) {"
                                    " $a = 3;"
                                    "} else if ($a === 1) {"
                                    " $a = 4;"
                                    "} else {"
                                    " $a = 5;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); if ($a === 2) "
                                    "{"
                                    " $a = 3.4;"
                                    " $b = 4;"
                                    "} else {"
                                    " $a = 4;"
                                    " $b = $b - 1;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_FLOAT, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_SUB, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a) { $a = 1; } else"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement after else");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); if($a) { $a = 1; } else ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError(
                        "<?php declare(strict_types=1); if($a) { $a = 1; } else $a = 1; else $a = 2;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: else");

                EXPECT_EXIT(SyntaxTreeWithError(
                        "<?php declare(strict_types=1); if($a) { $a = 1; } else if ($a === 2 $a = 1; else $a = 2;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right parenthesis Expecting ), found: ID");

                EXPECT_EXIT(SyntaxTreeWithError(
                        "<?php declare(strict_types=1); if($a) { $a = 1; } else if ($a === 2) $a = 1; else"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement after else");
            }

            TEST_F(SyntaxAnalyzerTest, WhileLoop) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); while ($a > 0) $a = $a - 1;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_SUB, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); while ($a > 0) {$b = $b + 1; $a = $a - 1;}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_SEQUENCE,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_IDENTIFIER, SYN_NODE_SUB,
                                     SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left parenthesis Expecting \\(, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while(1 === 1)"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement after while");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while($a) }"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: }");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while($a);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while($a) $b = "),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); while(if()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: if");
            }

            TEST_F(SyntaxAnalyzerTest, FunctionCall) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); f();",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); f(1);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                     SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); f(1 + 2);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); f(1, 2, 3, 4, 5, 6, 7);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                     SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS,
                                     SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS,
                                     SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 1; f(1 + 2, $a);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f("),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f(g();"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f(1 2);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: )");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f(if($a)$b=2;);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: if");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f($a = 2);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: INTEGER");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f((());"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f(((1*)));"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); f(===);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ===");

                EXPECT_EXIT(SyntaxTreeWithError("hello\n"
                                                "<?php\n"
                                                "declare(strict_types=1);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] PHP Open bracket Expecting <\\?php, found: ID");
            }

            TEST_F(SyntaxAnalyzerTest, ReturnStatement) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); return $a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_KEYWORD_RETURN, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("<?php"
                                    "declare(strict_types=1);"
                                    "function a() {"
                                    "  return 1;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_SEQUENCE, SYN_NODE_KEYWORD_RETURN, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); return"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); return 1 +"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("<?php declare(strict_types=1); return ("),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");
            }

            TEST_F(SyntaxAnalyzerTest, FunctionDeclaration) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f() {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(): string {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a, $b) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a, string $b) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a, string $b): void {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(float $a, void $b): void {"
                                    " $a = $b;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a, string $b): float {"
                                    " $a = $b;"
                                    " $b = $a;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER});

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left parenthesis Expecting \\(, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting type or identifier, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a)", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a 2)", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: INTEGER");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f($a $a){}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: ID");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(12){}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting type or identifier, found: INTEGER");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int){}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Function argument declaration Expecting ID, found: \\)");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a):{}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting function return type, found: \\{");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a):string}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: \\}");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a):string", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); function f(int $a):string {", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right curly brackets Expecting \\}, found: EOF");
            }

            TEST_F(SyntaxAnalyzerTest, PHPBrackets) {
                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 1;", {
                        SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER
                });

                IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 1; ?>", {
                        SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER
                });

                IsSyntaxTreeCorrect("<? declare(strict_types=1); $a = 1; ?>", {
                        SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER
                });

                EXPECT_EXIT(IsSyntaxTreeCorrect("efef<?php $a = 1; ?>;", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] PHP Open bracket Expecting <\\?php, found: ID");

                EXPECT_EXIT(IsSyntaxTreeCorrect("233<?php $a = 1; ?>;", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] PHP Open bracket Expecting <\\?php, found: INTEGER");

                EXPECT_EXIT(IsSyntaxTreeCorrect("<?php declare(strict_types=1); $a = 1; ?>;", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected end of file, got: ;");
                EXPECT_EXIT(IsSyntaxTreeCorrect("write(\\\"Hello\\\");", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] PHP Open bracket Expecting <\\?php, found: ID");
            }

            TEST_F(SyntaxAnalyzerTest, ExternalTests) {
                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "5;", {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "$x = 5;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "reads();\n"
                                    "readi();\n"
                                    "readf();\n"
                                    "write(5);\n"
                                    "write(\"hello world\", 99);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_SEQUENCE,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_CALL, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL,
                                     SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_CALL, SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_STRING, SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "if(1===1){}else{}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "if(1===1) {\n"
                                    "    $x = 5;\n"
                                    "    $y = 7;\n"
                                    "} else {\n"
                                    "    $x = 6;\n"
                                    "    $y = 8;\n"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "if(1===1) {\n"
                                    "    $x = 5;\n"
                                    "} else {\n"
                                    "    $x = 6;\n"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "return;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_KEYWORD_RETURN, SYN_NODE_KEYWORD_VOID});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "return 0;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_KEYWORD_RETURN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "return 1+2;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_KEYWORD_RETURN, SYN_NODE_INTEGER, SYN_NODE_ADD,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("<?php\n"
                                    "declare(strict_types=1);\n"
                                    "while(1===0){}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_TYPED_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE});
            }
        }
    }
}