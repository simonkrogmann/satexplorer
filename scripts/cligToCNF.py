#!/usr/bin/python3

import cli

def main():
    input_file, output_file = cli.get_file_conversion_arguments('input.csv', 'output.cnf')

    with open(input_file, 'r') as csv:
        clig = csv.read()
    lines = clig.splitlines()

    # sort by clauseIDs
    lines.sort(key=lambda x: int(x.split(' ')[0]))

    variables = set()
    clauses = []
    clauseID = None
    for line in lines:
        pair = [int(x) for x in line.split(' ')]
        if pair[0] != clauseID:
            clauses.append([])
            clauseID = pair[0]
        clauses[-1].append(pair[1])
        variables.add(abs(pair[1]))


    with open(output_file, 'w') as cnf:
        cnf.write(f'c converted from {input_file}\n')
        cnf.write(f'p cnf {len(variables)} {len(clauses)}\n')
        for clause in clauses:
            cnf.write(' '.join([str(x) for x in clause]) + ' 0\n')


if __name__ == '__main__':
    main()
