#include "Stepper.hpp"

#include <cassert>

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QFileInfo>
#include <ogdfWrapper/types.hpp>


std::string Stepper::initialize(std::string cnfPath){
    auto cnfFilename = QFileInfo(QString::fromStdString(cnfPath)).baseName().toStdString();
    auto solutionPath = outputPath + cnfFilename + ".solution";
    auto tracePath = outputPath + cnfFilename + ".trace";
    auto gmlPath = outputPath + cnfFilename + ".gml";
    auto svgPath = outputPath + cnfFilename + ".svg";

    QProcess process;
    auto conversionCmd = scriptPath + conversionScript + " " + cnfPath + " " + gmlPath;
    std::cout << "Convert to gml for layouting" << std::endl;
    std::cout << conversionCmd << std::endl;
    process.start(QString::fromStdString(conversionCmd));
    std::cout << "converting..." << std::endl;
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    auto satCmd = minisat + " " + cnfPath + solutionPath + tracePath;
    std::cout << "solve SAT instance" << std::endl;
    std::cout << satCmd << std::endl;
    process.start(QString::fromStdString(satCmd));
    std::cout << "solving..." << std::endl;
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    // read Trace and solve into m_steps

    m_graph.readGraph(gmlPath);
    m_graph.setNodeShapeAll();
    m_graph.colorEdges();
    m_graph.removeNodes(10);
    m_graph.layout();
    m_graph.colorNodes(graphdrawer::ogdfWrapper::nodeColor::UNPROCESSED);
    m_graph.colorNode(1, graphdrawer::ogdfWrapper::nodeColor::PROCESSED);
    m_graph.setNodeShape(1);
    m_graph.colorNode(2, graphdrawer::ogdfWrapper::nodeColor::STEP_SELECTED);
    m_graph.setNodeShape(2);
    
    m_graph.layout();
    m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);

    return svgPath;
}

void Stepper::step() {

}

