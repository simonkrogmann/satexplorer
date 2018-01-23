#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <ogdfWrapper/ogdfWrapper.hpp>
#include "cfg.hpp"

int main(int ac, char* av[]) {
    try {
        graphdrawer::cfg cfg(ac, av);
        if (!cfg.config_valid()) {
            return 1;
        }

        graphdrawer::ogdfWrapper wrapper;
        wrapper.readGraph(cfg.input_filename());
        if (cfg.cull_graph()) {
            wrapper.removeNodes(cfg.cull_graph());
        }
        wrapper.layout();
        wrapper.colorNodes(graphdrawer::NodeColor::UNPROCESSED);
        wrapper.colorNode({34, graphdrawer::NodeType::LITERAL},
                          graphdrawer::NodeColor::SET_TRUE);
        wrapper.setNodeShape({34, graphdrawer::NodeType::LITERAL},
                             cfg.node_width(), cfg.node_height());
        wrapper.colorNode({18, graphdrawer::NodeType::LITERAL},
                          graphdrawer::NodeColor::STEP_SELECTED);
        wrapper.setNodeShape({18, graphdrawer::NodeType::LITERAL},
                             cfg.node_width(), cfg.node_height());
        wrapper.writeGraph(cfg.output_filename(), cfg.output_filetype());
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
