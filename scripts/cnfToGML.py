#!/bin/python
"Converts cnf files to GML files"

import sys
import os

from itertools import product
import networkx as nx

if len(sys.argv) != 3:
    print("Usage: vigToGraphml </path/to/file.csv> </path/to/output.gml>")
    exit(1)

OUTPUT_DIR = os.path.dirname(sys.argv[2])
if not os.path.isdir(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

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

nx.write_gml(g, sys.argv[2])
