#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper/ogdfWrapper.hpp>

class ogdfWrapper;

class Stepper {
public:
    std::string initialize(std::string cnfPath);
    void step();

protected:
    std::vector<std::string> m_steps;
    size_t m_lastStep;
    graphdrawer::ogdfWrapper m_graph;

    const std::string conversionScript = "cnfToGML.py";
    const std::string minisat = "./minisat";
    const std::string outputPath = "data/";
    const std::string scriptPath = "../scripts/";
};

