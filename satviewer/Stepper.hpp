#include <QApplication>
#include <ogdfWrapper/ogdfWrapper.hpp>

enum class StepType { SET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART, LEARNEDCLAUSE, UNLEARNEDCLAUSE };

struct Step {
    StepType type;
    // either node or backtrack level or restart number
    union {
        uint data; // for where we don't care what kind of step it is
        uint level;
        uint numberOfRestarts;
        uint variable;
    };
    bool nodeValue;
    graphdrawer::NodeID inline nodeID() const { return { variable, graphdrawer::NodeType::LITERAL }; };
};

struct Clause {
    StepType type;
    uint id;
    std::vector<uint> variables;
    graphdrawer::NodeID inline nodeID() const { return { id, graphdrawer::NodeType::LITERAL }; };
};

class Stepper {
public:
    std::string initialize(std::string cnfPath, bool foceSolve);
    std::string step();
    std::string branch();
    std::string nextConflict();
    std::string nextRestart();
    void backtrack(int level);
    bool isFinished();
    std::string cull(int degree);
    std::string relayout();

protected:

    void loadFromGML(std::string glmPath);
    void readTrace(std::string tracePath);
    StepType readTraceStep();
    void readBlock(char & type, int & data);
    // returns true if a node has been colored
    void applyClause(int i = -1);
    bool applyStep(int i = -1);
    void stepUntil(StepType stepType);
    void printProgress();

    std::vector<Step> m_eventStack;
    std::vector<Clause> m_learnedClauses;
    graphdrawer::ogdfWrapper m_graph;
    std::ifstream m_tracefile;
    int64_t m_tracefileSize;
    int64_t m_readBlocks;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat-solver";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

    std::string m_svgPath;
    std::string m_gmlPath;

    int m_lastCull;
    int m_branchCount;

};
