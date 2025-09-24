#include "errors.hpp"
#include "executor.hpp"
#include "parser.hpp"
#include "util.hpp"
#include <fstream>
#include <format>
#include <iostream>
#include <ostream>
#include <unordered_set>

void print_help() {
    std::cout << R"(Usage:
  taskr <task_name> [options]

Options:
  -h, --help                Show this help message and exit
  -l, --list                List the available tasks
  -e, --environment name    Select the environment to use
)";
}

void print_config(const Config &config) {
    size_t max_task_name_len = 0;
    for (const auto &c : config.tasks) {
        max_task_name_len = std::max(max_task_name_len, c.second.name.length());
    }

    if (!config.tasks.empty()) {
        std::cout << "Tasks:" << std::endl;
        for (const auto &t : config.tasks) {
            size_t padding = max_task_name_len - t.second.name.length();
            std::cout << "  " << t.second.name << std::string(padding + 4, ' ') << t.second.desc << std::endl;
        }
    }

    if (!config.environments.empty()) {
        size_t max_env_name_len = 0;
        for (const auto &e : config.environments) {
            max_env_name_len = std::max(max_env_name_len, e.second.name.length());
        }

        std::cout << std::endl << "Environments:" << std::endl;
        for (const auto &e : config.environments) {
            size_t padding = max_env_name_len - e.second.name.length();
            if (e.second.isDefault) {
                std::cout << "* " << e.second.name << std::string(padding + 4, ' ') << e.second.file << std::endl;
            } else {
                std::cout << "  " << e.second.name << std::string(padding + 4, ' ') << e.second.file << std::endl;
            }
        }
    }

    if (config.tasks.empty() && config.environments.empty())
        std::cout << "Config file is empty" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        print_help();
        return 0;
    }

    try {
        const std::string filename = check_unique_case_insensitive_match("taskrfile");

        if (filename.find(".config/taskr") != std::string::npos) {
            std::cout << "Taskr: Using global config" << std::endl << std::endl;
        }

        TaskrParser parser;
        EnvParser envParser;
        TaskrExecutor executor;

        std::string taskName;
        std::string envName;

        std::ifstream file(filename);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        Config config = parser.parse_lines(lines);

        if (argc != 2 && argc != 4) {
            throw ArgError();
        }

        if (argc == 2 && (std::string(argv[1]) == "-l" || std::string(argv[1]) == "--list")) {
            print_config(config);
            return 0;
        }

        if (argc == 2) {
            taskName = argv[1];

            if (config.hasDefaultEnv) {
                for (auto kv : config.environments) {
                    if (kv.second.isDefault) {
                        std::ifstream file(kv.second.file);
                        if (!file.good()) {
                            throw FileNotFoundError(kv.second.file);
                        }
                        std::vector<std::string> lines;
                        std::string line;
                        while (std::getline(file, line)) {
                            lines.push_back(line);
                        }
                        envParser.load_env(lines, kv.second.file);
                    }
                }
            }
        }

        if (argc == 4) {
            if ((std::string(argv[1]) == "-e" || std::string(argv[1]) == "--environment")) {
                envName = argv[2];
                taskName = argv[3];
            };

            if ((std::string(argv[2]) == "-e" || std::string(argv[2]) == "--environment")) {
                envName = argv[3];
                taskName = argv[1];
            };

            if (config.environments.find(envName) == config.environments.end()) {
                throw TaskrError(std::format("No environment '{}' found in config", envName));
            }

            std::ifstream file(config.environments.at(envName).file);
            if (!file.good()) {
                throw FileNotFoundError(config.environments.at(envName).file);
            }
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(file, line)) {
                lines.push_back(line);
            }
            envParser.load_env(lines, config.environments.at(envName).file);
        };

        std::unordered_set<std::string> definedTasksAndAliases = parser.get_task_names_and_aliases();

        auto it = std::find(definedTasksAndAliases.begin(), definedTasksAndAliases.end(), taskName);

        if (it == definedTasksAndAliases.end()) {
            throw TaskrError(std::format("Task or alias '{}' not found", taskName));
        }

        executor.execute(config, taskName);

    } catch (const ArgError &e) {
        std::cerr << e.what() << "\n\n";
        print_help();
        return 1;
    } catch (const TaskrError &e) {
        std::cerr << e.what() << '\n';
        return 2;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected error: " << e.what() << '\n';
        return 3;
    }

    return 0;
}
