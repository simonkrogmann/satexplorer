#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>

#include "ogdfVector.hpp"
#include "types.hpp"

namespace ogdf {
class Graph;
class GraphAttributes;
class EdgeElement;
class NodeElement;
class FMMMLayout;
}  // namespace ogdf

namespace std {
template <>
struct hash<graphdrawer::NodeID> {
    inline size_t operator()(const graphdrawer::NodeID& nodeID) const {
        switch (nodeID.type) {
            case graphdrawer::NodeType::CLAUSE:
                return hash<int>()(-1 * nodeID.id);
            case graphdrawer::NodeType::LITERAL:
                return hash<int>()(nodeID.id);
            default:
                assert(false);
        }
    }
};
}  // namespace std

namespace graphdrawer {

/*
    This class is a highlevel interface for the ogdf graph library
    It holds and manages a graph through the ogdf library.
    It provides methods to configure and draw the graph as an SVG.
*/
class ogdfWrapper {
public:
    ogdfWrapper();
    ~ogdfWrapper();

    // read graph from file in graphml format
    void readGraph(std::string filename);

    // set layout options and layout the graph
    // uses _layoutType to switch between layouting methods
    void layout();

    // write the graph to <filename>.<format>
    void writeGraph(std::string filename, filetype format);

    // removes all Nodes with degree > maxDegree from the Graph
    // onlyEdges will keep the high degree Nodes and just remove their edges
    int removeNodes(int maxDegree, bool onlyEdges = false);

    // colors all nodes with the given color
    void colorNodes(NodeColor color);

    // sets the node shape of every node to elliptical
    // and gives each node the given dimensions
    void setNodeShapeAll(double width = 20.0, double height = 20.0);

    // sets the node shape of a specific node to elliptical and gives it the
    // given dimensions
    void setNodeShape(NodeID nodeID, double width = 20.0, double height = 20.0);

    // sets the width of edges
    void setStrokeWidth(float width);

    // colors the node in the given color
    void colorNode(NodeID nodeID, NodeColor color);

    // colors all edges black, and resets the stroke width to 1
    void colorEdges();

    bool hasNode(NodeID nodeID);

    void addEdge(NodeID nodeStart, NodeID nodeEnd);

    void removeEdge(NodeID nodeStart, NodeID NodeEnd);

    // sets the 'height' of the node
    // nodes with higher z are rendered on top
    void setZ(NodeID nodeID, double z);

    // set a layout algorithm to use for layouting the graph
    void setLayoutType(LayoutType type);

    // returns 1 if the node was added to the label map (label is an integer)
    // returns 0 if the nodeID is already present in the label map
    // returns 2 if the nodeID is not an int
    bool addNode(NodeID nodeID);

    bool removeNode(NodeID nodeID);

    // sets the x and y coordinate of node to the arithmetic middle of the nodes
    // in literals the ids within literals are treated as nodes of type LITERAL
    void moveToCenter(NodeID node, const std::vector<uint>& literals);

private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;
    ogdfVector<ogdf::EdgeElement*> _m_edges;
    std::unordered_map<NodeID, ogdf::NodeElement*> _label_map;
    LayoutType _layoutType;

    void _setOptions(ogdf::FMMMLayout& layout);

    // rebuilds the label_map
    void _updateGraph();
    void colorNode(ogdf::NodeElement* node_p, NodeColor color);
};
}  // namespace graphdrawer
