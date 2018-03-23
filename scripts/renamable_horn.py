#!/usr/bin/python3
'''
Transforms CNF formula to the S* variant as described in "Renaming a Set of Clauses as a Horn Set", Harry R. Lewis
The CNF formula is "renamable horn" iff S* is satisfiable
'''

import cli
from satlib import Clause

def main():
    input_file, output_file = cli.get_file_conversion_arguments('CLIG.cnf', 'S*.cnf')

    with open(input_file, 'r') as csv:
        clig = csv.read()
    lines = clig.splitlines()

    variables = set()
    clauses = list()
    for line in lines:
        if(line[0] == 'c' or line[0] == 'p'):
            continue
        literals = [int(x) for x in line.split(' ') if x != '0']
        for literal in literals:
            variables.add(abs(literal))
        clauses.append(Clause(literals))
    
    # calculate S*
    S_star = list()
    for clause in clauses:
        literals = clause.positive_literals + clause.negative_literals
        for i in range(len(literals)):
            for j in range(i+1, len(literals)):
                S_star.append(Clause([literals[i], literals[j]]))            
            
    with open(output_file, "w") as cnf:
        cnf.write(f'c converted from {input_file}\n')
        cnf.write(f'p cnf {len(variables)} {len(clauses)}\n')
        for clause in S_star:
            cnf.write(clause.as_cnf_line())
        

if __name__ == '__main__':
    main()
