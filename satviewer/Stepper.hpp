#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper/ogdfWrapper.hpp>

struct Step {
    StepType type;
    // either node or backtrack level or restart number
    int data;
};

enum class StepType { SET, UNSET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART };

class Stepper {
public:
    std::string initialize(std::string cnfPath);
    void step();

protected:

    void writeSvg(std::string glmPath, std::string svgPath);
    void readTrace(std::string tracePath);

    std::vector<Step> m_steps;
    size_t m_lastStep;
    graphdrawer::ogdfWrapper m_graph;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

};

