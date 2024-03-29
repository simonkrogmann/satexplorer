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
enum class Shape;
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
        }
        assert(false);
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

    // read graph from file in GML format
    void readGraph(const std::string& filename);

    // set layout options and layout the graph
    // uses _layoutType to switch between layouting methods
    // view http://amber-v7.cs.tu-dortmund.de/doku.php/tech:layouter for an
    // explanation of different layout types
    void layout() const;

    // write the graph to <filename>.<format>
    void writeGraph(const std::string& filename, FileType format);

    // removes all Nodes with degree > maxDegree from the Graph
    // onlyEdges will keep the high degree Nodes and just remove their edges
    int removeNodes(int maxDegree, bool onlyEdges = false);

    // sets the node shape of every node to elliptical
    // and gives each node the given dimensions
    void resetNodeShapeAll(double width = 20.0, double height = 20.0);

    // sets the node shape of a specific node to elliptical and gives it the
    // given dimensions
    void setNodeShape(NodeID nodeID, double width = 20.0, double height = 20.0);

    void setEdgeWidth(float width);

    // colors the node in the given color
    void colorNode(NodeID nodeID, NodeColor color);
    void colorNode(ogdf::NodeElement* node_p, NodeColor color);

    void colorAllNodes(NodeColor color);
    // colors the nodes in the array with the given color
    void colorNodes(NodeColor color, const std::vector<NodeID>& nodes);

    // colors all edges black, and sets the stroke width to 1
    void resetEdgeStyleAll();

    bool hasNode(NodeID nodeID);

    void addEdge(NodeID nodeStart, NodeID nodeEnd);

    void removeEdge(NodeID nodeStart, NodeID NodeEnd);

    // sets the 'height' of the node
    // nodes with higher z are rendered on top
    void setZ(NodeID nodeID, double z);

    // set a layout algorithm to use for layouting the graph
    void setLayoutType(LayoutType type);

    // returns true if the node was added to the label map (label is an integer)
    // returns false if the nodeID is already present in the label map
    bool addNode(NodeID nodeID);

    // returns true if the Node was successfully removed
    // returns false if the node does not exist in the graph
    bool removeNode(NodeID nodeID);

    // sets the x and y coordinate of node to the arithmetic middle of the nodes
    // in literals the ids within literals are treated as nodes of type LITERAL
    void moveToCenter(NodeID node, const std::vector<uint>& literals);

    std::vector<NodeID> nodesInRectangle(float xMin, float xMax, float yMin,
                                         float yMax);

    std::unordered_map<NodeID, std::pair<double, double>> getLayoutCoordinates()
        const;

    void toggleLabelRendering();

    void exportLayout(const std::string& filename);
    void importLayout(const std::string& filename);

private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;
    ogdfVector<ogdf::EdgeElement*> _m_edges;
    bool _m_labelRendering = false;
    std::unordered_map<NodeID, ogdf::NodeElement*> _label_map;
    LayoutType _layoutType = LayoutType::FMMM;

    void _setOptions(ogdf::FMMMLayout& layout) const;

    // rebuilds the label_map
    void _updateGraph();
    NodeID _nodeIDforPointer(ogdf::NodeElement* node_p);
    ogdf::Shape _nodeShapeforPointer(ogdf::NodeElement* node_p);
};
}  // namespace graphdrawer
