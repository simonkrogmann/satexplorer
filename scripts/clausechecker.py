#!/bin/python3
"""
Find out clauses, which are essential for the unsatisfiability of an unsatisfiable SAT instance,
by removing them one by one and checking if unsatisfiability can be proven before timeout
"""

import sys
from subprocess import check_output, CalledProcessError, TimeoutExpired

TIMEOUT = 2
BUILD_FOLDER = "../build-release"

def main():
    with open(sys.argv[1], 'r') as input_file:
        content = input_file.read()
        lines = content.splitlines()

    prefix = BUILD_FOLDER + "/data/new_problem"
    cnf_name = prefix + ".cnf"
    solution_name = prefix + ".solution"
    solver_command = [f"{BUILD_FOLDER}/minisat-solver", cnf_name, prefix]

    with open(sys.argv[1] + ".important", 'w') as target:
        counter = 0
        for i, line in enumerate(lines):
            if line[0] not in "1234567890-":
                target.write(line + "\n")
                continue

            new_problem = "\n".join(lines[:i] + lines[i+1:])
            with open(cnf_name, 'w') as new_problem_cnf:
                new_problem_cnf.write(new_problem)

            try:
                check_output(solver_command, timeout=TIMEOUT)
            except CalledProcessError:
                pass
            except TimeoutExpired:
                pass
            with open(solution_name, 'r') as solution_file:
                content = solution_file.read()
            if content.startswith("SAT"):
                target.write(line + "\n")
                counter += 1
            print(i, "/", len(lines), f", {counter} kept")

if __name__ == '__main__':
    main()
