#include "Stepper.hpp"

#include <sys/stat.h>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

#include <QFileInfo>
#include <QProcess>
#include <ogdfWrapper/types.hpp>

namespace {
std::unordered_map<char, StepType> stepFromCharacter{
    {'<', StepType::BACKTRACK},     {'+', StepType::SET},
    {'C', StepType::CONFLICT},      {'>', StepType::LEVEL},
    {'B', StepType::BRANCH},        {'R', StepType::RESTART},
    {'L', StepType::LEARNEDCLAUSE}, {'U', StepType::UNLEARNEDCLAUSE}};

bool shouldColor(StepType type) {
    return type == StepType::SET || type == StepType::BRANCH ||
           type == StepType::CONFLICT;
}

bool isClause(StepType type) {
    return type == StepType::LEARNEDCLAUSE || type == StepType::UNLEARNEDCLAUSE;
}

long getFileSize(std::string filename) {
    struct stat64 stat_buf;
    const int return_value = stat64(filename.c_str(), &stat_buf);
    return return_value == 0 ? stat_buf.st_size : -1;
}

bool isEOF(std::ifstream& file) {
    const bool eof = file.peek() == std::ifstream::traits_type::eof();
    if (eof) std::cout << "end of file" << std::endl;
    return eof;
}

}  // namespace

void Stepper::initialize(std::string cnfPath, SolverOptions options) {
    m_lastCull = std::numeric_limits<int>::max();

    // calculate file paths based on cnf file path
    const auto cnfFilename =
        QFileInfo(QString::fromStdString(cnfPath)).baseName().toStdString();
    const auto solutionPath = outputPath + cnfFilename + ".solution";
    const auto tracePath = outputPath + cnfFilename + ".trace";
    const auto baseOutputname = outputPath + cnfFilename;
    m_svgPath = outputPath + cnfFilename + ".svg";
    m_clusterPath = outputPath + cnfFilename + ".clustering";

    // create output directory
    system(("mkdir -p " + outputPath).c_str());
    QProcess process;

    // run minisat, if instance not solved yet
    if (!std::ifstream(tracePath) || options.forceRecomputation) {
        auto satCmd = minisat + " " + cnfPath + " " + baseOutputname;
        std::cout << "Solving SAT instance..." << std::endl
                  << satCmd << std::endl;
        process.start(QString::fromStdString(satCmd));
        process.waitForFinished(-1);
        std::cout << "Done." << std::endl;
    }

    m_gmlPath = outputPath + cnfFilename;
    if (options.simplified) {
        cnfPath = outputPath + cnfFilename + ".simplified";
        m_gmlPath += ".simplified";
    }
    if (options.onlyImplications) {
        m_gmlPath += ".implications";
    }
    m_gmlPath += ".gml";

    // convert cnf to gml via python script, if not converted yet
    if (!std::ifstream(m_gmlPath) || options.forceRecomputation) {
        auto conversionCmd =
            scriptPath + conversionScript + " " + cnfPath + " " + m_gmlPath;
        if (options.onlyImplications) {
            conversionCmd += " -implications";
        }
        std::cout << "Converting to gml for layouting..." << std::endl
                  << conversionCmd << std::endl;
        process.start(QString::fromStdString(conversionCmd));
        process.waitForFinished(-1);
        std::cout << "Done." << std::endl;
    }

    readTrace(tracePath);
    loadFromGML(m_gmlPath);
    m_graph.setLayoutType(graphdrawer::LayoutType::FMMM);
    relayout();
}

void Stepper::relayout() {
    m_graph.layout();
    for (const auto & [ nodeID, color ] : m_coloredNodes) {
        m_graph.colorNode(nodeID, color);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
}

void Stepper::step() {
    bool stepFinished = false;
    while (!isEOF(m_tracefile) && !stepFinished) {
        const auto& type = readTraceStep();
        if (isClause(type)) {
            applyClause();
            break;
        }
        stepFinished = applyStep();
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
    printProgress();
}

void Stepper::stepUntil(StepType finalType, bool layout) {
    int propagateCount = 0;
    int branchCount = 0;
    int invisibleCount = 0;
    int newClauseCount = 0;
    int deletedClauseCount = 0;
    while (!isEOF(m_tracefile)) {
        const auto type = readTraceStep();
        if ((type == finalType || isEOF(m_tracefile)) &&
            type == StepType::BACKTRACK) {
            m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
            applyStep();
            break;
        }
        if (isClause(type)) {
            applyClause();
            if (type == StepType::LEARNEDCLAUSE) ++newClauseCount;
            if (type == StepType::UNLEARNEDCLAUSE) ++deletedClauseCount;
        } else {
            const auto nodeColored = applyStep();
            if (type == StepType::SET) ++propagateCount;
            if (type == StepType::BRANCH) ++branchCount;
            if (shouldColor(type) && !nodeColored) ++invisibleCount;
        }
        if (type == finalType) {
            if (layout)
                m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
            break;
        }
    }
    std::cout << "guessed " << branchCount << " vars" << std::endl;
    std::cout << "propagated " << propagateCount << " vars" << std::endl;
    std::cout << invisibleCount << " invisible vars" << std::endl;
    std::cout << newClauseCount << " new clauses" << std::endl;
    std::cout << deletedClauseCount << " clauses deleted" << std::endl;
    printProgress();
}

void Stepper::printProgress() {
    std::cout << "Trace progress: " << std::fixed << std::setprecision(2)
              << m_readBlocks / (m_tracefileSize / 5.) * 100 << "%"
              << std::endl;
}

void Stepper::branch() {
    stepUntil(StepType::BRANCH, true);
}

void Stepper::nextConflict() {
    stepUntil(StepType::BACKTRACK, true);
}

void Stepper::nextRestart() {
    stepUntil(StepType::RESTART, true);
    if (m_eventStack.back().type == StepType::RESTART) {
        std::cout << "Restart " << m_eventStack.back().numberOfRestarts
                  << std::endl;
    } else {
        std::cout << "Restart " << m_numberOfRestarts << std::endl;
    }
}

void Stepper::lastRestart() {
    for (int i = 0; i < m_numberOfRestarts; ++i) {
        stepUntil(StepType::RESTART, false);
        assert(!isEOF(m_tracefile));
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
}

void Stepper::backtrack(int level) {
    if (level == 0 && m_currentLevel == 0) {
        assert(m_eventStack.back().type == StepType::BACKTRACK);
        m_eventStack.pop_back();
        return;
    }
    while (true) {
        assert(!m_eventStack.empty());
        const auto step = m_eventStack.back();
        m_eventStack.pop_back();
        assert(step.type != StepType::RESTART || level == 0);
        switch (step.type) {
            case StepType::BRANCH:
                m_branchCount--;
                [[fallthrough]];
            case StepType::CONFLICT:
                [[fallthrough]];
            case StepType::SET:
                if (m_graph.hasNode(step.nodeID())) {
                    m_graph.colorNode(step.nodeID(),
                                      graphdrawer::NodeColor::UNPROCESSED);
                    m_graph.setNodeShape(step.nodeID(), 20.0, 20.0);
                    m_graph.setZ(step.nodeID(), 0);
                }
                break;
            case StepType::LEVEL:
                assert(step.level > level);
                if (step.level == level + 1) return;
            default:
                break;
        }
    }
}

void Stepper::applyClause(int i) {
    if (i == -1) i = m_learnedClauses.size() - 1;
    const auto& clause = m_learnedClauses.back();
    if (clause.type == StepType::LEARNEDCLAUSE) {
        m_graph.addNode(clause.nodeID());
        m_graph.moveToCenter(clause.nodeID(), clause.variables);
        for (size_t i = 0; i < clause.variables.size(); ++i) {
            if (m_graph.hasNode(
                    {clause.variables[i], graphdrawer::NodeType::LITERAL})) {
                m_graph.addEdge(
                    clause.nodeID(),
                    {clause.variables[i], graphdrawer::NodeType::LITERAL});
            }
        }
    } else if (clause.type == StepType::UNLEARNEDCLAUSE) {
        m_graph.removeNode(clause.nodeID());
    } else {
        assert(false);
    }
}

bool Stepper::applyStep(int i) {
    if (i == -1) i = m_eventStack.size() - 1;
    const auto& step = m_eventStack[i];
    if (step.type == StepType::BRANCH) {
        m_branchCount++;
    }
    if (shouldColor(step.type) && !m_graph.hasNode(step.nodeID())) return false;
    int nodeSize;
    switch (step.type) {
        case StepType::SET:
            m_graph.colorNode(step.nodeID(),
                              step.nodeValue
                                  ? graphdrawer::NodeColor::SET_TRUE
                                  : graphdrawer::NodeColor::SET_FALSE);
            m_graph.setZ(step.nodeID(), 1);
            break;
        case StepType::BACKTRACK:
            assert(step.level < m_currentLevel || step.level == 0);
            backtrack(step.level);
            m_currentLevel = step.level;
            break;
        case StepType::BRANCH:
            m_graph.colorNode(step.nodeID(),
                              step.nodeValue
                                  ? graphdrawer::NodeColor::BRANCH_TRUE
                                  : graphdrawer::NodeColor::BRANCH_FALSE);
            nodeSize = std::max(1, 10 - m_branchCount) * 5 + 25;
            assert(m_branchCount >= 0);
            m_graph.setNodeShape(step.nodeID(), nodeSize, nodeSize);
            m_graph.setZ(step.nodeID(), m_branchCount + 1);
            break;
        case StepType::CONFLICT:
            m_graph.colorNode(step.nodeID(), graphdrawer::NodeColor::CONFLICT);
            m_graph.setNodeShape(step.nodeID(), 100, 100);
            m_graph.setZ(step.nodeID(), m_branchCount + 2);
            break;
        case StepType::LEVEL:
            assert(step.level == m_currentLevel + 1);
            m_currentLevel = step.level;
            [[fallthrough]];
        default:
            return false;
    }
    return true;
}

void Stepper::loadFromGML(const std::string& gmlPath) {
    m_branchCount = 0;
    m_graph.readGraph(gmlPath);
    m_graph.resetNodeShapeAll();
    m_graph.resetEdgeStyleAll();
    m_graph.colorAllNodes(graphdrawer::NodeColor::UNPROCESSED);
}

// opens the tracefile
void Stepper::readTrace(const std::string& tracePath) {
    m_tracefileSize = getFileSize(tracePath);
    m_readBlocks = 0;
    m_tracefile.open(tracePath, std::ios::binary | std::ios::in);
    int headerSize;
    m_tracefile.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    m_tracefile.read(reinterpret_cast<char*>(&m_numberOfRestarts),
                     sizeof(m_numberOfRestarts));
    m_tracefileSize -= headerSize;
    m_tracefile.seekg(headerSize);
    std::cout << "header: " << headerSize << std::endl;
    std::cout << "restarts: " << m_numberOfRestarts << std::endl;
    assert(m_tracefileSize % 5 == 0);
    assert(m_tracefile.is_open());
}

// tracefile is encoded in binary
// structure is char followed by int
// char denotes StepType, int denotes Node
void Stepper::readBlock(char& type, int& data) {
    m_tracefile.read(&type, sizeof(type));
    m_tracefile.read(reinterpret_cast<char*>(&data), sizeof(data));
    ++m_readBlocks;
}

StepType Stepper::readTraceStep() {
    assert(m_tracefile.is_open() && !m_tracefile.eof());

    char type;
    int32_t data;
    readBlock(type, data);

    // std::cout << type << " " << data << std::endl;
    const bool value = data >= 0;
    const auto stepType = stepFromCharacter[type];
    if (stepType == StepType::LEARNEDCLAUSE) {
        // when we encounter a Clause, first block tells us:
        // type = Clause
        // data = ID of Clause Node
        assert(data >= 0);
        m_learnedClauses.push_back({stepType, static_cast<uint32_t>(data), {}});
        int32_t length;
        char flag;
        readBlock(flag, length);
        assert(flag == 'S');
        // second block denotes
        // type = "S" Start
        // data = number of literals in the clause
        for (size_t i = 0; i < length; ++i) {
            // read literal info
            // node = literal node ID
            int32_t node;
            readBlock(flag, node);
            assert(flag == 'x');
            m_learnedClauses.back().variables.push_back(
                static_cast<uint32_t>(abs(node)));
        }
    } else if (stepType == StepType::UNLEARNEDCLAUSE) {
        assert(data >= 0);
        m_learnedClauses.push_back({stepType, static_cast<uint32_t>(data), {}});
        // TODO: clean clauses from m_learnedClauses
    } else {
        m_eventStack.push_back(
            {stepType, static_cast<uint32_t>(abs(data)), value});
    }

    return stepType;
}

bool Stepper::isFinished() {
    return isEOF(m_tracefile);
}

void Stepper::cull(int degree) {
    if (m_lastCull == degree) {
        return;
    }
    loadFromGML(m_gmlPath);
    m_lastCull = degree;
    m_graph.removeNodes(degree, true);
    m_graph.layout();
    m_currentLevel = 0;
    for (int i = 0; i < m_eventStack.size(); ++i) {
        applyStep(i);
    }
    for (int i = 0; i < m_learnedClauses.size(); ++i) {
        applyClause(i);
    }
    for (const auto & [ nodeID, color ] : m_coloredNodes) {
        m_graph.colorNode(nodeID, color);
    }
    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
}

const std::string Stepper::getSVGPath() const {
    return m_svgPath;
}

void Stepper::colorNodesInRect(float xMin, float xMax, float yMin, float yMax,
                               graphdrawer::NodeColor color) {
    for (const auto node : m_graph.nodesInRectangle(xMin, xMax, yMin, yMax)) {
        m_graph.colorNode(node, color);
        m_coloredNodes[node] = color;
    }

    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
}

// String splitting copied from StackOverflow
// https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void Stepper::cluster() {
    QProcess process;
    std::string clusterCmd = "python " + scriptPath + clusteringScript + " " +
                             m_gmlPath + " " + m_clusterPath;
    std::cout << "Clustering graph..." << std::endl << clusterCmd << std::endl;
    process.start(QString::fromStdString(clusterCmd));
    process.waitForFinished(-1);
    std::cout << "Done." << std::endl;

    std::cout << "Loading clustering";
    std::unordered_map<int, std::vector<graphdrawer::NodeID>> cluster_node_map;
    std::ifstream clustering_file(m_clusterPath);
    std::string line;
    while (std::getline(clustering_file, line)) {
        auto line_split = split(line, ' ');
        assert(line_split.size() == 2);

        auto node_type = line_split[0][0];
        auto node_id_nr = std::stoi(line_split[0].substr(1));
        auto node_id = graphdrawer::NodeID::fromCharType(node_type, node_id_nr);
        int cluster_id = std::stoi(line_split[1]);

        if (!cluster_node_map.count(cluster_id)) {
            cluster_node_map.emplace(cluster_id,
                                     std::vector<graphdrawer::NodeID>());
        }

        cluster_node_map[cluster_id].push_back(node_id);
    }
    std::cout << "...done" << std::endl;
    std::cout << "Applying clustering";
    for (auto[cluster_id, node_ids] : cluster_node_map) {
        const auto color =
            graphdrawer::NodeColor(cluster_id % graphdrawer::numNodeColors);
        m_graph.colorNodes(color, node_ids);
        for (auto node_id : node_ids) {
            m_coloredNodes[node_id] = color;
        }
    }
    std::cout << "...done" << std::endl;

    m_graph.writeGraph(m_svgPath, graphdrawer::FileType::SVG);
}
