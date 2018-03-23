#!/usr/bin/python3
'''
Checks whether a CNF formula is part of a base class as defined by Thomas J. Schaefer 
in "The Complexity of satisfiability problems"
'''

import cli

class Clause:
    def __init__(self, literals):
        self.positive_literals = []
        self.negative_literals = []
        for literal in literals:
            if literal < 0:
                self.negative_literals.append(literal)
            else:
                self.positive_literals.append(literal)
    def __len__(self):
        return len(self.positive_literals) + len(self.negative_literals)

def main():
    input_file, output_file = cli.get_file_conversion_arguments('CLIG.cnf', 'output.log')

    with open(input_file, 'r') as csv:
        clig = csv.read()
    lines = clig.splitlines()

    clauses = list()
    for line in lines:
        if(line[0] == 'c' or line[0] == 'p'):
            continue
        literals = [int(x) for x in line.split(' ') if x != '0']
        clauses.append(Clause(literals))
    
    is_horn = True
    is_anti_horn = True
    for clause in clauses:
        if len(clause.positive_literals) > 1:
            is_horn = False
        if len(clause.negative_literals) > 1:
            is_anti_horn = False

    is_2CNF = True
    for clause in clauses:
        if len(clause) > 2:
            is_2CNF = False
            break

    is_0_valid = True
    is_1_valid = True
    for clause in clauses:
        if len(clause.positive_literals) < 1:
            is_1_valid = False
        if len(clause.negative_literals) < 1:
            is_0_valid = False
            
    with open(output_file, "w") as log:
        log.write("Horn Formula: " + str(is_horn) + "\n")
        log.write("Anti Horn Formula: " + str(is_anti_horn) + "\n")
        log.write("2CNF Formula: " + str(is_2CNF) + "\n")
        log.write("0 valid Formula: " + str(is_0_valid) + "\n")
        log.write("1 valid Formula: " + str(is_1_valid) + "\n")

if __name__ == '__main__':
    main()
