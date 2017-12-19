#include "Stepper.hpp"

#include <cassert>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <utility>
#include <sys/stat.h>

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

long getFileSize(std::string filename)
{
    struct stat64 stat_buf;
    int rc = stat64(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

bool isEOF(std::ifstream & file)
{
    return file.peek() == std::ifstream::traits_type::eof();
}

} // namespace anonymous

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
    m_graph.setLayoutType(graphdrawer::LayoutType::FMMM);
    m_graph.layout();
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);

    return m_svgPath;
}

std::string Stepper::relayout() {
    m_graph.layout();
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}

std::string Stepper::step() {
    bool stepFinished = false;
    while(!isEOF(m_tracefile) && !stepFinished) {
        const auto& type = readTraceStep();
        if (type == StepType::LEARNEDCLAUSE || type == StepType::UNLEARNEDCLAUSE)
        {
            applyClause();
            break;
        }
        stepFinished = applyStep();
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    printProgress();
    return m_svgPath;
}

void Stepper::stepUntil(StepType finalType) {
    int propagateCount = 0;
    int branchCount = 0;
    int invisibleCount = 0;
    int newClauseCount = 0;
    int deletedClauseCount = 0;
    while(!isEOF(m_tracefile))
    {
        auto type = readTraceStep();
        if((type == finalType || isEOF(m_tracefile)) && type == StepType::BACKTRACK)
        {
            m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
            applyStep();
            break;
        }
        if (type == StepType::LEARNEDCLAUSE || type == StepType::UNLEARNEDCLAUSE)
        {
            applyClause();
            if (type == StepType::LEARNEDCLAUSE) ++newClauseCount;
            if (type == StepType::UNLEARNEDCLAUSE) ++deletedClauseCount;
        }
        else
        {
            auto nodeColored = applyStep();
            if (type == StepType::SET) ++propagateCount;
            if (type == StepType::BRANCH) ++branchCount;
            if (shouldColor(type) && !nodeColored) ++invisibleCount;
        }
        if(type == finalType)
        {
            m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
            break;
        }
    }
    std::cout << "guessed " << branchCount << " vars" <<  std::endl;
    std::cout << "propagated " << propagateCount << " vars" <<  std::endl;
    std::cout << invisibleCount << " invisible vars" <<  std::endl;
    std::cout << newClauseCount << " new clauses" <<  std::endl;
    std::cout << deletedClauseCount << " clauses deleted" <<  std::endl;
    printProgress();
}

void Stepper::printProgress()
{
    std::cout << "Trace progress: " << std::fixed << std::setprecision(2)
        << m_readBlocks / (m_tracefileSize / 5.) * 100 << "%" << std::endl;
}

std::string Stepper::branch()
{
    stepUntil(StepType::BRANCH);
    return m_svgPath;
}

std::string Stepper::nextConflict(){
    stepUntil(StepType::BACKTRACK);
    return m_svgPath;
}

std::string Stepper::nextRestart(){
    for (int i = 0; i < 1; ++i)
    {
        stepUntil(StepType::RESTART);
        std::cout << "Restart " << m_eventStack.back().numberOfRestarts << std::endl;
    }
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
                if(m_graph.hasNode(step.nodeID())) {
                    m_graph.colorNode(step.nodeID(), graphdrawer::NodeColor::UNPROCESSED);
                    m_graph.setNodeShape(step.nodeID(), 20.0, 20.0);
                }
                m_graph.setZ(step.nodeID(), 0);
                break;
            case StepType::LEVEL:
                if (step.level == level)
                    return;
            default:
                break;
        }
    }
}

void Stepper::applyClause(int i)
{
    if (i == -1) i = m_learnedClauses.size() - 1;
    const auto & clause = m_learnedClauses.back();
    if (clause.type == StepType::LEARNEDCLAUSE)
    {
        m_graph.addNode(clause.nodeID());
        m_graph.moveToCenter(clause.nodeID(), clause.variables);
        for(size_t i = 0; i < clause.variables.size(); ++i) {
            if(m_graph.hasNode({clause.variables[i], graphdrawer::NodeType::LITERAL})) {
                m_graph.addEdge(clause.nodeID(), {clause.variables[i], graphdrawer::NodeType::LITERAL});
            }
        }
    }
    else if (clause.type == StepType::UNLEARNEDCLAUSE)
    {
        m_graph.removeNode(clause.nodeID());
    }
    else
    {
        assert(false);
    }
}

bool Stepper::applyStep(int i) {
    if (i == -1) i = m_eventStack.size() - 1;
    const auto & step = m_eventStack[i];
    if (shouldColor(step.type) && !m_graph.hasNode(step.nodeID())) return false;
    int nodeSize;
    switch(step.type) {
        case StepType::SET:
            m_graph.colorNode(step.nodeID(),
                step.nodeValue? graphdrawer::NodeColor::SET_TRUE : graphdrawer::NodeColor::SET_FALSE);
                m_graph.setZ(step.nodeID(), 1);
            break;
        case StepType::BACKTRACK:
            backtrack(step.level);
            break;
        case StepType::BRANCH:
            m_graph.colorNode(step.nodeID(),
                step.nodeValue ? graphdrawer::NodeColor::BRANCH_TRUE : graphdrawer::NodeColor::BRANCH_FALSE);
            nodeSize = std::max(1, 10- m_branchCount) * 5 + 25;
            m_graph.setNodeShape(step.nodeID(), nodeSize, nodeSize);
            m_branchCount++;
            m_graph.setZ(step.nodeID(), m_branchCount + 1);
            break;
        case StepType::CONFLICT:
            m_graph.colorNode(step.nodeID(), graphdrawer::NodeColor::CONFLICT);
            m_graph.setNodeShape(step.nodeID(), 100, 100);
            m_graph.setZ(step.nodeID(), m_branchCount + 2);
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
    m_tracefileSize = getFileSize(tracePath);
    m_readBlocks = 0;
    m_tracefile.open(tracePath, std::ios::binary | std::ios::in);
    assert(m_tracefileSize % 5 == 0);
    assert(m_tracefile.is_open());
}


// tracefile is encoded in binary
// structure is char followed by int
// char denotes StepType, int denotes Node
void Stepper::readBlock(char & type, int & data)
{
    m_tracefile.read(&type, sizeof(type));
    m_tracefile.read(reinterpret_cast<char*>(&data), sizeof(data));
    ++m_readBlocks;
}

StepType Stepper::readTraceStep()
{
    assert(m_tracefile.is_open() && !m_tracefile.eof());

    char type;
    int data;
    readBlock(type, data);

    // std::cout << type << " " << data << std::endl;
    const bool value = data >= 0;
    const auto stepType = stepFromCharacter[type];
    if(stepType == StepType::LEARNEDCLAUSE || stepType == StepType::UNLEARNEDCLAUSE)
    {
        // TODO: make it so every learned clause has a unique identifier within the tracefile
        assert(data >= 0);
        m_learnedClauses.push_back({stepType, static_cast<uint>(data), {}});
        int length;
        char unused;
        readBlock(unused, length);
        for(size_t i = 0; i < length; ++i)
        {
            int node;
            readBlock(unused, node);
            m_learnedClauses.back().variables.push_back(static_cast<uint>(abs(node)));
        }
    }
    else
    {
        m_eventStack.push_back({stepType, static_cast<uint>(abs(data)), value});
    }

    return stepType;
}

bool Stepper::isFinished() {
    return isEOF(m_tracefile);
}

std::string Stepper::cull(int degree) {
    if(m_lastCull == degree) {
        return m_svgPath;
    }
    loadFromGML(m_gmlPath);
    m_lastCull = degree;
    m_graph.removeNodes(degree, true);
    m_graph.layout();
    for (int i = 0; i < m_eventStack.size(); ++i)
    {
        applyStep(i);
    }
    for (int i = 0; i < m_learnedClauses.size(); ++i)
    {
        applyClause(i);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::filetype::SVG);
    return m_svgPath;
}
