cmake_minimum_required(VERSION 3.23)
project(ifj_proj)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

add_executable(
        AllTests
        tests/main.cpp
        tests/string_test.cpp
        tests/lexical_fsm_test.cpp
        tests/syntax_analyzer_test.cpp
        tests/semantic_analysis_test.cpp
        tests/optimiser_test.cpp)

target_link_libraries(
        AllTests
        PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(AllTests)

add_executable(ifj_proj src/main.c src/errors.c src/errors.h src/lexical_fsm.c src/lexical_fsm.h src/str.c src/str.h src/code_generator.c src/code_generator.h src/syntax_analyzer.c src/syntax_analyzer.h src/symtable.c src/symtable.h src/semantic_analyzer.c src/semantic_analyzer.h src/optimiser.c src/optimiser.h)
