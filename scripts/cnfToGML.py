#!/usr/bin/python3
"Converts cnf files to GML files"

import sys
import networkx as nx
import cli
import cnf

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.cnf', 'output.gml')
    only_implications = (len(sys.argv) > 3 and sys.argv[3] == '-implications')

    graph = nx.Graph()
    clause_count = 0
    for clause in cnf.readCNF(input_file):
        for literal in clause:
            graph.add_node(literal)
        if only_implications and len(clause) == 2:
            graph.add_edge(clause[0], clause[1])
        elif not only_implications:
            clause_name = "c" + str(clause_count)
            clause_count += 1
            graph.add_node(clause_name)
            for literal in clause:
                graph.add_edge(clause_name, literal)



    nx.write_gml(graph, output_file)

if __name__ == '__main__':
    main()
