#pragma once

#include <stdexcept>

class TaskrError : public std::runtime_error {
  public:
    TaskrError(const std::string &msg) : std::runtime_error("TaskrError: " + msg) {}
};

class ParseError : public TaskrError {
  public:
    ParseError(const std::string &msg) : TaskrError("Parse error: " + msg) {}
};

class FileNotFoundError : public TaskrError {
  public:
    FileNotFoundError(const std::string &msg) : TaskrError("File not found: " + msg) {}
};

class MultiConfigError : public TaskrError {
  public:
    MultiConfigError(const std::string &msg) : TaskrError("Multiple configurations found: " + msg) {}
};

class FormatError : public TaskrError {
  public:
    FormatError() : TaskrError("Wrong command format") {}
};
