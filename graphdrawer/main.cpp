#include <ogdf/basic/graph_generators.h>
#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <string>

int main(int ac, char* av[])
{
    try {
        
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("file", po::value<std::string>(), "input file in graphml format")
        ;

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("file")) {
            std::cout << "Input file is "
                    << vm["file"].as<std::string>() << ".\n";
        } else {
            std::cout << "No input file specified" << std::endl;
            std::cout << desc << std::endl;
        }
        std::string filename = vm["file"].as<std::string>();

        ogdf::Graph graph;
        ogdf::GraphIO::read(graph, filename, ogdf::GraphIO::readGraphML);

        std::cout << "Nodes: " << graph.numberOfNodes()
            << " Edges: " << graph.numberOfEdges() << std::endl;
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
