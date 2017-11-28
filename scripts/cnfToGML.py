#!/bin/python
"Converts cnf files to GML files"

import sys
import os

from itertools import product
import networkx as nx

if len(sys.argv) != 2:
    print("Usage: vigToGraphml </path/to/file.csv>")
    exit(1)

INPUT_FILENAME = os.path.basename(sys.argv[1])
INPUT_FILEBASENAME = os.path.splitext(INPUT_FILENAME)[0]
INPUT_DIR = os.path.dirname(sys.argv[1])
g = nx.Graph()

def strip_minus(n):
    if n[0] == "-":
        return n[1:]
    return n

with open(sys.argv[1], 'r') as cnffile:
    for line in cnffile:
        if line[0] == "c" or line[0] == "p":
            continue
        # skip last entry - it is always 0
        nodes = line.strip().split(' ')[0:-1]
        nodes = list(map(strip_minus, nodes))
        for node in nodes:
            g.add_node(node)
        for node1, node2 in product(nodes, nodes):
            if node1 != node2:
                g.add_edge(node1, node2)
print(g.nodes())
print(g.edges())

nx.write_gml(g, INPUT_DIR + "/" + INPUT_FILEBASENAME + ".gml")
