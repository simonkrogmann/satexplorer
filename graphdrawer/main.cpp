
#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <string>
#include <fstream>

#include <cfg.hpp>
#include <ogdfWrapper.hpp>

int main(int ac, char* av[])
{
    try {
        
        graphdrawer::cfg cfg(ac,av);
        if(cfg.validate_config()) { // returns 1 if failure
            return 1;
        }

        graphdrawer::ogdfWrapper wrapper;
        wrapper.readGraph(cfg.input_filename());
        if(cfg.cull_graph()) {
            wrapper.removeNodes(cfg.cull_graph());
        }
        wrapper.colorNodes();
        wrapper.layout();
        wrapper.writeGraph(cfg.output_filename(), cfg.output_filetype());
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
