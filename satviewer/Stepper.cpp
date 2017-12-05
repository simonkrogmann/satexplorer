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
    while(!m_tracefile.eof() && !stepFinished) {
        auto& step = readTraceStep();
        ++m_lastStep;
        stepFinished = parseStep(step);
    }
    m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);
    return svgPath;
}

std::string Stepper::branch() {
    bool branchFinished = false;
    bool lastWasBranch = false;
    while(!m_tracefile.eof() && !branchFinished) {
        auto& step = readTraceStep();
        ++m_lastStep;
        if(step.type == StepType::BACKTRACK) {
            branchFinished = true;
        }
        else
        {
            if(!lastWasBranch) {
                parseStep(step);
            }
            else
            {
                lastWasBranch = false;
            }
        }
        if(step.type == StepType::BRANCH) {
            lastWasBranch = true;
        }
    }
    m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);
    return svgPath;
}

bool Stepper::parseStep(Step step) {
    if(!m_graph.hasNode(step.node)) return false;
    bool nodeColored = true;
    switch(step.type) {
        case StepType::SET:
            if(step.nodeValue) m_graph.colorNode(step.node, graphdrawer::nodeColor::SET_TRUE);
            else m_graph.colorNode(step.node, graphdrawer::nodeColor::SET_FALSE);
            break;
        case StepType::UNSET:
            m_graph.colorNode(step.node, graphdrawer::nodeColor::UNPROCESSED);
            break;
        case StepType::BRANCH:
            if(step.nodeValue) m_graph.colorNode(step.node, graphdrawer::nodeColor::BRANCH_TRUE);
            else m_graph.colorNode(step.node, graphdrawer::nodeColor::BRANCH_FALSE);
            break;
        default:
            nodeColored = false;
    }
    return nodeColored;
}

void Stepper::writeSvg(std::string gmlPath, std::string svgPath) {
    m_graph.readGraph(gmlPath);
    m_graph.setNodeShapeAll();
    m_graph.colorEdges();
    m_graph.colorNodes(graphdrawer::nodeColor::UNPROCESSED);
    //m_graph.removeNodes(10);
    m_graph.layout();
    m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);
}


// opens the tracefile
void Stepper::readTrace(std::string tracePath) {
    m_tracefile.open(tracePath, std::ios::binary | std::ios::in);
    assert(m_tracefile.is_open());
}


// tracefile is encoded in binary
// structure is char followed by int
// char denotes StepType, int denotes Node
Step& Stepper::readTraceStep() {
    if(!m_tracefile.eof() && m_tracefile.is_open())
    {
        char type;
        int data;

        m_tracefile.read(&type, sizeof(type));
        m_tracefile.read(reinterpret_cast<char*>(&data), sizeof(data));

        std::cout << type << " " << data << std::endl;
        bool value = data >= 0;

        StepType stepType;
        switch(type) {
            case '+': stepType = StepType::SET; break;
            case '-': stepType = StepType::UNSET; break;
            case '>': stepType = StepType::LEVEL; break;
            case '<': stepType = StepType::BACKTRACK; break;
            case 'R': stepType = StepType::RESTART; break;
            case 'C': stepType = StepType::CONFLICT; break;
            case 'B': stepType = StepType::BRANCH; break;
        }

        m_steps.push_back({stepType, abs(data), value});

        return m_steps.back();
    }
    throw std::runtime_error("End of tracefile or tracefile is closed");
}

bool Stepper::isFinished() {
    return m_tracefile.eof();
}