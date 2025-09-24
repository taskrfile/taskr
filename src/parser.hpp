#pragma once

#include "config.h"
#include "errors.hpp"
#include "util.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum TaskrParseState { START, IN_TASK, IN_ENV };

class TaskrParser {
  public:
    Config parse_lines(const std::vector<std::string> &lines) {
        definedTaskNames.clear();
        definedEnvNames.clear();
        currentTask = {};
        currentEnv = {};
        currentBlockName = "";

        Config config;
        int line_number = 0;

        for (const std::string &raw_line : lines) {
            ++line_number;
            std::string line = raw_line;

            if (is_comment(line) || line.empty()) {
                continue;
            }

            line = strip_inline_comment(line);

            std::smatch match;

            if (std::regex_match(line, match, default_env_header_regex)) {
                if (state == IN_TASK && !currentTask.name.empty()) {
                    validate_task(currentTask);
                    config.tasks[currentTask.name] = currentTask;
                    currentTask = Task{};
                } else if (state == IN_ENV && !currentEnv.name.empty()) {
                    validate_env(currentEnv);
                    config.environments[currentEnv.name] = currentEnv;
                    currentEnv = Environment{};
                }

                if (config.hasDefaultEnv) {
                    throw ParseError("More than 1 default environment found");
                }
                config.hasDefaultEnv = true;
                state = IN_ENV;

                currentEnv = Environment{};
                currentEnv.name = match[2];
                currentEnv.isDefault = true;
                currentBlockName = currentEnv.name;

                continue;
            }

            if (std::regex_match(line, match, env_header_regex)) {
                if (state == IN_TASK && !currentTask.name.empty()) {
                    validate_task(currentTask);
                    config.tasks[currentTask.name] = currentTask;
                    currentTask = Task{};
                } else if (state == IN_ENV && !currentEnv.name.empty()) {
                    validate_env(currentEnv);
                    config.environments[currentEnv.name] = currentEnv;
                    currentEnv = Environment{};
                }

                state = IN_ENV;

                currentEnv = Environment{};
                currentEnv.name = match[2];
                currentBlockName = currentEnv.name;

                continue;
            }

            if (std::regex_match(line, match, task_header_regex)) {
                if (state == IN_TASK && !currentTask.name.empty()) {
                    validate_task(currentTask);
                    config.tasks[currentTask.name] = currentTask;
                    currentTask = Task{};
                } else if (state == IN_ENV && !currentEnv.name.empty()) {
                    validate_env(currentEnv);
                    config.environments[currentEnv.name] = currentEnv;
                    currentEnv = Environment{};
                }

                state = IN_TASK;

                currentTask = Task{};
                currentTask.name = match[1];
                currentBlockName = currentTask.name;

                continue;
            }

            if (state == START) {
                throw ParseError("No block headers found");
            }

            handle_kv_line(line);
        }

        if (state == IN_TASK && !currentTask.name.empty()) {
            validate_task(currentTask);
            config.tasks[currentTask.name] = currentTask;
        }

        if (state == IN_ENV && !currentEnv.name.empty()) {
            validate_env(currentEnv);
            config.environments[currentEnv.name] = currentEnv;
        }

        return config;
    }

    std::unordered_set<std::string> get_task_names_and_aliases() { return definedTaskNames; };

  private:
    TaskrParseState state = START;
    Task currentTask;
    Environment currentEnv;
    std::string currentBlockName;

    std::unordered_set<std::string> definedTaskNames;
    std::unordered_set<std::string> definedEnvNames;

    std::regex comment_regex{R"(\s*//.*)"};

    std::regex task_header_regex{R"(task\s+([a-zA-Z_][\w\-]*)\s*:\s*(.*))"};
    std::regex task_kv_regex{R"(^(  )(run|desc|alias|needs)\s*=\s*(.+))"};

    std::regex env_header_regex{R"(\s*(env)\s+([a-zA-Z_][\w\-]*)\s*:\s*(.*))"};
    std::regex default_env_header_regex{R"(\s*(default env)\s+([a-zA-Z_][\w\-]*)\s*:\s*(.*))"};
    std::regex env_kv_regex{R"(^(  )(file)\s*=\s*(.+))"};

    bool is_comment(const std::string &line) const { return std::regex_match(line, comment_regex); }

    bool is_task_header(const std::string &line) const { return std::regex_match(line, task_header_regex); }
    bool is_env_header(const std::string &line) const { return std::regex_match(line, env_header_regex); }
    bool is_default_env_header(const std::string &line) const {
        return std::regex_match(line, default_env_header_regex);
    }

    bool is_task_kv(const std::string &line) const { return std::regex_match(line, task_kv_regex); }
    bool is_env_kv(const std::string &line) const { return std::regex_match(line, env_kv_regex); }

    void handle_kv_line(const std::string &line) {
        std::smatch match;

        if (state == IN_TASK && std::regex_match(line, match, task_kv_regex)) {
            std::string key = match[2];
            std::string value = trim_whitespace(match[3]);

            if (key == "run")
                currentTask.run = value;
            else if (key == "desc")
                currentTask.desc = value;
            else if (key == "alias")
                currentTask.alias = split(value, ',');
            else if (key == "needs")
                currentTask.needs = split(value, ',');
        }

        if (state == IN_ENV && std::regex_match(line, match, env_kv_regex)) {
            std::string key = match[2];
            std::string value = trim_whitespace(match[3]);

            if (key == "file")
                currentEnv.file = value;
        }
    };

    void validate_task(const Task &task) {
        if (definedTaskNames.count(task.name)) {
            throw ParseError("Task '" + task.name + "' is defined more than once");
        }

        for (const std::string &alias : task.alias) {
            if (alias == task.name) {
                throw ParseError("Alias '" + alias + "' cannot be the same as the task name '" + task.name + "'");
            }
        }

        for (const std::string &alias : task.alias) {
            if (definedTaskNames.count(alias)) {
                throw ParseError("Alias '" + alias + "' is used more than once or is a task");
            }
        }

        if (task.run.empty()) {
            throw ParseError("Task '" + task.name + "' is missing required key: 'run'");
        }

        definedTaskNames.insert(task.name);

        for (const std::string &alias : task.alias) {
            definedTaskNames.insert(alias);
        }

        for (const std::string &dependency : task.needs) {
            if (!definedTaskNames.count(dependency)) {
                throw ParseError("Dependency '" + dependency + "' could not be resolved");
            }
        }
    }

    void validate_env(const Environment &env) {
        if (definedEnvNames.count(env.name)) {
            throw ParseError("Environment '" + env.name + "' is defined more than once");
        }

        if (env.file.empty()) {
            throw ParseError("Environment '" + env.name + "' is missing required key: 'file'");
        }

        definedEnvNames.insert(env.name);
    }
};

class EnvParser {
  public:
    void load_env(const std::vector<std::string> &lines, const std::string &filename) {
        for (const std::string &line : lines) {
            if (line.empty() || line[0] == '#' || line[0] == ';') {
                continue;
            }

            auto delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) {
                throw ParseError("Invalid line format in environment file: '" + filename + "': " + line);
            }

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            key = trim_whitespace(key);
            value = trim_whitespace(value);

            data[key] = value;
        }

        for (auto kv : data) {
            set_env_var(kv.first, kv.second);
        };
    };

  private:
    std::unordered_map<std::string, std::string> data;

    void set_env_var(const std::string &key, const std::string &value) { setenv(key.c_str(), value.c_str(), 1); }
};
