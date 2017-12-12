#include "Stepper.hpp"

#include <cassert>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>

#include <QProcess>
#include <QFileInfo>
#include <ogdfWrapper/types.hpp>

namespace {

std::unordered_map <char, StepType> stepFromCharacter
{
    {'<', StepType::BACKTRACK},
    {'+', StepType::SET},
    {'C', StepType::CONFLICT},
    {'>', StepType::LEVEL},
    {'B', StepType::BRANCH},
    {'R', StepType::RESTART},
    {'L', StepType::LEARNEDCLAUSE},
    {'U', StepType::UNLEARNEDCLAUSE}
};

bool shouldColor(StepType type)
{
    if (type == StepType::SET || type == StepType::BRANCH
        || type == StepType::CONFLICT) return true;
    return false;
}

}

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
    m_graph.layout();
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
    int propagateCount = 0;
    int branchCount = 0;
    int invisibleCount = 0;
    while(!m_tracefile.eof())
    {
        const auto& step = readTraceStep();
        if(step.type == StepType::BACKTRACK)
        {
            m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
            parseStep(step);
            break;
        }
        auto nodeColored = parseStep(step);
        if (step.type == StepType::SET) ++propagateCount;
        if (step.type == StepType::BRANCH) ++branchCount;
        if (shouldColor(step.type) && !nodeColored) ++invisibleCount;
    }
    std::cout << "guessed " << branchCount << " vars" <<  std::endl;
    std::cout << "propagated " << propagateCount << " vars" <<  std::endl;
    std::cout << invisibleCount << " invisible vars" <<  std::endl;
    return m_svgPath;
}

void Stepper::backtrack(int level)
{
    while (true)
    {
        const auto step = std::move(m_eventStack.back());
        m_eventStack.pop_back();
        switch(step.type) {
            case StepType::BRANCH:
                m_branchCount--;
                [[fallthrough]];
            case StepType::CONFLICT:
                [[fallthrough]];
            case StepType::SET:
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
    if (shouldColor(step.type) && !m_graph.hasNode(step.node)) return false;
    int nodeSize;
    switch(step.type) {
        case StepType::SET:
            m_graph.colorNode(step.node,
                step.nodeValue? graphdrawer::NodeColor::SET_TRUE : graphdrawer::NodeColor::SET_FALSE);
            break;
        case StepType::BACKTRACK:
            backtrack(step.level);
            break;
        case StepType::BRANCH:
            m_graph.colorNode(step.node,
                step.nodeValue ? graphdrawer::NodeColor::BRANCH_TRUE : graphdrawer::NodeColor::BRANCH_FALSE);
            nodeSize = std::max(1, 10- m_branchCount) * 10 + 40;
            m_graph.setNodeShape(step.node, nodeSize, nodeSize);
            m_branchCount++;
            break;
        case StepType::CONFLICT:
            m_graph.colorNode(step.node, graphdrawer::NodeColor::CONFLICT);
            m_graph.setNodeShape(step.node, 100, 100);
            break;
        case StepType::LEARNEDCLAUSE:
            std::cout << "add edges for new clause" <<std::endl;
            for(size_t i = 0; i < step.clauseSize; ++i) {
                if(!m_graph.hasNode(step.clause->at(i))) continue;
                for(size_t j = i + 1; j < step.clauseSize; ++j) {
                    if(!m_graph.hasNode(step.clause->at(j))) continue;
                    m_graph.addEdge(step.clause->at(i), step.clause->at(j));
                }
            }
            break;
        case StepType ::UNLEARNEDCLAUSE:
            std::cout << "remove edges of unlearned clause" << std::endl;
            for(size_t i = 0; i < step.clauseSize; ++i) {
                if(!m_graph.hasNode(step.clause->at(i))) continue;
                for(size_t j = i + 1; j < step.clauseSize; ++j) {
                    if(!m_graph.hasNode(step.clause->at(j))) continue;
                    m_graph.removeEdge(step.clause->at(i), step.clause->at(j));
                }
            }
            break;
        default:
            return false;
    }
    return true;
}

void Stepper::loadFromGML(std::string gmlPath) {
    m_branchCount = 0;
    m_graph.readGraph(gmlPath);
    m_graph.setNodeShapeAll();
    m_graph.colorEdges();
    m_graph.colorNodes(graphdrawer::NodeColor::UNPROCESSED);
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
    assert(m_tracefile.is_open() && !m_tracefile.eof());

    char type;
    int data;
    m_tracefile.read(&type, sizeof(type));
    m_tracefile.read(reinterpret_cast<char*>(&data), sizeof(data));

    std::cout << type << " " << data << std::endl;
    const bool value = data >= 0;
    const auto stepType = stepFromCharacter[type];
    m_eventStack.push_back({stepType, abs(data), value, nullptr});

    if(stepType == StepType::LEARNEDCLAUSE || stepType == StepType::UNLEARNEDCLAUSE) {
        std::cout << "read trace for new clause" <<std::endl;
        m_eventStack.back().clause = std::make_unique<std::vector<int>>();
        for(size_t i = 0; i < data; ++i) {
            int node;
            char unused;
            m_tracefile.read(reinterpret_cast<char*>(&unused), sizeof(unused));
            m_tracefile.read(reinterpret_cast<char*>(&node), sizeof(node));
            m_eventStack.back().clause->push_back(abs(node));
        }
    }

    return m_eventStack.back();
}

bool Stepper::isFinished() {
    return m_tracefile.eof();
}

std::string Stepper::cull(int degree) {
    if(m_lastCull == degree) {
        return m_svgPath;
    }
    loadFromGML(m_gmlPath);
    m_lastCull = degree;
    m_graph.removeNodes(degree, true);
    m_graph.layout();
    for(auto& step : m_eventStack)
    {
        parseStep(step);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}
