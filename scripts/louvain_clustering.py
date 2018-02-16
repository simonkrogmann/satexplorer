#!/bin/python3
"""Clusters a graph"""
import networkx as nx
import community as lv
import cli

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.gml', 'clustering_file')

    graph = nx.read_gml(input_file)
    partition = lv.best_partition(graph)

    with open(output_file, "w") as file:
        for node in partition.keys():
            file.write(node + " " + str(partition[node]) + "\n")

if __name__ == '__main__':
    main()
