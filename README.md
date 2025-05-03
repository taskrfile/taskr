# Taskr

Taskr is a simple command-line task runner written in C++. It allows you to define tasks in a `taskr.toml` file and execute them with ease. It also supports loading environment variables from a `.env` file.

Inspired by [just](https://github.com/casey/just).

## Features

- Define tasks in a TOML file with commands and dependencies.
- Load environment variables from a `.env` file.
- List available tasks or show help for usage.

### Disclaimer
- Inline comments in `taskr.toml` and `.env` files are not yet supported.

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.15 or higher
- Make (or any build system supported by CMake)

### Building the Project

1. Clone the repository:
  ```bash
  git clone <repository-url>
  ```
2. Create a build directory and run CMake:
  ```bash
  cd cli-cpp
  mkdir build
  cd build
  cmake ..
  ```
3. Build the project:
  ```bash
  make
  ```
4. The compiled binary will be located in the `bin` directory.

## Usage
1. Define tasks in `taskr.toml`:
```toml
[echo]
command = "echo \"Hello World!!\""

[build]
command = "make -C build"
description = "build the project"

[install]
command = "sudo ln -f bin/taskr /usr/bin/taskr"
dependencies = ["echo", "build"]
description = "link the taskr binary to /usr/bin/taskr"
```

2. Run a task:
```bash
./bin/taskr run <task-name>
```
3. List available tasks:
```bash
./bin/taskr -l
```
4. Show help:
```bash
./bin/taskr -h
```
