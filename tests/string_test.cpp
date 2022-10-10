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
        }
    }
}
