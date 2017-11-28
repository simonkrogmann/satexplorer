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


std::map <std::string , StepType> stepToEnum
        {
                {"Backtrack", StepType::BACKTRACK},
                {"Set", StepType::SET},
                {"Unset", StepType::UNSET},
                {"Conflict", StepType::CONFLICT},
                {"Enter", StepType::LEVEL},
                {"Branch", StepType::BRANCH},
                {"Restart", StepType::RESTART}
        };


std::string Stepper::initialize(std::string cnfPath){
    auto cnfFilename = QFileInfo(QString::fromStdString(cnfPath)).baseName().toStdString();
    auto solutionPath = outputPath + cnfFilename + ".solution";
    auto tracePath = outputPath + cnfFilename + ".trace";
    auto gmlPath = outputPath + cnfFilename + ".gml";
    svgPath = outputPath + cnfFilename + ".svg";

    QProcess process;
    auto conversionCmd = scriptPath + conversionScript + " " + cnfPath + " " + gmlPath;
    std::cout << "Convert to gml for layouting" << std::endl;
    std::cout << conversionCmd << std::endl;
    process.start(QString::fromStdString(conversionCmd));
    std::cout << "converting..." << std::endl;
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    auto satCmd = minisat + " " + cnfPath + " " + solutionPath + " " + tracePath;
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

std::string Stepper::step() {
    bool stepFinished = false;
    while(m_lastStep < m_steps.size() && !stepFinished) {
        auto [type, data] = m_steps[m_lastStep];
        ++m_lastStep;
        stepFinished = true;
        switch(type) {
            case StepType::SET:
                m_graph.colorNode(data, graphdrawer::nodeColor::PROCESSED);
                break;
            case StepType::UNSET:
                m_graph.colorNode(data, graphdrawer::nodeColor::UNPROCESSED);
                break;
            default:
                stepFinished = false;
        }
    }
    m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);
    return svgPath;
}

void Stepper::writeSvg(std::string gmlPath, std::string svgPath) {
    m_graph.readGraph(gmlPath);
    m_graph.setNodeShapeAll();
    m_graph.colorEdges();
    m_graph.colorNodes(graphdrawer::nodeColor::UNPROCESSED);
    m_graph.removeNodes(10);
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

            m_steps.push_back({stepToEnum.at(word), std::stoi(number)});
        }
    }
    traceFile.close();
}
