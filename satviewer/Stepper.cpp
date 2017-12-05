#include "Stepper.hpp"

#include <cassert>
#include <fstream>
#include <map>
#include <string>
#include <utility>

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
    m_lastCull = std::numeric_limits<int>::max();
    auto cnfFilename = QFileInfo(QString::fromStdString(cnfPath)).baseName().toStdString();
    auto solutionPath = outputPath + cnfFilename + ".solution";
    auto tracePath = outputPath + cnfFilename + ".trace";
    m_gmlPath = outputPath + cnfFilename + ".gml";
    m_svgPath = outputPath + cnfFilename + ".svg";

    QProcess process;
    auto conversionCmd = scriptPath + conversionScript + " " + cnfPath + " " + m_gmlPath;
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
    writeSvg(m_gmlPath, m_svgPath);

    return m_svgPath;
}

std::string Stepper::step() {
    bool stepFinished = false;
    while(!m_tracefile.eof() && !stepFinished) {
        auto& step = readTraceStep();
        stepFinished = parseStep(step);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}

std::string Stepper::branch()
{
    while(!m_tracefile.eof())
    {
        auto& step = readTraceStep();
        if(step.type == StepType::BACKTRACK)
        {
            m_graph.writeGraph(svgPath, graphdrawer::filetype::SVG);
            parseStep(step);
            break;
        }
        parseStep(step);
    }
<<<<<<< HEAD
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
=======
    return svgPath;
>>>>>>> Decrease trace file size
}

void Stepper::backtrack(int level)
{
    while (true)
    {
        auto step = m_eventStack.back();
        m_eventStack.pop_back();
        switch(step.type) {
            case StepType::SET:
            case StepType::BRANCH:
                if(m_graph.hasNode(step.node))
                    m_graph.colorNode(step.node, graphdrawer::nodeColor::UNPROCESSED);
                break;
            case StepType::LEVEL:
                if (step.level == level)
                    return;
            default:
                break;
        }
    }
}

bool Stepper::parseStep(Step step) {
    bool nodeColored = true;
    switch(step.type) {
        case StepType::SET:
            if(!m_graph.hasNode(step.node)) return false;
            m_graph.colorNode(step.node, step.nodeValue ? graphdrawer::nodeColor::SET_TRUE : graphdrawer::nodeColor::SET_FALSE);
            break;
        case StepType::BACKTRACK:
            backtrack(step.level);
            break;
        case StepType::BRANCH:
            if(!m_graph.hasNode(step.node)) return false;
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

        m_eventStack.push_back({stepType, abs(data), value});

        return m_eventStack.back();
    }
    throw std::runtime_error("End of tracefile or tracefile is closed");
}

bool Stepper::isFinished() {
    return m_tracefile.eof();
}

std::string Stepper::cull(int degree) {
    if(m_lastCull < degree) {
        writeSvg(m_gmlPath, m_svgPath);
        for(auto& step : m_eventStack)
        {
            parseStep(step);
        }
    }
    m_lastCull = degree;
    m_graph.removeNodes(degree);
    m_graph.layout();
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}
