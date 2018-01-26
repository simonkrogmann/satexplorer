#pragma once

namespace graphdrawer {
enum class FileType { GML, SVG };

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
