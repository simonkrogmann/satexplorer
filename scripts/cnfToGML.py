#!/usr/bin/python3
"Converts cnf files to GML files"

import sys
import os

from itertools import product
import networkx as nx

if len(sys.argv) != 3:
    print("Usage: vigToGraphml </path/to/file.csv> </path/to/output.gml>")
    exit(1)

OUTPUT_DIR = os.path.dirname(sys.argv[2])
if OUTPUT_DIR and not os.path.isdir(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

g = nx.Graph()

def strip_minus(n):
    if n[0] == "-":
        return n[1:]
    return n

clause_count = 0
with open(sys.argv[1], 'r') as cnffile:
    lines = cnffile.readlines()
    without_comments = [line for line in lines if line[0] != "c" and line[0] != "p"]
    unify = " ".join(without_comments)
    tokens = unify.split()
    clauses = []
    clause = []
    for n in tokens:
        if n != '0':
            clause.append(strip_minus(n))
        else:
            clauses.append(clause)
            clause = []
    for clause in clauses:
        clause_node = "c_" + str(clause_count)
        clause_count += 1
        g.add_node(clause_node)
        for literal in clause:
            g.add_node(literal)
            g.add_edge(clause_node, literal)
        #for literal1, literal2 in product(clause, clause):
        #    if literal1 != literal2:
        #        g.add_edge(literal1, literal2)
        

nx.write_gml(g, sys.argv[2])
