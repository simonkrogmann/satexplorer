#include "Stepper.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>

void Stepper::initialize(String cnfPath){
    QProcess process;
    auto conversionCmd = conversionSkript + " " + cnfPath;
    log("Convert to gml for layouting");
    log(conversionCmd);
    process.start(QString::fromStdString(conversionCmd));
    std::cout << "converting..." << std::endl;
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    // TODO: write lambda that gets rid of the ".cnf" at the end before adding any more
    m_solutionPath = cnfPath + "Solution";
    m_tracePath = cnfPath + "Trace";

    auto satCmd = minisat + " " + cnfPath + m_solutionPath + m_tracePath;
    log("solve SAT instance");
    log(satCmd);
    process.start(QString::fromStdString(satCmd));
    std::cout << "solving..." << std::endl;
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    // read Trace and solve into m_steps

    m_graph->readGraph("wherever we save the .gml to");
    m_graph->layout();
}

void Stepper::Step() {

}

