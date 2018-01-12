import sys
from subprocess import check_output, CalledProcessError, TimeoutExpired

with open(sys.argv[1], 'r') as f:
    content = f.read()

target = open(sys.argv[1] + ".important", 'w')

lines = content.splitlines()

counter = 0

for i, line in enumerate(lines):
    if line[0] not in "1234567890-":
        target.write(line + "\n")
        continue

    new_problem = "\n".join(lines[:i] + lines[i+1:])
    with open("build/data/new_problem.cnf", 'w') as f:
        f.write(new_problem)

    try:
        output = check_output(["build/minisat-solver", "build/data/new_problem.cnf",
        "build/data/new_problem.solution", "build/data/new_problem.trace"], timeout=2)
    except CalledProcessError:
        pass
    except TimeoutExpired:
        pass
    with open("build/data/new_problem.solution", 'r') as f:
        content = f.read()
    if content.startswith("SAT"):
        target.write(line + "\n")
        counter += 1
    print(i, "/", len(lines), f", {counter} kept")

target.close()
