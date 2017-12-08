#include "ogdfWrapper.hpp"

#include <iostream>
#include <string>

#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/graphics.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/Array.h>


namespace graphdrawer {

namespace {

ogdf::Color::Name getColor(NodeColor color) {
    std::unordered_map<NodeColor, ogdf::Color::Name> ogdfColors {
        { NodeColor::UNPROCESSED, ogdf::Color::Name::White },
        { NodeColor::SET_TRUE, ogdf::Color::Name::Green },
        { NodeColor::SET_FALSE, ogdf::Color::Name::Red },
        { NodeColor::STEP_SELECTED, ogdf::Color::Name::Blue },
        { NodeColor::BRANCH_TRUE, ogdf::Color::Name::Blue },
        { NodeColor::BRANCH_FALSE, ogdf::Color::Name::Orange },
    };
    return ogdfColors[color];
}

}

ogdfWrapper::ogdfWrapper()
{

}

ogdfWrapper::~ogdfWrapper()
{

}

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

void ogdfWrapper::colorNodes(NodeColor color) {
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

void ogdfWrapper::setStrokeWidth(float width)
{
    for(auto node_p : _m_nodes)  {
        auto& strokeWidth = _p_graphAttributes->strokeWidth(node_p);
        strokeWidth = width;
    }
    for(auto edge_p : _m_edges)  {
        auto& strokeWidth = _p_graphAttributes->strokeWidth(edge_p);
        strokeWidth = width;
    }
}

void ogdfWrapper::colorNode(ogdf::NodeElement* node_p, NodeColor color) {
    auto colorName = getColor(color);
    auto & stroke_color = _p_graphAttributes->strokeColor(node_p);
    stroke_color = ogdf::Color(ogdf::Color::Name::Black);
    auto & fill_color = _p_graphAttributes->fillColor(node_p);
    fill_color = ogdf::Color(colorName);
}

void ogdfWrapper::colorNode(int nodeID, NodeColor color) {
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

int ogdfWrapper::removeNodes(int maxDegree, bool onlyEdges) {
    int removedEntities = 0;
    std::set<ogdf::EdgeElement*> edgesToRemove;
    for(auto node_p : _m_nodes) {
        if(node_p->degree() > maxDegree) {
            if(onlyEdges) {
                for(const auto edge_p : _m_edges) {
                    if(edge_p->source() == node_p || edge_p->target() == node_p) {
                        edgesToRemove.insert(edge_p);
                    }
                }
            }
            else {
                _p_graph->delNode(node_p);
                ++removedEntities;
            }
            
        }
    }
    if(onlyEdges) {
        for(auto edge_p : edgesToRemove) {
            _p_graph->delEdge(edge_p);
        }
    }
    _updateGraph();
    if(onlyEdges) {
        std::cout << "removed " << removedEntities << " Edges." << std::endl;
    }
    else {
        std::cout << "removed " << removedEntities << " Nodes." << std::endl;
    }
    return removedEntities;
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
    layout.useHighLevelOptions(true);
    layout.unitEdgeLength(30.0);
    layout.newInitialPlacement(true);
    layout.qualityVersusSpeed(ogdf::FMMMOptions::QualityVsSpeed::GorgeousAndEfficient);
    //layout.repForcesStrength(layout.repForcesStrength() * 2);
    //layout.springStrength(layout.springStrength() * 2);
}

void ogdfWrapper::writeGraph(std::string filename, filetype format) {
    std::cout << "writing " << filename;
    std::fstream file(filename, file.out);
    ogdf::GraphIO::SVGSettings settings;
    switch(format) {
        case filetype::GML : ogdf::GraphIO::writeGML(*_p_graphAttributes, file); break;
        case filetype::SVG :
            //settings.fontSize(2);
            ogdf::GraphIO::drawSVG(*_p_graphAttributes, file, settings);
            break;
        default : throw std::invalid_argument("Unknown filetype");
    }
    std::cout << " ...done." << std::endl;
}

bool ogdfWrapper::hasNode(int nodeID) {
    return _label_map.count(std::to_string(nodeID)) > 0;
}

}
