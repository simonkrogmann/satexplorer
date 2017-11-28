#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper/ogdfWrapper.hpp>

enum class StepType { SET, UNSET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART };

struct Step {
    StepType type;
    // either node or backtrack level or restart number
    int data;
};

class Stepper {
public:
    std::string initialize(std::string cnfPath);
    std::string step();

protected:

    void writeSvg(std::string glmPath, std::string svgPath);
    void readTrace(std::string tracePath);

    std::vector<Step> m_steps;
    size_t m_lastStep;
    graphdrawer::ogdfWrapper m_graph;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "../minisat/build/minisat";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

    std::string svgPath;

};

