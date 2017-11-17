#pragma once

#include <ogdf/basic/basic.h>

#include <memory>
#include <map>
#include <string>

#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/Array.h>
#include "ogdfVector.hpp"
#include "types.hpp"


namespace graphdrawer {

class ogdfWrapper {
public:
    enum class nodeColor {
        UNPROCESSED,
        PROCESSED,
        STEP_SELECTED
    };
    // read graph from file in graphml format
    void readGraph(std::string filename);

    // set layout options and layout the graph
    // using FMMM Layout
    void layout();

    // write the graph to <filename>.<format>
    void writeGraph(std::string filename, filetype format);

    // removes all Nodes with degree > maxDegree from the Graph
    int removeNodes(int maxDegree);

    void colorNodes(nodeColor color);

    void setNodeShapeAll(double width, double height);

    void setNodeShape(int nodeID, double width, double height);

    void colorNode(int nodeID, nodeColor color);

    void colorEdges();



private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;
    ogdfVector<ogdf::EdgeElement*> _m_edges;
    std::map<std::string, ogdf::NodeElement*> _label_map;

    void _setOptions(ogdf::FMMMLayout& layout);
    void _updateGraph();
    ogdf::Color::Name getColor(nodeColor color);
    
};
}
