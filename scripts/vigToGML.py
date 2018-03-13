#!/usr/bin/python3
"Converts variable incidence graph format to gml format"

import networkx as nx
import cli

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.csv', 'output.gml')
    graph = nx.read_edgelist(input_file)
    nx.write_gml(graph, output_file)

if __name__ == '__main__':
    main()
