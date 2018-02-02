#pragma once

#include <ogdfWrapper/ogdfWrapper.hpp>
#include <ogdfWrapper/types.hpp>

#include <fstream>
#include <string>

enum class StepType {
    SET,
    BACKTRACK,
    CONFLICT,
    LEVEL,
    BRANCH,
    RESTART,
    LEARNEDCLAUSE,
    UNLEARNEDCLAUSE
};

struct Step {
    StepType type;
    // either node or backtrack level or restart number
    union {
        uint data;  // for where we don't care what kind of step it is
        uint level;
        uint numberOfRestarts;
        uint variable;
    };
    bool nodeValue;
    graphdrawer::NodeID inline nodeID() const {
        return {variable, graphdrawer::NodeType::LITERAL};
    };
};

struct Clause {
    StepType type;
    uint id;
    std::vector<uint> variables;
    graphdrawer::NodeID inline nodeID() const {
        return {id, graphdrawer::NodeType::LITERAL};
    };
};

/*
    Creates and holds Graph of SAT instance through ogdfWrapper
    Reads and processes information from tracefile
        tracefile is generated by minisat (sat-solver)
*/
class Stepper {
public:
    void initialize(std::string cnfPath, bool forceSolve, bool showSimplified);
    /*
        read,parse and execute one step of the tracefile
    */
    void step();

    /*
        execute steps until the next branch step
    */
    void branch();

    /*
        execute steps until the next conflict step
    */
    void nextConflict();

    /*
        skip forward to the next restart step
    */
    void nextRestart();

    /*
        skip forward to the last restart step in the tracefile
    */
    void lastRestart();
    void backtrack(int level);
    bool isFinished();
    /*
        remove edges that have at least one endpoint with degree higher than the
       parameter
    */
    void cull(int degree);
    void relayout();
    const std::string getSVGPath() const;

protected:
    void loadFromGML(std::string glmPath);
    void readTrace(std::string tracePath);
    /*
        Reads and parses one entry from the tracefile
    */
    StepType readTraceStep();

    /*
        Reads one entry from the tracefile
        Tracefile is in binary. Format is char followed by int
    */
    void readBlock(char& type, int& data);
    // returns true if a node has been colored
    void applyClause(int i = -1);
    bool applyStep(int i = -1);
    void stepUntil(StepType stepType, bool layout);
    void printProgress();

    std::vector<Step> m_eventStack;
    std::vector<Clause> m_learnedClauses;
    graphdrawer::ogdfWrapper m_graph;
    std::ifstream m_tracefile;
    int64_t m_tracefileSize;
    int64_t m_readBlocks;
    int m_numberOfRestarts;
    int m_currentLevel = 0;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat-solver";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

    std::string m_svgPath;
    std::string m_gmlPath;

    int m_lastCull;
    int m_branchCount;
};
