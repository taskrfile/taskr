# Taskr

`taskr` is a simple command-line task runner written in C++.
It allows you to define tasks in a `taskrfile` and execute them with ease.
It also supports loading environment variables from defined environment files.

Inspired by [just](https://github.com/casey/just).

## Installing
```
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
$ cmake --build build -j
$ sudo cmake --install build
```
> [!CAUTION]
> Windows is not (yet) supported. If on Windows, use WSL.

## CLI
```
$ taskr -h
Usage:
  taskr <task_name> [options]

Options:
  -h, --help         Show this help message and exit
  -l, --list         List the available tasks
  -e, --environment  Select the environment you want to use
```

`taskr` will look for a `taskrfile` file in the current directory. The filename is checked case-insensitive, this means that `TaskrFile` is also a valid name.

> [!TIP]
> Set `alias t=taskr` in your shell to use fewer keystrokes!

## Configuration
### Formatting
- Comments using `//`
- Top-level blocks are defined with `env <name>:` or `task <name>:`
- Inside blocks:
	- `key = value`: value is processed as a string
	- Lists are comma-separated
- Indentation: 2 spaces required for a key underneath a top-level block
- No quotes needed, unless absolutely necessary

### Top Level Blocks
#### ENV
Header:
```taskrfile
env <name>:
```
Keys:
- `file`: which file's env variables need to be loaded for this specific environment.

#### DEFAULT ENV
The default environment is loaded when you do not explicitly provide an environment using `-e`. Using `-e` overrides this.
Header:
```taskrfile
default env <name>:
```
Keys:
- `file`: which file's env variables need to be loaded for this specific environment.

#### TASK
Header:
```taskrfile
task <name>:
```
Keys:
- `run`: The command that the task will execute. This is the only **required** key.
- `desc`: The description of the task.
- `needs`: The dependencies of the task, dependencies will run in the order you defined.
- `alias`: list of aliases that can be used to run the task.

### Example Configuration
```taskrfile
// default environment, will get loaded even without -e flag
default env dev:
  file = dev.env

env prod:
  file = prod.env

task echo:
  run = echo "Hello World!!" // quotes are treated as escaped quotes

task build:
  run   = make -C build
  desc  = build the project
  alias = b, bld

task install:
  run   = sudo ln -f bin/taskr /usr/bin/taskr
  needs = echo, build
  desc  = link the taskr binary to /usr/bin/taskr
  alias = i
```

# Sidenote
While I do my best to keep this functional, stable, and useful, **things can (and probably will) break from time to time**.

Please use it with caution. Contributions, bug reports, and ideas are always welcome.
