#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct Task {
    std::string name;
    std::string run;
    std::string desc;
    std::vector<std::string> alias;
    std::vector<std::string> needs;
};

struct Environment {
    std::string name;
    std::string file;
    bool isDefault = false;
};

struct Config {
    bool hasDefaultEnv = false;
    std::unordered_map<std::string, Environment> environments;
    std::unordered_map<std::string, Task> tasks;
};
