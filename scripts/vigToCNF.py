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
    variable_pairs = list()
    for line in lines:
        pair = [int(x) for x in line.split(' ')]
        variables.add(pair[0])
        variables.add(pair[1])
        variable_pairs.append(pair)


    with open(output_file, 'w') as cnf:
        cnf.write(f'c converted from {input_file}\n')
        cnf.write(f'p cnf {len(variables)} {len(variable_pairs)}\n')
        for pair in variable_pairs:
            cnf.write(' '.join([str(x) for x in pair]) + ' 0\n')


if __name__ == '__main__':
    main()
