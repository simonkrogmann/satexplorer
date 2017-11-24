#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <ogdfWrapper>

class ogdfWrapper;

class Stepper {
public:
    void initialize(String cnfPath);
    void step();

protected:
    std::vector<String> m_steps;
    size_t m_lastStep;
    ogdfWrapper m_graph;

    const String conversionSkript = "./cnfToGlm.py";
    const String minisat = "./minisat";
    String m_solutionPath;
    String m_tracePath;
};

