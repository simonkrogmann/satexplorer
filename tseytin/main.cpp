#include <iostream>
#include <memory>

#include <bcsat/bc.hh>
#include <bcsat/defs.hh>

int main(const int argc, const char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: tseytintransform <infile> <outfile>" << std::endl;
        exit(1);
    }
    std::unique_ptr<BC> circuit(BC::parse_circuit(argv[1]));
    std::cout << circuit->count_gates() << std::endl;
}
