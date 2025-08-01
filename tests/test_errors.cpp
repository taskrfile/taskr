#include "errors.hpp"
#include <gtest/gtest.h>

TEST(ErrorTest, TaskrErrorTest) {
    TaskrError e("Something went wrong");
    EXPECT_STREQ(e.what(), "TaskrError: Something went wrong");
}

TEST(ErrorTest, ArgErrorTest) {
    ArgError e;
    EXPECT_STREQ(e.what(), "TaskrError: Wrong command format");
}

TEST(ErrorTest, MultiConfigErrorTest) {
    MultiConfigError e("taskrfile, taskrfile");
    EXPECT_STREQ(e.what(), "TaskrError: Multiple configurations found: taskrfile, taskrfile");
}

TEST(ErrorTest, FileNotFoundErrorTest) {
    FileNotFoundError e("config.json");
    EXPECT_STREQ(e.what(), "TaskrError: File not found: config.json");
}

TEST(ErrorTest, ParseErrorTest) {
    ParseError e("unexpected token at line 3");
    EXPECT_STREQ(e.what(), "TaskrError: Parse error: unexpected token at line 3");
}
