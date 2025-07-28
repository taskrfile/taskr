#include "errors.hpp"
#include "executor.hpp"
#include "parser.hpp"
#include "util.hpp"
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
}

int main(int argc, char *argv[]) {
    if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        print_help();
        return 0;
    }

    try {
        const std::string filename = check_unique_case_insensitive_match("taskrfile");

        TaskrParser parser;
        EnvParser envParser;
        TaskrExecutor executor;

        std::string taskName;
        std::string envName;

        const Config config = parser.parse_file(filename);

        if (argc != 2 && argc != 4) {
            throw FormatError();
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
                        envParser.load_env_file(kv.second.file);
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
                throw TaskrError("No environment '" + envName + "' found in config");
            }

            envParser.load_env_file(config.environments.at(envName).file);
        };

        std::unordered_set<std::string> definedTasksAndAliases = parser.get_task_names_and_aliases();

        auto it = std::find(definedTasksAndAliases.begin(), definedTasksAndAliases.end(), taskName);

        if (it == definedTasksAndAliases.end()) {
            throw TaskrError("Task or alias '" + taskName + "' not found");
        }

        executor.execute(config, taskName);

    } catch (const FormatError &e) {
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
