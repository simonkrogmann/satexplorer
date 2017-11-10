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
        ogdf::GraphAttributes::nodeStyle
    );
    ogdf::GraphIO::read(*_p_graphAttributes, *_p_graph, filename, ogdf::GraphIO::readGML);

    std::cout << "Nodes: " << _p_graph->numberOfNodes()
        << " Edges: " << _p_graph->numberOfEdges() << std::endl;

    _p_graph->allNodes(_m_nodes);
    // what the f ?
    // std::cout << _p_graph->firstNode()->graphOf() << " " <<  _p_graph.get();
    // for(auto node_p : _m_nodes) {
    //     std::cout << (node_p->graphOf() == _p_graph.get()) << std::endl;
    //     auto & color = _p_graphAttributes->fillColor(node_p);
    //     //color = ogdf::Color(ogdf::Color::Name::Gold);
    // }
}

void ogdfWrapper::colorNodes() {
    for(auto node_p : _m_nodes) {
        std::cout << (node_p->graphOf() == _p_graph.get()) << std::endl;
        auto & color = _p_graphAttributes->fillColor(node_p);
        //color = ogdf::Color(ogdf::Color::Name::Gold);
    }
}

int ogdfWrapper::removeNodes(int maxDegree) {
    //ogdfVector<ogdf::NodeElement*> nodesTooLarge;

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