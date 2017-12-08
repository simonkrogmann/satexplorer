#include "Stepper.hpp"

#include <cassert>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>

#include <QProcess>
#include <QFileInfo>
#include <ogdfWrapper/types.hpp>


std::unordered_map <char, StepType> stepFromCharacter
{
    {'<', StepType::BACKTRACK},
    {'+', StepType::SET},
    {'C', StepType::CONFLICT},
    {'>', StepType::LEVEL},
    {'B', StepType::BRANCH},
    {'R', StepType::RESTART}
};

std::string Stepper::initialize(std::string cnfPath, bool forceSolve){
    m_lastCull = std::numeric_limits<int>::max();
    auto cnfFilename = QFileInfo(QString::fromStdString(cnfPath)).baseName().toStdString();
    auto solutionPath = outputPath + cnfFilename + ".solution";
    auto tracePath = outputPath + cnfFilename + ".trace";
    m_gmlPath = outputPath + cnfFilename + ".gml";
    m_svgPath = outputPath + cnfFilename + ".svg";

    QProcess process;
    if(!std::ifstream(m_gmlPath) || forceSolve) { // check if conversion for this file has already taken place
        auto conversionCmd = scriptPath + conversionScript + " " + cnfPath + " " + m_gmlPath;
        std::cout << "Convert to gml for layouting" << std::endl;
        std::cout << conversionCmd << std::endl;
        process.start(QString::fromStdString(conversionCmd));
        std::cout << "converting..." << std::endl;
        process.waitForFinished(-1);
        std::cout << "Done." << std::endl;
    }

    if(!std::ifstream(tracePath) || forceSolve) { // check if this file has already been solved
        auto satCmd = minisat + " " + cnfPath + " " + solutionPath + " " + tracePath;
        std::cout << "solve SAT instance" << std::endl;
        std::cout << satCmd << std::endl;
        process.start(QString::fromStdString(satCmd));
        std::cout << "solving..." << std::endl;
        process.waitForFinished(-1);
        std::cout << "Done." << std::endl;
    }

    readTrace(tracePath);
    loadFromGML(m_gmlPath);
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);

    return m_svgPath;
}

std::string Stepper::step() {
    bool stepFinished = false;
    while(!m_tracefile.eof() && !stepFinished) {
        const auto& step = readTraceStep();
        stepFinished = parseStep(step);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}

std::string Stepper::branch()
{
    while(!m_tracefile.eof())
    {
        const auto& step = readTraceStep();
        if(step.type == StepType::BACKTRACK)
        {
            m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
            parseStep(step);
            break;
        }
        parseStep(step);
    }
    return m_svgPath;
}

void Stepper::backtrack(int level)
{
    while (true)
    {
        const auto step = m_eventStack.back();
        m_eventStack.pop_back();
        switch(step.type) {
            case StepType::SET:
            case StepType::BRANCH:
                if(m_graph.hasNode(step.node)) {
                    m_graph.colorNode(step.node, graphdrawer::NodeColor::UNPROCESSED);
                    m_graph.setNodeShape(step.node, 20.0, 20.0);
                }
                break;
            case StepType::LEVEL:
                if (step.level == level)
                    return;
            default:
                break;
        }
    }
}

bool Stepper::parseStep(const Step & step) {
    bool NodeColored = true;
    switch(step.type) {
        case StepType::SET:
            if(!m_graph.hasNode(step.node)) return false;
            m_graph.colorNode(step.node, step.nodeValue ? graphdrawer::NodeColor::SET_TRUE : graphdrawer::NodeColor::SET_FALSE);
            break;
        case StepType::BACKTRACK:
            backtrack(step.level);
            break;
        case StepType::BRANCH:
            if(!m_graph.hasNode(step.node)) return false;
            if(step.nodeValue) m_graph.colorNode(step.node, graphdrawer::NodeColor::BRANCH_TRUE);
            else m_graph.colorNode(step.node, graphdrawer::NodeColor::BRANCH_FALSE);
            m_graph.setNodeShape(step.node, 40.0, 40.0);
            break;
        default:
            NodeColored = false;
    }
    return NodeColored;
}

void Stepper::loadFromGML(std::string gmlPath) {
    m_graph.readGraph(gmlPath);
    m_graph.setNodeShapeAll();
    //m_graph.setStrokeWidth(0.1f);
    m_graph.colorEdges();
    m_graph.colorNodes(graphdrawer::NodeColor::UNPROCESSED);
    m_graph.layout();
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
    assert(m_tracefile.is_open());
    if(!m_tracefile.eof())
    {
        char type;
        int data;

        m_tracefile.read(&type, sizeof(type));
        m_tracefile.read(reinterpret_cast<char*>(&data), sizeof(data));

        std::cout << type << " " << data << std::endl;
        const bool value = data >= 0;

        const auto stepType = stepFromCharacter[type];
        m_eventStack.push_back({stepType, abs(data), value});

        return m_eventStack.back();
    }
    throw std::runtime_error("End of tracefile or tracefile is closed");
}

bool Stepper::isFinished() {
    return m_tracefile.eof();
}

std::string Stepper::cull(int degree) {
    if(m_lastCull == degree) {
        return m_svgPath;
    }
    if(m_lastCull < degree) {
        loadFromGML(m_gmlPath);
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
