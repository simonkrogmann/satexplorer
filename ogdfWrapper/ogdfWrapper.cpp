#include "ogdfWrapper.hpp"

#include <iostream>
#include <string>
#include <unordered_set>

#ifndef NDEBUG
#define NDEBUG
#define RESTORE_NDEBUG
#endif
#include <ogdf/basic/Array.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/graphics.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#ifdef RESTORE_NDEBUG
#undef NDEBUG
#endif

namespace graphdrawer {
namespace {
ogdf::Color::Name getColor(NodeColor color) {
    std::unordered_map<NodeColor, ogdf::Color::Name> ogdfColors{
        {NodeColor::UNPROCESSED, ogdf::Color::Name::White},
        {NodeColor::SET_TRUE, ogdf::Color::Name::Green},
        {NodeColor::SET_FALSE, ogdf::Color::Name::Red},
        {NodeColor::STEP_SELECTED, ogdf::Color::Name::Blue},
        {NodeColor::BRANCH_TRUE, ogdf::Color::Name::Darkblue},
        {NodeColor::BRANCH_FALSE, ogdf::Color::Name::Orange},
        {NodeColor::CONFLICT, ogdf::Color::Name::Black},
        {NodeColor::Brown, ogdf::Color::Name::Brown},
        {NodeColor::Cyan, ogdf::Color::Name::Cyan},
        {NodeColor::Gray, ogdf::Color::Name::Gray},
        {NodeColor::Yellow, ogdf::Color::Name::Yellow},
        {NodeColor::Purple, ogdf::Color::Name::Purple},
        {NodeColor::Pink, ogdf::Color::Name::Pink},
        {NodeColor::Olive, ogdf::Color::Name::Olive},
        {NodeColor::Yellowgreen, ogdf::Color::Name::Yellowgreen}};
    return ogdfColors[color];
}

const std::unordered_map<char, ogdf::Shape> nodeShapeFromCharacter{
    {'c', ogdf::Shape::Rect}, {'l', ogdf::Shape::Ellipse}};
}  // namespace

ogdfWrapper::ogdfWrapper() {}

ogdfWrapper::~ogdfWrapper() {}

void ogdfWrapper::readGraph(const std::string& filename) {
    std::cout << "reading graph" << std::endl;

    _p_graph = std::make_unique<ogdf::Graph>();
    _p_graphAttributes = std::make_unique<ogdf::GraphAttributes>(
        *_p_graph, ogdf::GraphAttributes::nodeLabel |
                       ogdf::GraphAttributes::threeD |
                       ogdf::GraphAttributes::nodeGraphics |
                       ogdf::GraphAttributes::edgeGraphics |
                       ogdf::GraphAttributes::nodeStyle |
                       ogdf::GraphAttributes::edgeStyle);
    ogdf::GraphIO::read(*_p_graphAttributes, *_p_graph, filename,
                        ogdf::GraphIO::readGML);

    std::cout << "Nodes: " << _p_graph->numberOfNodes()
              << " Edges: " << _p_graph->numberOfEdges() << std::endl;

    _updateGraph();
}

ogdf::Shape ogdfWrapper::_nodeShapeforPointer(ogdf::NodeElement* node_p) {
    const auto& labelChar = _p_graphAttributes->label(node_p)[0];
    return nodeShapeFromCharacter.at(labelChar);
}

NodeID ogdfWrapper::_nodeIDforPointer(ogdf::NodeElement* node_p) {
    const auto& label = _p_graphAttributes->label(node_p);
    // first character denotes node type
    // c = clause
    // l = literal
    // see cnfToGML.py
    const uint id = std::stoi(label.substr(1));
    return NodeID::fromCharType(label.at(0), id);
}

void ogdfWrapper::_updateGraph() {
    _p_graph->allNodes(_m_nodes);
    _p_graph->allEdges(_m_edges);

    _label_map.clear();
    for (auto node_p : _m_nodes) {
        _label_map[_nodeIDforPointer(node_p)] = node_p;
    }
}

void ogdfWrapper::colorAllNodes(NodeColor color) {
    for (auto node_p : _m_nodes) {
        colorNode(node_p, color);
    }
}

void ogdfWrapper::colorNodes(NodeColor color,
                             const std::vector<NodeID>& nodes) {
    for (auto node : nodes) {
        if (hasNode(node)) {
            colorNode(_label_map.at(node), color);
        }
    }
}

void ogdfWrapper::setNodeShape(NodeID nodeID, double width, double height) {
    auto& node_p = _label_map.at(nodeID);
    _p_graphAttributes->width(node_p) = width;
    _p_graphAttributes->height(node_p) = height;
}

void ogdfWrapper::resetNodeShapeAll(double width, double height) {
    _p_graphAttributes->setAllWidth(width);
    _p_graphAttributes->setAllHeight(height);
    for (auto node_p : _m_nodes) {
        auto& shape = _p_graphAttributes->shape(node_p);
        shape = _nodeShapeforPointer(node_p);
    }
}

void ogdfWrapper::setEdgeWidth(float width) {
    for (auto node_p : _m_nodes) {
        auto& strokeWidth = _p_graphAttributes->strokeWidth(node_p);
        strokeWidth = width;
    }
    for (auto edge_p : _m_edges) {
        auto& strokeWidth = _p_graphAttributes->strokeWidth(edge_p);
        strokeWidth = width;
    }
}

void ogdfWrapper::colorNode(ogdf::NodeElement* node_p, NodeColor color) {
    auto colorName = getColor(color);
    auto& stroke_color = _p_graphAttributes->strokeColor(node_p);
    stroke_color = ogdf::Color(ogdf::Color::Name::Black);
    auto& fill_color = _p_graphAttributes->fillColor(node_p);
    fill_color = ogdf::Color(colorName);
}

void ogdfWrapper::colorNode(NodeID nodeID, NodeColor color) {
    auto& node_p = _label_map.at(nodeID);
    colorNode(node_p, color);
}

void ogdfWrapper::resetEdgeStyleAll() {
    for (auto edge_p : _m_edges) {
        auto& stroke_color = _p_graphAttributes->strokeColor(edge_p);
        stroke_color = ogdf::Color(ogdf::Color::Name::Black);
        auto& stroke_width = _p_graphAttributes->strokeWidth(edge_p);
        stroke_width = 1.f;
    }
}

int ogdfWrapper::removeNodes(int maxDegree, bool onlyEdges) {
    int removedEntities = 0;
    std::unordered_set<ogdf::EdgeElement*> edgesToRemove;
    std::vector<ogdf::NodeElement*> nodesToRemove;
    for (auto node_p : _m_nodes) {
        if (node_p->degree() > maxDegree) {
            if (onlyEdges) {
                for (const auto edge_p : _m_edges) {
                    if (edge_p->source() == node_p ||
                        edge_p->target() == node_p) {
                        edgesToRemove.insert(edge_p);
                    }
                }
            } else {
                nodesToRemove.push_back(node_p);
            }
        }
    }
    if (onlyEdges) {
        for (auto edge_p : edgesToRemove) {
            _p_graph->delEdge(edge_p);
        }
        removedEntities = edgesToRemove.size();
    } else {
        for (auto node_p : nodesToRemove) {
            _p_graph->delNode(node_p);
        }
        removedEntities = nodesToRemove.size();
    }
    _updateGraph();
    std::cout << "removed " << removedEntities
              << (onlyEdges ? " Edges." : " Nodes.") << std::endl;
    return removedEntities;
}

void ogdfWrapper::layout() const {
    switch (_layoutType) {
        case LayoutType::FMMM: {
            ogdf::FMMMLayout fmmmLayout;
            _setOptions(fmmmLayout);
            fmmmLayout.call(*_p_graphAttributes);
        } break;
        case LayoutType::SUGIYAMA: {
            ogdf::SugiyamaLayout sugiyamaLayout;
            sugiyamaLayout.setRanking(new ogdf::OptimalRanking);
            sugiyamaLayout.setCrossMin(new ogdf::MedianHeuristic);
            ogdf::OptimalHierarchyLayout* ohl =
                new ogdf::OptimalHierarchyLayout;
            ohl->layerDistance(30.0);
            ohl->nodeDistance(25.0);
            ohl->weightBalancing(0.8);
            sugiyamaLayout.setLayout(ohl);
            sugiyamaLayout.call(*_p_graphAttributes);
        } break;
    }
}

void ogdfWrapper::_setOptions(ogdf::FMMMLayout& layout) const {
    layout.useHighLevelOptions(true);
    layout.unitEdgeLength(30.0);
    layout.newInitialPlacement(true);
    layout.qualityVersusSpeed(
        ogdf::FMMMOptions::QualityVsSpeed::GorgeousAndEfficient);
    // layout.repForcesStrength(layout.repForcesStrength() * 2);
    // layout.springStrength(layout.springStrength() * 2);
}

void ogdfWrapper::writeGraph(const std::string& filename, FileType format) {
    std::fstream file(filename, file.out);
    ogdf::GraphIO::SVGSettings settings;
    switch (format) {
        case FileType::GML:
            ogdf::GraphIO::writeGML(*_p_graphAttributes, file);
            break;
        case FileType::SVG:
            // rebuild ogdf with updated CompressSVG.patch if textRendering is
            // not found
#ifdef OGDF_TEXTRENDERING_PATCH
            settings.textRendering(_m_labelRendering);
#endif
            ogdf::GraphIO::drawSVG(*_p_graphAttributes, file, settings);
            break;
        default:
            throw std::invalid_argument("Unknown FileType");
    }
    std::cout << "Wrote " << filename << std::endl;
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
    const auto& nodeS_p = _label_map.at(nodeStart);
    const auto& nodeE_p = _label_map.at(nodeEnd);
    for (const auto edge_p : _m_edges) {
        if ((edge_p->source() == nodeS_p && edge_p->target() == nodeE_p) ||
            (edge_p->source() == nodeE_p && edge_p->target() == nodeS_p)) {
            _p_graph->delEdge(edge_p);
            break;
        }
    }
}

void ogdfWrapper::setZ(NodeID nodeID, double z) {
    const auto& node_p = _label_map.at(nodeID);
    auto& node_z = _p_graphAttributes->z(node_p);
    node_z = z;
}

void ogdfWrapper::setLayoutType(LayoutType type) {
    _layoutType = type;
}

bool ogdfWrapper::addNode(NodeID nodeID) {
    if (hasNode(nodeID)) {
        return false;
    }
    const auto node_p = _p_graph->newNode();
    _p_graphAttributes->label(node_p) = nodeID.toString();
    _label_map[nodeID] = node_p;
    setNodeShape(nodeID);
    return true;
}

bool ogdfWrapper::removeNode(NodeID nodeID) {
    if (!hasNode(nodeID)) {
        return false;
    }
    _p_graph->delNode(_label_map.at(nodeID));
    _label_map.erase(nodeID);
    return true;
}

void ogdfWrapper::moveToCenter(NodeID node, const std::vector<uint>& anchors) {
    if (!hasNode(node)) {
        return;
    }
    double x = 0;
    double y = 0;
    int visibleCount = 0;
    for (const auto& anchor : anchors) {
        graphdrawer::NodeID anchorNode = {anchor,
                                          graphdrawer::NodeType::LITERAL};
        if (hasNode(anchorNode)) {
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

std::vector<NodeID> ogdfWrapper::nodesInRectangle(float xMin, float xMax,
                                                  float yMin, float yMax) {
    auto graphWidth = _p_graphAttributes->boundingBox().width();
    auto graphHeight = _p_graphAttributes->boundingBox().height();
    std::vector<NodeID> containedNodes;
    xMin *= graphWidth;
    xMax *= graphWidth;
    yMin *= graphHeight;
    yMax *= graphHeight;

    for (auto node : _m_nodes) {
        if (xMin <= _p_graphAttributes->x(node) &&
            _p_graphAttributes->x(node) <= xMax &&
            yMin <= _p_graphAttributes->y(node) &&
            _p_graphAttributes->y(node) <= yMax) {
            containedNodes.push_back(_nodeIDforPointer(node));
        }
    }

    return containedNodes;
}

std::unordered_map<NodeID, std::pair<double, double>> ogdfWrapper::
    getLayoutCoordinates() const {
    std::unordered_map<NodeID, std::pair<double, double>> layout_coordinates;

    for (auto [label, node] : _label_map) {
        auto x_coordinate = _p_graphAttributes->x(node);
        auto y_coordinate = _p_graphAttributes->y(node);

        layout_coordinates[label] = std::make_pair(x_coordinate, y_coordinate);
    }

    return layout_coordinates;
}

void ogdfWrapper::toggleLabelRendering() {
    _m_labelRendering = !_m_labelRendering;
}

void ogdfWrapper::exportLayout(const std::string& filename) {
    std::ofstream layout(filename);
    layout << "size " << _label_map.size() << std::endl;
    for (auto [label, node] : _label_map) {
        layout << label.toString() << " " << _p_graphAttributes->x(node) << " "
               << _p_graphAttributes->y(node) << std::endl;
    }
}

void ogdfWrapper::importLayout(const std::string& filename) {
    std::ifstream layout(filename);
    std::string header;
    unsigned int size;
    layout >> header >> size;
    std::unordered_set<ogdf::NodeElement*> nodesToKeep;
    for (int i = 0; i < size; ++i) {
        std::string name;
        layout >> name;
        auto node = _label_map[NodeID::fromString(name)];
        layout >> _p_graphAttributes->x(node) >> _p_graphAttributes->y(node);
        nodesToKeep.insert(node);
    }
    for (auto node_p : _m_nodes) {
        if (nodesToKeep.find(node_p) == nodesToKeep.end()) {
            removeNode(_nodeIDforPointer(node_p));
        }
    }
}

}  // namespace graphdrawer
