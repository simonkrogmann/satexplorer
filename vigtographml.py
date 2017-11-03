#!/usr/bin/python
"Converts variable incidence graph format to graphml format"

import sys
import os
import networkx as nx

if len(sys.argv) != 2:
    print("Usage: vigToGraphml </path/to/file.csv>")

INPUT_FILENAME = os.path.basename(sys.argv[1])
INPUT_FILEBASENAME = os.path.splitext(INPUT_FILENAME)[0]
INPUT_DIR = os.path.dirname(sys.argv[1])
G = nx.read_edgelist(sys.argv[1])
nx.write_graphml(G, INPUT_DIR + "/" + INPUT_FILEBASENAME + ".graphml")
