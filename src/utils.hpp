#pragma once

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum LogLevel { INFO, WARNING, ERROR };

inline void log(const std::string &message, LogLevel level = INFO) {
    switch (level) {
    case INFO:
        std::cout << "taskr: INFO: " << message << std::endl;
        break;
    case WARNING:
        std::cout << "taskr: WARNING: " << message << std::endl;
        break;
    case ERROR:
        std::cerr << "taskr: ERROR: " << message << std::endl;
        break;
    }
}

inline std::string trim(const std::string &str) {
    std::string::const_iterator start_it = str.begin();

    while (start_it != str.end() && std::isspace(*start_it)) {
        ++start_it;
    }

    std::string::const_iterator end_it = str.end();

    do {
        --end_it;
    } while (end_it != start_it && std::isspace(*end_it));

    return std::string(start_it, end_it + 1);
}

inline bool has_quotes(const std::string &str) {
    return (str.front() == '"' && str.back() == '"') ||
           (str.front() == '\'' && str.back() == '\'');
}

inline std::string trim_quotes(const std::string &str) {
    if (str.size() > 1) {
        char first = str.front();
        char last = str.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return str.substr(1, str.size() - 2);
        }
    }
    return str;
}

inline bool has_brackets(const std::string &str) {
    return (str.front() == '[' && str.back() == ']');
}

inline std::string trim_brackets(const std::string &str) {
    if (str.size() > 1) {
        char first = str.front();
        char last = str.back();
        if (first == '[' && last == ']') {
            return str.substr(1, str.size() - 2);
        }
    }
    return str;
}

inline bool contains_space(const std::string &str) {
    for (char c : str) {
        if (std::isspace(c))
            return 1;
    }
    return 0;
}

inline std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}
