#pragma once

#include <cassert>
#include <string>

namespace graphdrawer {
enum class FileType { GML, SVG };

enum class NodeColor {
    UNPROCESSED,
    SET_TRUE,
    SET_FALSE,
    BRANCH_TRUE,
    BRANCH_FALSE,
    CONFLICT,
    STEP_SELECTED,
    Brown,
    Cyan,
    Gray,
    Yellow,
    Purple,
    Pink,
    Olive,
    Yellowgreen  // this has to stay the last color
};

const int numNodeColors =
    static_cast<int>(graphdrawer::NodeColor::Yellowgreen) + 1;

// view http://amber-v7.cs.tu-dortmund.de/doku.php/tech:layouter for an
// explanation of different layout types
enum class LayoutType { FMMM, SUGIYAMA };

enum class NodeType { CLAUSE, LITERAL };

namespace {

NodeType nodeTypeFromCharacter(char c) {
    switch (c) {
        case 'l':
            return NodeType::LITERAL;
        case 'c':
            return NodeType::CLAUSE;
    }
    assert(false);
}

char characterFromNodeType(NodeType n) {
    switch (n) {
        case NodeType::LITERAL:
            return 'l';
        case NodeType::CLAUSE:
            return 'c';
    }
    assert(false);
}

}  // namespace

// used to distinguish between clause and literal nodes
struct NodeID {
    unsigned int id;
    NodeType type;

    static NodeID clause(unsigned int id) {
        return NodeID{id, NodeType::CLAUSE};
    }

    static NodeID literal(unsigned int id) {
        return NodeID{id, NodeType::LITERAL};
    }

    static NodeID fromCharType(char type, unsigned int id) {
        return {id, nodeTypeFromCharacter(type)};
    }

    static NodeID fromString(const std::string& s) {
        // std::stoui doesn't exist for some reason
        return {static_cast<unsigned int>(std::stoul(s.substr(1))),
                nodeTypeFromCharacter(s[0])};
    }

    inline bool operator==(const NodeID& other) const {
        return this->id == other.id && this->type == other.type;
    }

    inline std::string toString() const {
        return characterFromNodeType(type) + std::to_string(id);
    }
};

}  // namespace graphdrawer
