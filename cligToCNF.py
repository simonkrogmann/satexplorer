import argparse
import csv

parser = argparse.ArgumentParser(description='Convert a clause literal incident graph from csv to cnf.')
parser.add_argument('inputFile', metavar='I', type=str,
                    help='name of the csv file')
parser.add_argument('outputFile', metavar='O', type=str,
                    help='name of the cnf file')


def main():

	args = parser.parse_args()

	with open(args.inputFile, 'r') as csv:
		clig = csv.read()
	lines = clig.splitlines()

	# sort by clauseIDs
	lines.sort(key = lambda x: int(x.split(" ")[0]))

	variables = set()
	clauses = []
	clauseID = None
	for line in lines:
		pair = [int(x) for x in line.split(" ")]
		if pair[0] != clauseID:
			clauses.append([]) 
			clauseID = pair[0]
		clauses[-1].append(pair[1])
		variables.add(abs(pair[1]))


	with open(args.outputFile, 'w') as cnf:
		cnf.write("c converted from {args.inputFile}\n")
		cnf.write("p cnf {} {} \n".format(len(variables), len(clauses)))
		for clause in clauses:
			cnf.write(" ".join([str(x) for x in clause]) + " 0\n")


if __name__ == '__main__':
	main()