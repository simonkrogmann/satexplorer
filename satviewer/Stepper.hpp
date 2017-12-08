#include <QApplication>
#include <ogdfWrapper/ogdfWrapper.hpp>

enum class StepType { SET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART };

struct Step {
    StepType type;
    // either node or backtrack level or restart number
    union {
        int data; // for where we don't care what kind of step it is
        int level;
        int node;
        int numberOfRestarts;
    };
    bool nodeValue;
};

class Stepper {
public:
    std::string initialize(std::string cnfPath, bool foceSolve);
    std::string step();
    std::string branch();
    void backtrack(int level);
    bool isFinished();
    std::string cull(int degree);

protected:

    void loadFromGML(std::string glmPath);
    void readTrace(std::string tracePath);
    Step& readTraceStep();
    // returns true if a node has been colored
    bool parseStep(const Step & step);

    std::vector<Step> m_eventStack;
    graphdrawer::ogdfWrapper m_graph;
    std::ifstream m_tracefile;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat-solver";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

    std::string m_svgPath;
    std::string m_gmlPath;

    int m_lastCull;

};
