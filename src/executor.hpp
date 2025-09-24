#pragma once

#include "config.h"
#include "errors.hpp"
#include <algorithm>
#include <format>
#include <unordered_set>
#include <vector>

class TaskrExecutor {
  public:
    void execute(const Config &config, const std::string &taskName) {
        std::unordered_set<std::string> visited;
        execute_task(config, taskName, visited);
    }

  private:
    void execute_task(const Config &config, const std::string &taskName, std::unordered_set<std::string> &visited) {
        if (visited.count(taskName)) {
            return;
        }

        const Task *task = find_task(config, taskName);
        if (!task) {
            throw TaskrError(std::format("Task not found: {}", taskName));
        }

        visited.insert(task->name);

        for (const auto &dep : task->needs) {
            execute_task(config, dep, visited);
        }

        run(task->run);
    }

    const Task *find_task(const Config &config, const std::string &nameOrAlias) const {
        for (const auto &kv : config.tasks) {
            const Task &task = kv.second;
            if (task.name == nameOrAlias)
                return &task;

            if (std::find(task.alias.begin(), task.alias.end(), nameOrAlias) != task.alias.end()) {
                return &task;
            }
        }
        return nullptr;
    }

    void run(const std::string &command) { std::system(command.c_str()); }
};
