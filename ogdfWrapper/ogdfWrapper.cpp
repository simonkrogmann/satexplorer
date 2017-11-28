#include "ogdfWrapper.hpp"

#include <iostream>
#include <string>

#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graphics.h>

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

    _updateGraph();
}

void ogdfWrapper::_updateGraph() {
    _p_graph->allNodes(_m_nodes);
    _p_graph->allEdges(_m_edges);

    _label_map.clear();
    for(auto node_p : _m_nodes) {
        std::string& label = _p_graphAttributes->label(node_p);
        _label_map.emplace(std::make_pair(label, node_p));
    }
}

void ogdfWrapper::colorNodes(nodeColor color) {
    for(auto node_p : _m_nodes) {
        colorNode(node_p, color);
    }
}

void ogdfWrapper::setNodeShape(int nodeID, double width, double height) {
    auto& node_p = _label_map.at(std::to_string(nodeID));
    _p_graphAttributes->width(node_p) = width;
    _p_graphAttributes->height(node_p) = height;
    _p_graphAttributes->shape(node_p) = ogdf::Shape::Ellipse;
}

void ogdfWrapper::setNodeShapeAll(double width, double height) {
    _p_graphAttributes->setAllWidth(width);
    _p_graphAttributes->setAllHeight(height);
    for(auto node_p : _m_nodes)  {
        auto& shape = _p_graphAttributes->shape(node_p);
        shape = ogdf::Shape::Ellipse;
    }
}

void ogdfWrapper::colorNode(ogdf::NodeElement* node_p, nodeColor color) {
    auto colorName = getColor(color);
    auto & stroke_color = _p_graphAttributes->strokeColor(node_p);
    stroke_color = ogdf::Color(ogdf::Color::Name::Black);
    auto & fill_color = _p_graphAttributes->fillColor(node_p);
    fill_color = ogdf::Color(colorName);
}

void ogdfWrapper::colorNode(int nodeID, nodeColor color) {
    auto& node_p = _label_map.at(std::to_string(nodeID));
    colorNode(node_p, color);
}

void ogdfWrapper::colorEdges() {
    for(auto node_p : _m_edges) {
        auto & stroke_color = _p_graphAttributes->strokeColor(node_p);
        stroke_color = ogdf::Color(ogdf::Color::Name::Black);
        auto & stroke_width = _p_graphAttributes->strokeWidth(node_p);
        stroke_width = 1.f;
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
    _updateGraph();
    std::cout << "removed " << removedNodes << " Nodes." << std::endl;
    return removedNodes;
}

void ogdfWrapper::layout() {
    ogdf::FMMMLayout layout;
    _setOptions(layout);

    std::cout << "generating layout" << std::endl;
    layout.call(*_p_graphAttributes);
    std::cout << "layout complete" << std::endl;
    std::cout << "layouting took " << layout.getCpuTime() << " seconds" << std::endl;
}

void ogdfWrapper::_setOptions(ogdf::FMMMLayout& layout) {
    layout.newInitialPlacement(true);
    layout.useHighLevelOptions(true);
    layout.unitEdgeLength(20.0);
    layout.repForcesStrength(layout.repForcesStrength() * 2);
    //layout.springStrength(layout.springStrength() * 2);
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

ogdf::Color::Name ogdfWrapper::getColor(nodeColor color) {
    switch(color) {
        case nodeColor::UNPROCESSED: return ogdf::Color::Name::White;
        case nodeColor::PROCESSED: return ogdf::Color::Name::Blue;
        case nodeColor::STEP_SELECTED: return ogdf::Color::Name::Orange;
    }
}

}
