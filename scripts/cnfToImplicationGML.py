#!/usr/bin/python3
"Converts cnf files to GML files, but only keeps clauses of length 2"

import networkx as nx
import cli
import cnf

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.cnf', 'implication.gml')

    graph = nx.Graph()
    for clause in cnf.readCNF(input_file):
        for literal in clause:
            graph.add_node(literal)
        if len(clause) != 2:
            continue
        graph.add_edge(clause[0], clause[1])


    nx.write_gml(graph, output_file)

if __name__ == '__main__':
    main()
