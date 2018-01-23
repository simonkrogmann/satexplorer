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

namespace graphdrawer {
enum class NodeColor {
    UNPROCESSED,
    SET_TRUE,
    SET_FALSE,
    BRANCH_TRUE,
    BRANCH_FALSE,
    CONFLICT,
    STEP_SELECTED
};

enum class LayoutType { FMMM, SUGIYAMA };

enum class NodeType { CLAUSE, LITERAL };

// used to distinguish between clause and literal nodes
struct NodeID {
    uint id;
    NodeType type;

    static NodeID clause(uint id) { return NodeID{id, NodeType::CLAUSE}; }

    static NodeID literal(uint id) { return NodeID{id, NodeType::LITERAL}; }

    inline bool operator==(const NodeID& other) const {
        return this->id == other.id && this->type == other.type;
    }
};

}  // namespace graphdrawer

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

    void colorNodes(NodeColor color);

    void setNodeShapeAll(double width = 20.0, double height = 20.0);

    void setNodeShape(NodeID nodeID, double width = 20.0, double height = 20.0);

    void setStrokeWidth(float width);

    void colorNode(NodeID nodeID, NodeColor color);

    void colorEdges();

    bool hasNode(NodeID nodeID);

    void addEdge(NodeID nodeStart, NodeID nodeEnd);

    void removeEdge(NodeID nodeStart, NodeID NodeEnd);

    // sets the 'height' of the node
    // nodes with higher z get written to svg first
    void setZ(NodeID nodeID, double z);

    void setLayoutType(LayoutType type);

    // returns 1 if the node was added to the label map (label is an integer)
    // returns 0 if the nodeID is already present in the label map
    // returns 2 if the nodeID is not an int
    bool addNode(NodeID nodeID);

    bool removeNode(NodeID nodeID);

    void moveToCenter(NodeID node, const std::vector<uint>& anchors);

private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;
    ogdfVector<ogdf::EdgeElement*> _m_edges;
    std::unordered_map<NodeID, ogdf::NodeElement*> _label_map;
    LayoutType _layoutType;

    void _setOptions(ogdf::FMMMLayout& layout);
    void _updateGraph();
    void colorNode(ogdf::NodeElement* node_p, NodeColor color);
};
}  // namespace graphdrawer
