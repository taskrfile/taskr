#pragma once

#include <format>
#include <stdexcept>

// Main error class
class TaskrError : public std::runtime_error {
  public:
    explicit TaskrError(const std::string &msg) : std::runtime_error(std::format("TaskrError: {}", msg)) {}
};

// CLI errors
class ArgError : public TaskrError {
  public:
    explicit ArgError() : TaskrError("Wrong command format") {}
};

// Configuration errors
class MultiConfigError : public TaskrError {
  public:
    explicit MultiConfigError(const std::string &msg)
        : TaskrError(std::format("Multiple configurations found: {}", msg)) {}
};

class FileNotFoundError : public TaskrError {
  public:
    explicit FileNotFoundError(const std::string &msg) : TaskrError(std::format("File not found: {}", msg)) {}
};

// Parser Errors
class ParseError : public TaskrError {
  public:
    explicit ParseError(const std::string &msg) : TaskrError(std::format("Parse error: {}", msg)) {}
};
