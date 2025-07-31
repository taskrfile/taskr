#include <gtest/gtest.h>
#include <vector>
#include "util.hpp"

std::string original;

TEST(UtilTest, ToLowerCase){
    original = "Hello World!";
    EXPECT_EQ(to_lowercase(original), "hello world!");
}

TEST(UtilTest, TrimWhiteSpace){
    original = "     Hello World! ";
    EXPECT_EQ(trim_whitespace(original), "Hello World!");
}

TEST(UtilTest, Split){
    original = "key=value";
    std::vector<std::string> splitted = split(original, '=');

    EXPECT_EQ(splitted.size(), 2);
    EXPECT_EQ(splitted[0], "key");
    EXPECT_EQ(splitted[1], "value");
}

TEST(UtilTest, StripInlineComment){
    original = "Hello World! // this is a comment";
    EXPECT_EQ(strip_inline_comment(original), "Hello World! ");

    original = "Hello World! / this is not a comment";
    EXPECT_EQ(strip_inline_comment(original), original);
}
