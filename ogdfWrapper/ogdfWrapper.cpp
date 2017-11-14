#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdfWrapper.hpp>
#include <ogdf/basic/graphics.h>
#include <iostream>


namespace graphdrawer {

void ogdfWrapper::readGraph(std::string filename) {
    std::cout << "reading graph" << std::endl;
    
    _p_graph = std::make_unique<ogdf::Graph>();
    _p_graphAttributes = std::make_unique<ogdf::GraphAttributes>(*_p_graph, 
        ogdf::GraphAttributes::nodeLabel | 
        ogdf::GraphAttributes::nodeGraphics | 
        ogdf::GraphAttributes::edgeGraphics |
        ogdf::GraphAttributes::nodeStyle |
        ogdf::GraphAttributes::edgeStyle
    );
    ogdf::GraphIO::read(*_p_graphAttributes, *_p_graph, filename, ogdf::GraphIO::readGML);

    std::cout << "Nodes: " << _p_graph->numberOfNodes()
        << " Edges: " << _p_graph->numberOfEdges() << std::endl;

    _p_graph->allNodes(_m_nodes);
    _p_graph->allEdges(_m_edges);
}

void ogdfWrapper::colorNodes() {
    for(auto node_p : _m_nodes) {
        auto & stroke_color = _p_graphAttributes->strokeColor(node_p);
        stroke_color = ogdf::Color(ogdf::Color::Name::Black);
        auto & fill_color = _p_graphAttributes->fillColor(node_p);
        fill_color = ogdf::Color(ogdf::Color::Name::Green);
        auto& width = _p_graphAttributes->width(node_p);
        width = 50.0;
        auto& height = _p_graphAttributes->height(node_p);
        height = 50.0;
    }
}

void ogdfWrapper::colorEdges() {
    for(auto node_p : _m_edges) {
        auto & stroke_color = _p_graphAttributes->strokeColor(node_p);
        stroke_color = ogdf::Color(ogdf::Color::Name::Yellow);
        auto & stroke_width = _p_graphAttributes->strokeWidth(node_p);
        stroke_width = 10.f;
    }
}

int ogdfWrapper::removeNodes(int maxDegree) {
    int removedNodes = 0;
    for(auto node_p : _m_nodes) {
        if(node_p->degree() > maxDegree) {
            _p_graph->delNode(node_p);
            ++removedNodes;
        }
    }
    _p_graph->allNodes(_m_nodes);
    std::cout << "removed " << removedNodes << " Nodes." << std::endl;
    return removedNodes;
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