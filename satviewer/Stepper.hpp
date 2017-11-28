#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper/ogdfWrapper.hpp>

class ogdfWrapper;

enum class Step { SET, UNSET, BACKTRACK, CONFLICT, LEVEL, BRANCH, RESTART };

class Stepper {
public:
    std::string initialize(std::string cnfPath);
    void step();

protected:

    void writeSvg(std::string glmPath, std::string svgPath);
    void readTrace(std::string tracePath);

    std::vector<std::pair<Step, int>> m_steps;
    size_t m_lastStep;
    graphdrawer::ogdfWrapper m_graph;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";

};

