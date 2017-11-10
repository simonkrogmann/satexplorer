#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdfWrapper.hpp>
#include <iostream>

namespace graphdrawer {

void ogdfWrapper::readGraph(std::string filename) {
    std::cout << "reading graph" << std::endl;
    
    _p_graph = std::make_unique<ogdf::Graph>();
    ogdf::GraphIO::read(*_p_graph, filename, ogdf::GraphIO::readGraphML);

    std::cout << "Nodes: " << _p_graph->numberOfNodes()
        << " Edges: " << _p_graph->numberOfEdges() << std::endl;

    _p_graphAttributes = std::make_unique<ogdf::GraphAttributes>(*_p_graph);
}

void ogdfWrapper::layout() {
    ogdf::FMMMLayout layout;
    set_options(layout);

    std::cout << "generating layout" << std::endl;
    layout.call(*_p_graphAttributes);
    std::cout << "layout complete" << std::endl;
    std::cout << "layouting took " << layout.getCpuTime() << " seconds" << std::endl;
}

void ogdfWrapper::set_options(ogdf::FMMMLayout& layout) {
    layout.newInitialPlacement(true);
    layout.useHighLevelOptions(true);
    layout.unitEdgeLength(20.0);
}

void ogdfWrapper::writeGraph(std::string filename, filetype format) {
    std::cout << "writing " << filename;
    std::fstream file(filename, file.out);
    switch(format) {
        case filetype::GML : ogdf::GraphIO::writeGML(*_p_graphAttributes, file); break;
        case filetype::SVG : ogdf::GraphIO::drawSVG(*_p_graphAttributes, file); break;
        default : throw std::invalid_argument("Unknown filetype");
    }
    std::cout << " ...done." << std::endl;
}

}