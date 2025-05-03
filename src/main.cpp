#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#include "utils.hpp"
#include "parser.hpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::system;
using std::find;
using std::setw;
using std::left;
using std::max;

void show_help() {
	cout << "Usage:\n"
	     << "  taskr <task_name>\n\n"
	     << "Options:\n"
  	     << "  -h, --help      Show this help message and exit\n"
  	     << "  -l, --list      List the available tasks\n";
}

void show_tasks(const vector<Task>& tasks) {
	size_t name_width = 0;
	for (const Task& task : tasks) {
		name_width = max(name_width, task.name.length());
	}

	cout << "List of available tasks:\n";
	cout << left;

	for (const Task& task : tasks) {
		cout << "  - " << setw(name_width) << task.name;
		if (!task.description.empty()) {
			cout << " --> " << task.description;
		}
		cout << endl;
	}
}

vector<string> resolve_dependencies(const string &task_name, const vector<Task>& tasks) {
	vector<string> resolved_tasks;
	vector<string> visited;
	vector<string> stack;
	stack.push_back(task_name);
	while (!stack.empty()) {
		string current_task = stack.back();
		stack.pop_back();

		if (find(visited.begin(), visited.end(), current_task) != visited.end()) {
			continue;
		}

		visited.push_back(current_task);

		for (const Task &task : tasks) {
			if (task.name == current_task) {
				for (const string &dependency : task.dependencies) {
					stack.push_back(dependency);
				}
				break;
			}
		}

		resolved_tasks.push_back(current_task);
	}
	reverse(resolved_tasks.begin(), resolved_tasks.end());
	return resolved_tasks;
}

void execute_task(const string &task_name, const vector<Task>& tasks) {
	vector<string> resolved_tasks = resolve_dependencies(task_name, tasks);
	for (const string &resolved_task : resolved_tasks) {
		for (const Task &task : tasks) {
			if (task.name == resolved_task) {
				system(task.command.c_str());
			}
		}
	}
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		log("Incorrect amount of arguments", ERROR);
		exit(1);
	}

	string arg = argv[1];

	Parser parser = Parser();
	vector<Task> tasks = parser.load_tasks();
	bool isTask = 0;

	if (tasks.size() == 0 && (arg != "-h" && arg != "--help")) {
		log("No tasks found in taskr.toml", ERROR);
		exit(1);
	}

	for (const Task &task : tasks) {
		if (task.name == arg) {
			isTask = 1;
			break;
		}
	}

	if (!isTask && (arg == "-h" || arg == "--help")) {
		show_help();
		exit(0);
	} else if (!isTask && (arg == "-l" || arg == "--list")) {
		show_tasks(tasks);
		exit(0);
	} else if (isTask) {
		parser.load_env();
		execute_task(arg, tasks);
	} else {
		cout << "This argument is not supported.\n\n";
		show_help();
		exit(1);
	}
}
