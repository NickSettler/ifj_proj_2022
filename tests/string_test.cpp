#include <gtest/gtest.h>

extern "C" {
#include "../src/str.h"
#include "../src/str.c"
}

namespace ifj {
    namespace tests {
        namespace {
            class StringTest : public ::testing::Test {
            protected:
                StringTest() = default;

                void SetUp() override {
                }
            };

            TEST(String, AppendChar) {
                string_t *str = string_init("");
                string_append_char(str, 'w');
                EXPECT_STREQ(str->value, "w");
            }

            TEST(String, AppendString) {
                string_t *str = string_init("Hello");
                string_append_string(str, " world");
                EXPECT_STREQ(str->value, "Hello world");
            }

            TEST(String, Clear) {
                string_t *str = string_init("Hello world");
                string_clear(str);
                EXPECT_STREQ(str->value, "");
            }

            TEST(String, StringCheckBy) {
                string_t *str = string_init("12345");
                bool is_digit = string_check_by(str, isdigit);
                EXPECT_EQ(is_digit, true);
            }

            TEST(String, StringConvertBy) {
                string_t *str = string_init("Hello World");
                string_convert_by(str, tolower);
                EXPECT_STREQ(str->value, "hello world");
            }
        }
    }
}
