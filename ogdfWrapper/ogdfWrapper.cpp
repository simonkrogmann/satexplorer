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
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>


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
        { NodeColor::CONFLICT, ogdf::Color::Name::Black },
    };
    return ogdfColors[color];
}

std::unordered_map <char, NodeType> nodeTypeFromCharacter
{
    {'c', NodeType::CLAUSE},
    {'l', NodeType::LITERAL}
};

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
        ogdf::GraphAttributes::threeD |
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
        std::string label = _p_graphAttributes->label(node_p);

        // first character denotes node type
        // c = clause
        // l = literal
        // see cnfToGML.py
        uint id = std::stoi(label.substr(1));
        _label_map.emplace(std::make_pair(NodeID{id, nodeTypeFromCharacter.at(label[0])}, node_p));
    }
}

void ogdfWrapper::colorNodes(NodeColor color) {
    for(auto node_p : _m_nodes) {
        colorNode(node_p, color);
    }
}

void ogdfWrapper::setNodeShape(NodeID nodeID, double width, double height) {
    auto& node_p = _label_map.at(nodeID);
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

void ogdfWrapper::colorNode(NodeID nodeID, NodeColor color) {
    auto& node_p = _label_map.at(nodeID);
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
    std::vector<ogdf::NodeElement*> nodesToRemove;
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
                nodesToRemove.push_back(node_p);
            }

        }
    }
    if(onlyEdges) {
        for(auto edge_p : edgesToRemove) {
            _p_graph->delEdge(edge_p);
        }
        removedEntities = edgesToRemove.size();
    }
    else
    {
        for(auto node_p : nodesToRemove) {
            _p_graph->delNode(node_p);
        }
        removedEntities = nodesToRemove.size();
    }
    _updateGraph();
    std::cout << "removed " << removedEntities
        << (onlyEdges ? " Edges." : " Nodes.") << std::endl;
    return removedEntities;
}

void ogdfWrapper::layout() {
    std::cout << "generating layout" << std::endl;
    switch(_layoutType) {
        case LayoutType::FMMM:
        {
            ogdf::FMMMLayout fmmmLayout;
            _setOptions(fmmmLayout);
            fmmmLayout.call(*_p_graphAttributes);
        }
            break;
        case LayoutType::SUGIYAMA:
        {
            ogdf::SugiyamaLayout sugiyamaLayout;
            sugiyamaLayout.setRanking(new ogdf::OptimalRanking);
            sugiyamaLayout.setCrossMin(new ogdf::MedianHeuristic);
            ogdf::OptimalHierarchyLayout *ohl = new ogdf::OptimalHierarchyLayout;
            ohl->layerDistance(30.0);
            ohl->nodeDistance(25.0);
            ohl->weightBalancing(0.8);
            sugiyamaLayout.setLayout(ohl);
            sugiyamaLayout.call(*_p_graphAttributes);
        }
            break;
    }
    std::cout << "layout complete" << std::endl;
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
            // rebuild ogdf with updated CompressSVG.patch if textRendering is not found
            settings.textRendering(false);
            ogdf::GraphIO::drawSVG(*_p_graphAttributes, file, settings);
            break;
        default : throw std::invalid_argument("Unknown filetype");
    }
    std::cout << " ...done." << std::endl;
}

bool ogdfWrapper::hasNode(NodeID nodeID) {
    return _label_map.find(nodeID) != _label_map.end();
}

void ogdfWrapper::addEdge(NodeID nodeStart, NodeID nodeEnd) {
    auto& nodeS_p = _label_map.at(nodeStart);
    auto& nodeE_p = _label_map.at(nodeEnd);
    _p_graph->newEdge(nodeS_p, nodeE_p);
}

void ogdfWrapper::removeEdge(NodeID nodeStart, NodeID nodeEnd) {
    auto& nodeS_p = _label_map.at(nodeStart);
    auto& nodeE_p = _label_map.at(nodeEnd);
    for(const auto edge_p : _m_edges) {
        if((edge_p->source() == nodeS_p && edge_p->target() == nodeE_p) || (edge_p->source() == nodeE_p && edge_p->target() == nodeS_p)) {
            _p_graph->delEdge(edge_p);
            break;
        }
    }
}

void ogdfWrapper::setZ(NodeID nodeID, double z) {
    auto& node_p = _label_map.at(nodeID);
    auto& node_z = _p_graphAttributes->z(node_p);
    node_z = z;
}

void ogdfWrapper::setLayoutType(LayoutType type) {
    _layoutType = type;
}

bool ogdfWrapper::addNode(NodeID nodeID) {
    if(hasNode(nodeID)) {
        return false;
    }
    auto node_p = _p_graph->newNode();
    _p_graphAttributes->label(node_p) = nodeID.id;
    _label_map.emplace(std::make_pair(nodeID, node_p));
    setNodeShape(nodeID);
    return true;
}

bool ogdfWrapper::removeNode(NodeID nodeID) {
    if(!hasNode(nodeID)) {
        return false;
    }
    _p_graph->delNode(_label_map.at(nodeID));
    _label_map.erase(nodeID);
    return true;
}

void ogdfWrapper::moveToCenter(NodeID node, const std::vector<uint>& anchors) {
    if(!_label_map.count(node)) {
        return;
    }
    double x = 0;
    double y = 0;
    int visibleCount = 0;
    for(const auto& anchor : anchors) {
        graphdrawer::NodeID anchorNode = {anchor, graphdrawer::NodeType::LITERAL};
        if(hasNode(anchorNode)) {
            const auto anchor_p = _label_map.at(anchorNode);
            x += _p_graphAttributes->x(anchor_p);
            y += _p_graphAttributes->y(anchor_p);
            ++visibleCount;
        }
    }
    x /= visibleCount;
    y /= visibleCount;
    const auto node_p = _label_map.at(node);
    _p_graphAttributes->x(node_p) = x;
    _p_graphAttributes->y(node_p) = y;
}


}
