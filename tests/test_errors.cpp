#include "errors.hpp"
#include <gtest/gtest.h>

TEST(ErrorTest, TaskrErrorTest) {
    TaskrError err("Something went wrong");
    EXPECT_STREQ(err.what(), "TaskrError: Something went wrong");
}

TEST(ErrorTest, ArgErrorTest) {
    ArgError err;
    EXPECT_STREQ(err.what(), "TaskrError: Wrong command format");
}

TEST(ErrorTest, MultiConfigErrorTest) {
    MultiConfigError err("taskrfile, taskrfile");
    EXPECT_STREQ(err.what(), "TaskrError: Multiple configurations found: taskrfile, taskrfile");
}

TEST(ErrorTest, FileNotFoundErrorTest) {
    FileNotFoundError err("config.json");
    EXPECT_STREQ(err.what(), "TaskrError: File not found: config.json");
}

TEST(ErrorTest, ParseErrorTest) {
    ParseError err("unexpected token at line 3");
    EXPECT_STREQ(err.what(), "TaskrError: Parse error: unexpected token at line 3");
}
