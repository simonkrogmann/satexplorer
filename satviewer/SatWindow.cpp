#include "SatWindow.hpp"

#include <QSvgRenderer>
#include <QTimer>

SatWindow::SatWindow(QWidget*parent) : QMainWindow(parent), m_svgWidget(this), m_stepper(), m_toolbar(tr("Graph"), this) {
    setCentralWidget(&m_svgWidget);
    m_stepAction = m_toolbar.addAction("Step", this, &SatWindow::handleStepButton);
    m_branchAction = m_toolbar.addAction("Branch", this, &SatWindow::handleBranchButton);
    m_conflictAction = m_toolbar.addAction("Next conflict", this, &SatWindow::handleConflictButton);
    m_restartAction = m_toolbar.addAction("Restart", this, &SatWindow::handleRestartButton);
    m_relayoutAction = m_toolbar.addAction("Relayout", this, &SatWindow::handleRelayoutButton);
    m_toolbar.addAction("Show All", this, &SatWindow::handleShowAllButton);

    m_validator.setBottom(0);
    m_cullBox.setValidator(&m_validator);
    m_cullBox.setPlaceholderText("cull degree");
    connect(&m_cullBox, SIGNAL(returnPressed()), this, SLOT(handleCullInput()));

    m_toolbar.addWidget(&m_cullBox);

    addToolBar(Qt::BottomToolBarArea, &m_toolbar);
    setWindowTitle(tr("SatExplorer"));
}

void SatWindow::run(){
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    m_svgWidget.setPalette(pal);

    // Button to open File dialogue?
    if (m_filename == "")
    {
        m_filename = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)").toStdString();
    }
    auto svgPath = m_stepper.initialize(m_filename, m_forceSolve);
    m_svgWidget.setAutoFillBackground(true);
    m_svgWidget.renderer()->setViewBox(QRect(QPoint(0, 0), QSize(500, 500)));

    m_svgWidget.load(QString::fromStdString(svgPath));

    show();

}

void SatWindow::handleStepButton() {
    auto path = m_stepper.step();
    m_svgWidget.load(QString::fromStdString(path));
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleBranchButton() {
    auto path = m_stepper.branch();
    m_svgWidget.load(QString::fromStdString(path));
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleConflictButton() {
    auto path = m_stepper.nextConflict();
    m_svgWidget.load(QString::fromStdString(path));
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleRestartButton() {
    auto path = m_stepper.nextRestart();
    m_svgWidget.load(QString::fromStdString(path));
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::startTimer() {
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(handleStepButton()));
    timer.start(100);
}

void SatWindow::endOfTrace(bool eof) {
    if(eof) {
        m_stepAction->setDisabled(true);
        m_branchAction->setDisabled(true);
        m_conflictAction->setDisabled(true);
    }
}

void SatWindow::handleRelayoutButton(){
    auto path = m_stepper.relayout();
    m_svgWidget.load(QString::fromStdString(path));
}

void SatWindow::handleCullInput() {
    int degree = m_cullBox.text().toInt();
    auto path = m_stepper.cull(degree);
    m_svgWidget.load(QString::fromStdString(path));
}

void SatWindow::handleShowAllButton() {
    auto path = m_stepper.cull(std::numeric_limits<int>::max());
    m_cullBox.clear();
    m_svgWidget.load(QString::fromStdString(path));
}

void SatWindow::setFilename(std::string filename)
{
    m_filename = filename;
}

void SatWindow::setForceSolve(bool forceSolve){
    m_forceSolve = forceSolve;
}

