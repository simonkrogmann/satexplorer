#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <ogdfWrapper/ogdfWrapper.hpp>
#include "cfg.hpp"
/*
    This program demonstrates how to use the ogdfWrapper library
    It reads a graph, colors some nodes and generates a svg
*/
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
        // wrapper.colorNodes(graphdrawer::NodeColor::UNPROCESSED);
        // Node Ids are picked arbitrarily here
        // If you indend to run this, change these IDs to some that exist in
        // your input graph
        // wrapper.colorNode({34, graphdrawer::NodeType::LITERAL},
        //                   graphdrawer::NodeColor::SET_TRUE);
        // wrapper.setNodeShape({34, graphdrawer::NodeType::LITERAL},
        //                      cfg.node_width(), cfg.node_height());
        // wrapper.colorNode({18, graphdrawer::NodeType::LITERAL},
        //                   graphdrawer::NodeColor::STEP_SELECTED);
        // wrapper.setNodeShape({18, graphdrawer::NodeType::LITERAL},
        //                      cfg.node_width(), cfg.node_height());
        // wrapper.writeGraph(cfg.output_filename(), cfg.output_filetype());
        auto node_coordinates = wrapper.getLayoutCoordinates();

        std::ofstream output_file(cfg.output_filename());
        for (auto node_coordinate : node_coordinates) {
            auto prefix =
                node_coordinate.first.type == graphdrawer::NodeType::LITERAL
                    ? 'l'
                    : 'c';
            auto id = node_coordinate.first.id;
            auto x_coordinate = node_coordinate.second.first;
            auto y_coordinate = node_coordinate.second.second;
            output_file << prefix << id << " " << x_coordinate << " "
                        << y_coordinate << std::endl;
        }
        output_file.flush();
        output_file.close();
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
