#!/bin/python
"Converts cnf files to GML files"

import sys
import os
import math

import networkx as nx

if len(sys.argv) != 2:
    print("Usage: vigToGraphml </path/to/file.csv>")
    exit(1)

INPUT_FILENAME = os.path.basename(sys.argv[1])
INPUT_FILEBASENAME = os.path.splitext(INPUT_FILENAME)[0]
INPUT_DIR = os.path.dirname(sys.argv[1])
g = nx.Graph()

def stripMinus(x):
    if x[0] == "-":
        return x[1:]
    return x

with open(sys.argv[1], 'r') as cnffile:
    for line in cnffile:
        if line[0] == "c" or line[0] == "p":
            continue
        # skip last entry - it is always 0
        nodes = line.strip().split(' ')[0:-1]
        print(nodes)
        for node1 in nodes:
            n1 = stripMinus(node1)
            g.add_node(n1, label=n1)
            for node2 in nodes:
                n2 = stripMinus(node1)
                if n1 != n2:
                    g.add_edge(n1, n2)

nx.write_gml(g, INPUT_DIR + "/" + INPUT_FILEBASENAME + ".gml")
