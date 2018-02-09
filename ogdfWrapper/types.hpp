#pragma once

#include <cassert>

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
    Yellowgreen
};

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

    inline bool operator==(const NodeID& other) const {
        return this->id == other.id && this->type == other.type;
    }
};

}  // namespace graphdrawer
