#pragma once

#include "errors.hpp"
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

inline std::string to_lowercase(const std::string &str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline std::vector<std::string> find_case_insensitive_files(const std::string &target, const std::string path) {
    std::vector<std::string> matches;
    const std::string target_lower = to_lowercase(target);

    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (to_lowercase(filename) == target_lower) {
                matches.push_back(filename);
            }
        }
    }
    return matches;
}

inline std::string get_global_config(const std::string &target) {
    std::string homeDir = getenv("HOME");

    auto matches = find_case_insensitive_files(target, homeDir + "/.config/taskr");

    if (matches.empty())
        throw FileNotFoundError("No \"" + target + "\" found.");

    if (matches.size() > 1)
        throw MultiConfigError("Multiple global files found with case-insensitive match to \"" + target + "\".");

    return homeDir + "/.config/taskr/" + matches.front();
}

inline std::string check_unique_case_insensitive_match(const std::string &target) {
    auto matches = find_case_insensitive_files(target, fs::current_path());

    if (matches.empty()) {
        return get_global_config(target);
    }

    if (matches.size() > 1) {
        throw MultiConfigError("Multiple files found with case-insensitive match to \"" + target + "\".");
    }

    return matches.front();
}

inline std::string trim_whitespace(const std::string &str) {
    std::size_t first = str.find_first_not_of(" \t");
    std::size_t last = str.find_last_not_of(" \t");
    if (first == std::string::npos) {
        return "";
    }
    return str.substr(first, last - first + 1);
}

inline std::vector<std::string> split(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim_whitespace(token));
    }
    return tokens;
}

inline std::string strip_inline_comment(const std::string &line) {
    std::size_t comment_pos = line.find("//");
    if (comment_pos != std::string::npos) {
        return line.substr(0, comment_pos);
    }
    return line;
}
