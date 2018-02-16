#!/bin/python3
"""Clusters a graph"""
import networkx as nx
import itertools
import community as lv
import sys

if len(sys.argv) < 3:
    print("Usage: louvain_clustering.py <file> <output_file>")
    exit(1)

graph = nx.read_gml(sys.argv[1])
partition = lv.best_partition(graph)

with open(sys.argv[2], "w") as file:
    for node in partition.keys():
        file.write(node + " " + str(partition[node]) + "\n")

