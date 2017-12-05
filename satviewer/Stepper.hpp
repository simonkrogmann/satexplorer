#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper/ogdfWrapper.hpp>

enum class StepType { SET, UNSET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART };

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
    std::string initialize(std::string cnfPath);
    std::string step();
    std::string branch();

protected:

    void writeSvg(std::string glmPath, std::string svgPath);
    void readTrace(std::string tracePath);
    Step& readTraceStep();
    // returns true if a node has been colored
    bool parseStep(Step step);

    std::vector<Step> m_steps;
    size_t m_lastStep;
    graphdrawer::ogdfWrapper m_graph;
    std::ifstream m_tracefile;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat-solver";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

    std::string svgPath;

};
