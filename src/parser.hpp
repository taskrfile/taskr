#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <algorithm>

#include "utils.hpp"

struct Task {
	std::string name;
	std::string command;

	std::string description;
	std::vector<std::string> dependencies;
};

class Parser {
public:
	std::vector<Task> load_tasks() {
		if (!file_exists("taskr.toml")) {
			log("No taskr.toml file found.", ERROR);
			exit(1);
		}

		std::vector<Task> tasks;

		std::vector<std::string> lines = read_file("taskr.toml");

		for (size_t i = 0; i < lines.size(); ++i) {
			std::string line = lines[i];
			if (line.empty()) continue;

			bool is_block_line = has_brackets(line);

			if (tasks.size() == 0 && !is_block_line) {
				log("Invalid taskr.toml file format: line " + std::to_string((i + 1)), ERROR);
				exit(1);
			}

			if (is_block_line) {
				if (contains_space(line)) {
					log("Invalid taskr.toml file format: line " + std::to_string((i + 1)), ERROR);
					exit(1);
				}

				if (tasks.size() > 0 && tasks.back().command.empty()) {
					log("Task " + tasks.back().name + " has no command", ERROR);
					exit(1);
				}

				Task task;
				task.name = trim_brackets(line);
				task.command = "";
				task.description = "";
				task.dependencies.clear();

				tasks.push_back(task);
			}

			if (line.find("=") != std::string::npos) {
				std::pair<std::string, std::string> kv = parse_kv_line(line, "taskr.toml", i + 1);
				if (kv.first.empty()) continue;

				Task& task = tasks.back();

				if (kv.first == "command") {
					if (task.command.empty()) {
						task.command = kv.second;
					} else {
						log("Task " + task.name + " has multiple command keys", ERROR);
						exit(1);
					}
				} else if (kv.first == "description") {
					if (task.description.empty()) {
						task.description = kv.second;
					} else {
						log("Task " + task.name + " has multiple description keys", ERROR);
						exit(1);
					}
				} else if (kv.first == "dependencies") {
					if (task.dependencies.empty()) {
						task.dependencies = parse_dependencies(kv.second, tasks);
					} else {
						log("Task " + task.name + " has multiple dependencies keys", ERROR);
						exit(1);
					}
				} else {
					log("Invalid key found in taskr.toml: line: " + std::to_string(i + 1), ERROR);
					exit(1);
				}
			}
		}

		return tasks;
	}

	void load_env() {
		if (!file_exists(".env")) return;

		std::vector<std::string> lines = read_file(".env");

		for (size_t i = 0; i < lines.size(); ++i) {
			std::string line = lines[i];
			if (line.empty()) continue;

			std::pair<std::string, std::string> kv = parse_kv_line(line, ".env", i + 1);
			if (!kv.first.empty()) {
				set_env_var(kv.first, kv.second);
			}
		}
	}

private:
	bool file_exists(std::string filename) {
		return std::filesystem::exists(filename);
	}

	std::vector<std::string> read_file(std::string filename) {
		std::vector<std::string> lines;
		std::ifstream input_file(filename);
		std:: string line;

		while (getline(input_file, line)) {
			std::string trimmed = trim(line);

			// If empty or line comment, skip line
			if (trimmed.empty() || trimmed[0] == '#') {
				lines.push_back("");
				continue;
			}

			lines.push_back(line);
		}

		return lines;
	}

	std::pair<std::string, std::string> parse_kv_line(const std::string& line, const std::string& filename, size_t line_number) {
		size_t eq_pos = line.find("=");
		if (eq_pos == std::string::npos) return {"", ""};

		std::string key = trim(line.substr(0, eq_pos));
		std::string value = trim(line.substr(eq_pos + 1));

		bool quoted = has_quotes(value);

		if (filename == "taskr.toml" && !quoted && key != "dependencies") {
			log("Missing quotes in taskr.toml: line " + std::to_string(line_number), ERROR);
			exit(1);
		}
		bool brackets = has_brackets(value);
		std::string trimmed_value = trim_quotes(value);

		trimmed_value.erase(std::remove(trimmed_value.begin(), trimmed_value.end(), '\\'), trimmed_value.end());

		if (!quoted && !brackets && contains_space(trimmed_value)) {
			log("Spaces found without using quotes in " + filename + ": line " + std::to_string(line_number), ERROR);
			exit(1);
		}

		return { key, trimmed_value };
	}

	std::vector<std::string> parse_dependencies(const std::string& line, std::vector<Task>& tasks) {
		std::vector<std::string> dependencies;
		std::string trimmed = trim_brackets(line);

		if (trimmed.empty()) return dependencies;

		std::vector<std::string> deps = split(trimmed, ',');

		for (size_t i = 0; i < deps.size(); ++i) {
			std::string dep = trim(trim_quotes(deps[i]));

			if (dep.empty()) continue;

			if (std::find(dependencies.begin(), dependencies.end(), dep) != dependencies.end()) {
				log("Duplicate dependency found: " + dep, ERROR);
				exit(1);
			}

			if (tasks.size() > 0) {
				Task& task = tasks.back();
				if (task.name == dep) {
					log("Task " + task.name + " cannot depend on itself", ERROR);
					exit(1);
				}
			}

			if (std::none_of(tasks.begin(), tasks.end(), [&dep](const Task& task) {
				return task.name == dep;
			})) {
				log("Dependency " + dep + " not found", ERROR);
				exit(1);
			}


			dependencies.push_back(dep);
		}

		return dependencies;
	}

	void set_env_var(const std::string& key, const std::string& value) {
		#ifdef _WIN32
		_putenv_s("MY_VAR", "value");
		#else
		setenv("MY_VAR", "value", 1);
		#endif
	}
};
