'''
Helper classes for handling SAT Formulas
'''

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

    def as_cnf_line(self):
        positive_literal_string = " ".join([str(x) for x in self.positive_literals])
        negative_literal_string = " ".join([str(x) for x in self.negative_literals])
        seperator = " " if len(self.positive_literals) > 0 and len(self.negative_literals) > 0 else ""
        return positive_literal_string + seperator + negative_literal_string + " 0\n"