# satexplorer

## Install

Dependencies:

- qt5-base (libqt5 on Ubuntu)
- OGDF (clone, copy files from ogdf_installation to OGDF source and then execute the copied install script)
- boost
- python-networkx
- python-louvain
- zlib
- bison
- flex

remember to init and update git submodules

## Build

Create a build folder and `cd` into it, execute `cmake ..` and `make -j4`.

## Usage

Run `./satviewer-bin [sat-instance] [options]` in build folder.
The sat instance must be in cnf format.
If none is specified, a file dialog will open to select one.

### Options

| Flag | Description |
| --- | --- |
| -f | force recomputation of the solution instead of reusing a previous trace file |
| -s | visualize the graph simplified by minisat instead of the original graph |
| -i | display an implication, i.e., only edges for clauses with two variables |


### VSCODE launch configuration

```
{
    "name": "debug",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/satviewer-bin",
    "args": ["path/to/file.cnf"],
    "stopAtEntry": false,
    "cwd": "${workspaceFolder}/build/",
    "environment": [],
    "externalConsole": true,
    "MIMode": "gdb",
    "setupCommands": [
        {
            "description": "Enable pretty-printing for gdb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
        }
    ]
}
```
