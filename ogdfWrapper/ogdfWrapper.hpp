#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/Array.h>
#include <ogdfVector.hpp>
#include <types.hpp>
#include <memory>

namespace graphdrawer {
class ogdfWrapper {
public:
    // read graph from file in graphml format
    void readGraph(std::string filename);

    // set layout options and layout the graph
    // using FMMM Layout
    void layout();

    // write the graph to <filename>.<format>
    void writeGraph(std::string filename, filetype format);

    // removes all Nodes with degree > maxDegree from the Graph
    int removeNodes(int maxDegree);

    void colorNodes();

private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;
    ogdfVector<ogdf::NodeElement*> _m_nodes;

    void set_options(ogdf::FMMMLayout& layout);
    
};
}