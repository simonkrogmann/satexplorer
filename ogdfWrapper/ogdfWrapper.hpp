#pragma once

#include <ogdf/basic/basic.h>

#include <memory>
#include <unordered_map>
#include <string>

#include "ogdfVector.hpp"
#include "types.hpp"

namespace ogdf
{
    class Graph;
    class GraphAttributes;
    class EdgeElement;
    class NodeElement;
    class FMMMLayout;
}


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

class ogdfWrapper {
public:

    ogdfWrapper();
    ~ogdfWrapper();

    // read graph from file in graphml format
    void readGraph(std::string filename);

    // set layout options and layout the graph
    // using FMMM Layout
    void layout();

    // write the graph to <filename>.<format>
    void writeGraph(std::string filename, filetype format);

    // removes all Nodes with degree > maxDegree from the Graph
    // onlyEdges will keep the high degree Nodes and just remove their edges
    int removeNodes(int maxDegree, bool onlyEdges = false);

    void colorNodes(NodeColor color);

    void setNodeShapeAll(double width = 20.0, double height = 20.0);

    void setNodeShape(int nodeID, double width = 20.0, double height = 20.0);

    void setStrokeWidth(float width);

    void colorNode(int nodeID, NodeColor color);

    void colorEdges();

    bool hasNode(int nodeID);

    void addEdge(int nodeStart, int nodeEnd);

    void removeEdge(int nodeStart, int NodeEnd);

    // sets the 'height' of the node
    // nodes with higher z get written to svg first
    void setZ(int nodeID, double z);


private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;
    ogdfVector<ogdf::EdgeElement*> _m_edges;
    std::unordered_map<int, ogdf::NodeElement*> _label_map;

    void _setOptions(ogdf::FMMMLayout& layout);
    void _updateGraph();
    void colorNode(ogdf::NodeElement* node_p, NodeColor color);


};
}
