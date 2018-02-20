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
## How it works
The program takes files in `DIMACS` (`.cnf`) format. For detailed information refer to [http://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html](http://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html)
The input file is converted to the `GML` format via a python script. The `.gml` file can then be read by the [OGDF](http://amber-v7.cs.tu-dortmund.de/doku.php/start) library.

The `.cnf` file is also used to run the SAT solver [minisat](https://github.com/niklasso/minisat). This generates a tracefile (`.trace`), documenting the steps the solver took during the solving process. Additionally, minisat generates a `.solution` file containing a solution of the SAT Instance, if there is any.

### The tracefile
The tracefile encodes the steps taken by minisat in binary. The format is `char` followed by `int32`. This combination is called a `block`. For information how each step is encoded as a char refer to the `stepFromCharacter` map in `satviever/Stepper.cpp`.

For the Step Types that set a variable to `true` or `false` the sign of the integer corresponds to `true` for positive and `false` for negative. (e.g '+' and 1000 would set variable 1000 to `true`, while '+' and -1000 woould set it to `false`)

Clauses are handled a bit differently. The start of a clause is indicated by **'L'** or **'U'**. Its followed by the ID of the new clause (minisat makes sure it does not conflict with existing clauses). 
The next block has to have the flag 'S', indicating the start of the variable sequence. The integer denotes the amount of variables in the clause.
The following blocks represent the variables that are part of the clause. the `char` will be 'x' and the integer contains the ID of the variable.

### Clustering
Clustering is done via a python script using the [python-louvain](https://github.com/taynaud/python-louvain) implementation of the Louvain algorithm.

### Interface and Drawing
We are using Qt5 for the GUI. The graph image is rendered as an `.svg` by the OGDF Library.