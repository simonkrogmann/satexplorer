#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <string>
#include <fstream>

#include "cfg.hpp"
#include <ogdfWrapper/ogdfWrapper.hpp>

int main(int ac, char* av[])
{
    try {
        
        graphdrawer::cfg cfg(ac,av);
        if(!cfg.config_valid()) {
            return 1;
        }

        graphdrawer::ogdfWrapper wrapper;
        wrapper.readGraph(cfg.input_filename());
        if(cfg.cull_graph()) {
            wrapper.removeNodes(cfg.cull_graph());
        }
        wrapper.layout();
        wrapper.colorNodes(graphdrawer::ogdfWrapper::nodeColor::UNPROCESSED);
        wrapper.colorNode(34, graphdrawer::ogdfWrapper::nodeColor::PROCESSED);
        wrapper.setNodeShape(34, cfg.node_width(), cfg.node_height());
        wrapper.colorNode(18, graphdrawer::ogdfWrapper::nodeColor::STEP_SELECTED);
        wrapper.setNodeShape(18, cfg.node_width(), cfg.node_height());
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
