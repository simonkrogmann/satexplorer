#!/usr/bin/python3
"Converts cnf files to GML files"

import networkx as nx
import cli
import cnf

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.cnf', 'output.gml')

    graph = nx.Graph()
    clause_count = 0
    for clause in cnf.readCNF(input_file):
        clause_node = "c" + str(clause_count)
        clause_count += 1
        graph.add_node(clause_node)
        for literal in clause:
            graph.add_node(literal)
            graph.add_edge(clause_node, literal)


    nx.write_gml(graph, output_file)

if __name__ == '__main__':
    main()
