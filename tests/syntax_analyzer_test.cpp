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
                IsSyntaxTreeCorrect("$a = 12 + 32;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("$a = "), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("$a = ;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("$a = );"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("$a = 2 3;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: INTEGER");
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionBasic) {
                IsSyntaxTreeCorrect("1 + 2;", {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("4 - $a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("4 - +$a;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_SUB, SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("12 * 3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("12 * -3;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_MUL, SYN_NODE_INTEGER,
                                     SYN_NODE_NEGATE});

                IsSyntaxTreeCorrect("33 / 11;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("$a = 2 +"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("$a = 2 +;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("$a = 2 ++;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Increment operator is not supported");

                EXPECT_EXIT(SyntaxTreeWithError("$a = 2 --;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Decrement operator is not supported");
            }

            TEST_F(SyntaxAnalyzerTest, StringTest) {
                IsSyntaxTreeCorrect("$a = \"Hello world\";",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_STRING});

                IsSyntaxTreeCorrect("$a = \"Hello world\" . \"Hello world\";",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_STRING,
                                     SYN_NODE_CONCAT, SYN_NODE_STRING});

                EXPECT_EXIT(SyntaxTreeWithError("$a = \"Hello world\" . ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("$a = . ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: .");
            }

            TEST_F(SyntaxAnalyzerTest, ArithmeticExpressionAdvanced) {
                IsSyntaxTreeCorrect("1 + 2 * 3 / 12;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("$a = (1 + 2) / (3 * 4);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_DIV, SYN_NODE_INTEGER, SYN_NODE_MUL,
                                     SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("$a = (2 + 4) /;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("$a = $a + 1 -;"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ;");
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

                EXPECT_EXIT(SyntaxTreeWithError("if("), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("if($a"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right parenthesis Expecting ), found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("if($a)"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Incorrect if statement");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) {"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right curly brackets Expecting }, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) }"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: }");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) {}"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Incorrect if statement");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) {$a}"), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Assignment Expecting =, found: }");
            }

            TEST_F(SyntaxAnalyzerTest, IfElseConditions) {
                IsSyntaxTreeCorrect("if ($a == 2) $a = 3; else $a = 4;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if ($a == 2) {"
                                    " $a = 3;"
                                    "} else if ($a == 1) {"
                                    " $a = 4;"
                                    "} else {"
                                    " $a = 5;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("if ($a == 2) "
                                    "{"
                                    " $a = 3.4;"
                                    " $b = 4;"
                                    "} else {"
                                    " $a = 4;"
                                    " $b = $b - 1;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_EQUAL, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_IF, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_FLOAT, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_INTEGER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_SUB, SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("if($a) { $a = 1; } else"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement after else");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) { $a = 1; } else ;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) { $a = 1; } else $a = 1; else $a = 2;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: else");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) { $a = 1; } else if ($a == 2 $a = 1; else $a = 2;"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right parenthesis Expecting ), found: ID");

                EXPECT_EXIT(SyntaxTreeWithError("if($a) { $a = 1; } else if ($a == 2) $a = 1; else"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement after else");
            }

            TEST_F(SyntaxAnalyzerTest, WhileLoop) {
                IsSyntaxTreeCorrect("while ($a > 0) $a = $a - 1;",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_WHILE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_SUB, SYN_NODE_INTEGER});

                IsSyntaxTreeCorrect("while ($a > 0) {$b = $b + 1; $a = $a - 1;}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_GREATER, SYN_NODE_INTEGER,
                                     SYN_NODE_KEYWORD_WHILE, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_SEQUENCE,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_IDENTIFIER, SYN_NODE_SUB,
                                     SYN_NODE_INTEGER});

                EXPECT_EXIT(SyntaxTreeWithError("while"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left parenthesis Expecting \\(, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("while()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("while($a) }"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: }");

                EXPECT_EXIT(SyntaxTreeWithError("while($a);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected statement, got: ;");

                EXPECT_EXIT(SyntaxTreeWithError("while($a) $b = "),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("while(if()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: if");
            }

            TEST_F(SyntaxAnalyzerTest, FunctionCall) {
                IsSyntaxTreeCorrect("f();", {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL});

                IsSyntaxTreeCorrect("f(1);", {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                              SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("f(1 + 2);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                     SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("f(1, 2, 3, 4, 5, 6, 7);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_INTEGER,
                                     SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS,
                                     SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS,
                                     SYN_NODE_INTEGER, SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                IsSyntaxTreeCorrect("$a = 1; f(1 + 2, $a);",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN, SYN_NODE_INTEGER,
                                     SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_CALL, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_ARGS, SYN_NODE_INTEGER, SYN_NODE_ADD, SYN_NODE_INTEGER, SYN_NODE_ARGS});

                EXPECT_EXIT(SyntaxTreeWithError("f("),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("f()"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: EOF");

                EXPECT_EXIT(SyntaxTreeWithError("f(g();"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: )");

                EXPECT_EXIT(SyntaxTreeWithError("f(1 2);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: )");

                EXPECT_EXIT(SyntaxTreeWithError("f(if($a)$b=2;);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: if");

                EXPECT_EXIT(SyntaxTreeWithError("f($a = 2);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Semicolon Expecting ;, found: INTEGER");

                EXPECT_EXIT(SyntaxTreeWithError("f((());"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("f(((1*)));"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: )");

                EXPECT_EXIT(SyntaxTreeWithError("f(==);"),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expected expression, got: ==");
            }

            TEST_F(SyntaxAnalyzerTest, FunctionDeclaration) {
                IsSyntaxTreeCorrect("function f() {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION});

                IsSyntaxTreeCorrect("function f(): string {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION});

                IsSyntaxTreeCorrect("function f($a) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("function f($a, $b) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("function f(int $a) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("function f(int $a, string $b) {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("function f(int $a, string $b): void {}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG});

                IsSyntaxTreeCorrect("function f(float $a, void $b): void {"
                                    " $a = $b;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER});

                IsSyntaxTreeCorrect("function f(int $a, string $b): float {"
                                    " $a = $b;"
                                    " $b = $a;"
                                    "}",
                                    {SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_DECLARATION,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_FUNCTION_ARG, SYN_NODE_IDENTIFIER,
                                     SYN_NODE_FUNCTION_ARG, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER, SYN_NODE_SEQUENCE, SYN_NODE_IDENTIFIER, SYN_NODE_ASSIGN,
                                     SYN_NODE_IDENTIFIER});

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left parenthesis Expecting \\(, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting type or identifier, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f($a", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f($a)", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f($a 2)", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: INTEGER");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f($a $a){}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting ',' or '\\)', found: ID");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(12){}", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting type or identifier, found: INTEGER");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(int){}", {}), ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Function argument declaration Expecting ID, found: \\)");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(int $a):{}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Expecting function return type, found: \\{");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(int $a):string}", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: \\}");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(int $a):string", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Left curly brackets Expecting \\{, found: EOF");

                EXPECT_EXIT(IsSyntaxTreeCorrect("function f(int $a):string {", {}),
                            ::testing::ExitedWithCode(SYNTAX_ERROR_CODE),
                            "\\[SYNTAX ERROR\\] Right curly brackets Expecting \\}, found: EOF");
            }
        }
    }
}