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

namespace {
template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
}  // namespace

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
        wrapper.setNodeShapeAll();
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

        if (cfg.layout_coordinates_enabled()) {
            std::cout << "Reading layout coordinates" << std::endl;
            auto node_coordinates = wrapper.getLayoutCoordinates();

            std::ofstream output_file(cfg.layout_coordinates_filename());
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
            std::cout << "Wrote layout coordinates to "
                      << cfg.layout_coordinates_filename() << std::endl;
        }

        if (cfg.node_clustering_enabled()) {
            std::cout << "Loading clustering";
            std::unordered_map<int, std::vector<graphdrawer::NodeID>>
                cluster_node_map;
            std::ifstream clustering_file(cfg.node_clustering_filename());
            std::string line;
            while (std::getline(clustering_file, line)) {
                auto line_split = split(line, ' ');
                assert(line_split.size() == 2);

                auto node_type = line_split[0][0];
                auto node_id_nr = std::stoi(line_split[0].substr(1));
                auto node_id =
                    graphdrawer::NodeID::fromCharType(node_type, node_id_nr);
                int cluster_id = std::stoi(line_split[1]);

                if (!cluster_node_map.count(cluster_id)) {
                    cluster_node_map.emplace(
                        cluster_id, std::vector<graphdrawer::NodeID>());
                }

                cluster_node_map[cluster_id].push_back(node_id);
            }
            std::cout << "...done" << std::endl;
            std::cout << "Applying clustering";
            uint i = 0;
            for (auto pair : cluster_node_map) {
                wrapper.colorNodes(graphdrawer::NodeColor(i), pair.second);
                ++i;
                i %=
                    (static_cast<int>(graphdrawer::NodeColor::Yellowgreen) + 1);
            }
            std::cout << "...done" << std::endl;

            wrapper.writeGraph(cfg.output_filename(), cfg.output_filetype());
        }
    } catch (std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
