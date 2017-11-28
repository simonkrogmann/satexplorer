#include "Stepper.hpp"

#include <cassert>
#include <fstream>
#include <map>
#include <string>
#include <utility>

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QFileInfo>
#include <ogdfWrapper/types.hpp>


std::map <std::string , Step> stepToEnum
        {
                {"Backtrack", Step::BACKTRACK},
                {"Set", Step::SET},
                {"Unset", Step::UNSET},
                {"Conflict", Step::CONFLICT},
                {"Enter", Step::LEVEL},
                {"Branch", Step::BRANCH},
                {"Restart", Step::RESTART}
        };


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

    readTrace(tracePath);
    writeSvg(gmlPath, svgPath);

    return svgPath;
}

void Stepper::step() {

}

void Stepper::writeSvg(std::string gmlPath, std::string svgPath) {
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
}

void Stepper::readTrace(std::string tracePath) {
    std::ifstream traceFile;
    traceFile.open(tracePath);
    assert(traceFile.is_open());

    while(!traceFile.eof() && traceFile.is_open())
    {
        std::string line;

        std::getline(traceFile, line);
        if (!line.empty()) {
            // convert to pair of enum and int
            auto wordEnd = line.find(" ");
            auto word = line.substr(0, wordEnd);

            auto numberStart = line.rfind(" ");
            auto number = line.substr(numberStart + 1);

            auto step = std::make_pair(stepToEnum.at(word), std::stoi(number));
            m_steps.push_back(step);
        }
    }
    traceFile.close();
}