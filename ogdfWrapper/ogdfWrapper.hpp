#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <types.hpp>
#include <memory>

namespace graphdrawer {
class ogdfWrapper {
public:
    // read graph from file in graphml format
    void readGraph(std::string filename);

    void process();

    void writeGraph(std::string filename, filetype format);
private:
    std::unique_ptr<ogdf::Graph> _p_graph;
    std::unique_ptr<ogdf::GraphAttributes> _p_graphAttributes;

    void set_options(ogdf::FMMMLayout& layout);
    
};
}