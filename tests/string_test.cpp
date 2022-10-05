#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
extern "C" {
    #include "../src/string.h"
    #include "../src/string.c"
}

TEST(string, append_char) {
    string_t *str = string_init("");
    string_append_char(str, 'w');
    EXPECT_STREQ(str->value, "w");
}

TEST(string, append_string) {
    string_t *str = string_init("Hello");
    string_append_string(str, " world");
    EXPECT_STREQ(str->value, "Hello world");
}

TEST(string, clear) {
    string_t *str = string_init("Hello world");
    string_clear(str);
    EXPECT_STREQ(str->value, "");
}