def strip_minus(token):
    if token[0] == "-":
        return token[1:]
    return token

def readCNF(filename):
    with open(filename, 'r') as cnffile:
        lines = cnffile.readlines()
    without_comments = [line for line in lines if line[0] not in 'cp']
    unify = " ".join(without_comments)
    tokens = unify.split()
    clauses = []
    clause = []
    for token in tokens:
        if token != '0':
            name = 'l' + strip_minus(token)
            clause.append(name)
        else:
            clauses.append(clause)
            clause = []
    return clauses
