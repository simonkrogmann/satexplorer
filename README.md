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

Run `./satviewer-bin` in build folder.
You can specify a `.cnf` file as the first parameter to skip the file select dialouge.
`-f` forces re-solving the sat instance


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
