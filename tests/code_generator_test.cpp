#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

extern "C" {
#include "../src/code_generator.c"
#include "../src/code_generator.h"
}

namespace ifj {
    namespace tests {
        namespace {
            class CodeGeneratorTest : public ::testing::Test {
            protected:
                CodeGeneratorTest() = default;

                void SetUp() override {
                }
            };
        }
    }
}
