#include "config.h"
#include "parser.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

TaskrParser parser;
EnvParser envParser;
Config config;
std::vector<std::string> lines;

// --- TaskrParser
TEST(ParserTest, EmptyFileTest) {
    lines = {};

    config = parser.parse_lines(lines);

    // Check config
    EXPECT_EQ(config.tasks.size(), 0);
    EXPECT_EQ(config.environments.size(), 0);
    EXPECT_FALSE(config.hasDefaultEnv);
}

TEST(ParserTest, NoBlockHeaderTest) {
    lines = {"  run = echo nothing"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: No block headers found");
    }
}

TEST(ParserTest, ValidTaskTest) {
    lines = {
        "task build:",
        "  run = echo build",
    };
    config = parser.parse_lines(lines);

    // Check config
    EXPECT_EQ(config.tasks.size(), 1);
    EXPECT_EQ(config.environments.size(), 0);
    EXPECT_FALSE(config.hasDefaultEnv);

    // Check task
    EXPECT_TRUE(config.tasks.count("build"));
    const Task &task = config.tasks.at("build");
    EXPECT_EQ(task.name, "build");
    EXPECT_EQ(task.run, "echo build");
    EXPECT_EQ(task.desc, "");
    EXPECT_TRUE(task.alias.empty());
    EXPECT_TRUE(task.needs.empty());
}

TEST(ParserTest, ValidFullTaskTest) {
    lines = {"task build:",
             "  run = echo build",
             "",
             "task install:",
             "  run   = echo install",
             "  desc  = prints install",
             "  needs = build",
             "  alias = i"};
    config = parser.parse_lines(lines);

    // Check config
    EXPECT_EQ(config.tasks.size(), 2);
    EXPECT_EQ(config.environments.size(), 0);
    EXPECT_FALSE(config.hasDefaultEnv);

    // Check task
    EXPECT_TRUE(config.tasks.count("build"));
    EXPECT_TRUE(config.tasks.count("install"));
    const Task &task = config.tasks.at("install");
    EXPECT_EQ(task.name, "install");
    EXPECT_EQ(task.run, "echo install");
    EXPECT_EQ(task.desc, "prints install");
    EXPECT_EQ(task.alias, std::vector<std::string>{"i"});
    EXPECT_EQ(task.needs, std::vector<std::string>{"build"});
}

TEST(ParserTest, NoRunTaskTest) {
    lines = {"task build:", "desc = build task"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Task 'build' is missing required key: 'run'");
    }
}

TEST(ParserTest, DoubleTaskTest) {
    lines = {"task build:", "  run = echo build", "task build:", "  run = echo build"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Task 'build' is defined more than once");
    }
}

TEST(ParserTest, AliasIsNameTaskTest) {
    lines = {"task build:", "  run = echo build", "  alias = build"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Alias 'build' cannot be the same as the task name 'build'");
    }
};

TEST(ParserTest, AliasIsNameOtherTaskTest) {
    lines = {"task build:",
             "  run = echo build",
             "task create:",
             "  run = echo create",
             "  alias = build"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Alias 'build' is used more than once or is a task");
    }
};

TEST(ParserTest, UnresolvedDependencyTaskTest) {
    lines = {"task build:", "  run = echo build", "  needs = greeting"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Dependency 'greeting' could not be resolved");
    }
};

TEST(ParserTest, ValidEnvTest) {
    lines = {"env dev:", "  file = .env"};
    config = parser.parse_lines(lines);

    // Check config
    EXPECT_EQ(config.tasks.size(), 0);
    EXPECT_EQ(config.environments.size(), 1);
    EXPECT_FALSE(config.hasDefaultEnv);

    // Check env
    EXPECT_TRUE(config.environments.count("dev"));
    const Environment &env = config.environments.at("dev");
    EXPECT_EQ(env.name, "dev");
    EXPECT_EQ(env.file, ".env");
    EXPECT_FALSE(env.isDefault);
}

TEST(ParserTest, ValidDefaultEnvTest) {
    lines = {"default env dev:", "  file = .env"};
    config = parser.parse_lines(lines);

    // Check config
    EXPECT_EQ(config.tasks.size(), 0);
    EXPECT_EQ(config.environments.size(), 1);
    EXPECT_TRUE(config.hasDefaultEnv);

    // Check env
    EXPECT_TRUE(config.environments.count("dev"));
    const Environment &env = config.environments.at("dev");
    EXPECT_EQ(env.name, "dev");
    EXPECT_EQ(env.file, ".env");
    EXPECT_TRUE(env.isDefault);
}

TEST(ParserTest, NoFileEnvTest) {
    lines = {"env dev:"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Environment 'dev' is missing required key: 'file'");
    }
}

TEST(ParserTest, DoubleEnvTest) {
    lines = {"env dev:", "  file = .env", "env dev:", "  file = dev.env"};

    EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);

    try {
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: Environment 'dev' is defined more than once");
    }
}

TEST(ParserTest, MultipleDefaultEnvTest) {
    lines = {"default env dev:", "  file = .env", "default env test:", "  file = test.env"};

    try {
        EXPECT_THROW({ parser.parse_lines(lines); }, ParseError);
        parser.parse_lines(lines);
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(), "TaskrError: Parse error: More than 1 default environment found");
    }
}

// --- EnvParser
TEST(ParserTest, EnvFileTest) {
    // Valid
    lines = {"KEY=value", "# Comment", "; Another comment", "TASKRUSER = JohnDoe"};

    ASSERT_NO_THROW(envParser.load_env(lines, "imaginary.env"));
    EXPECT_EQ(std::getenv("KEY"), "value");
    EXPECT_EQ(std::getenv("TASKRUSER"), "JohnDoe");

    // Invalid
    lines = {"INVALID_LINE"};
    EXPECT_THROW(envParser.load_env(lines, "imaginary.env"), ParseError);

    try {
        envParser.load_env(lines, "imaginary.env");
    } catch (const ParseError &e) {
        EXPECT_STREQ(e.what(),
                     "TaskrError: Parse error: Invalid line format in environment file: 'imaginary.env': INVALID_LINE");
    }
}
