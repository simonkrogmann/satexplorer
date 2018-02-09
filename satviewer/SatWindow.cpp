#include "SatWindow.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QSvgRenderer>
#include <QTimer>
#include <iostream>

SatWindow::SatWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_svgWidget(new SelectionSvgWidget)
    , m_scrollArea(new QScrollArea)
    , m_stepper()
    , m_toolbar(tr("Graph"), this)
    , m_scaleFactor(1) {
    m_svgWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    m_scrollArea->setWidget(m_svgWidget);
    setCentralWidget(m_scrollArea);

    m_stepAction =
        m_toolbar.addAction("Step", this, &SatWindow::handleStepButton);
    m_branchAction =
        m_toolbar.addAction("Branch", this, &SatWindow::handleBranchButton);
    m_conflictAction = m_toolbar.addAction("Next conflict", this,
                                           &SatWindow::handleConflictButton);
    m_restartAction = m_toolbar.addAction("Next Restart", this,
                                          &SatWindow::handleRestartButton);
    m_lastRestartAction = m_toolbar.addAction(
        "Last Restart", this, &SatWindow::handleLastRestartButton);
    m_toolbar.addAction("Relayout", this, &SatWindow::handleRelayoutButton);
    m_toolbar.addAction("Zoom In", this, &SatWindow::zoomIn);
    m_toolbar.addAction("Zoom Out", this, &SatWindow::zoomOut);
    m_toolbar.addAction("Cluster", this, &SatWindow::cluster);
    m_toolbar.addAction("Show All", this, &SatWindow::handleShowAllButton);

    m_validator.setBottom(0);
    m_cullBox.setValidator(&m_validator);
    m_cullBox.setPlaceholderText("cull degree");
    connect(&m_cullBox, SIGNAL(returnPressed()), this, SLOT(handleCullInput()));

    m_toolbar.addWidget(&m_cullBox);

    addToolBar(Qt::BottomToolBarArea, &m_toolbar);
    setWindowTitle(tr("SatExplorer"));

    QObject::connect(m_svgWidget, SIGNAL(rectangleDrawn(TwoPoints, bool)), this,
                     SLOT(handleRectangleDrawn(TwoPoints, bool)));
}

void SatWindow::run() {
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    m_svgWidget->setPalette(pal);

    // TODO: Button to open File dialogue?
    if (m_filename == "") {
        m_filename = QFileDialog::getOpenFileName(this, "Open Image", "/home/",
                                                  "Image Files (*.cnf)")
                         .toStdString();
    }
    m_stepper.initialize(m_filename, m_forceSolve, m_showSimplified);
    m_svgWidget->setAutoFillBackground(true);
    m_svgWidget->renderer()->setViewBox(QRect(QPoint(0, 0), QSize(500, 500)));

    m_svgWidget->load(QString::fromStdString(m_stepper.getSVGPath()));
    setInitialWindowSize(m_svgWidget->sizeHint());
    m_svgWidget->resize(m_scaleFactor * m_svgWidget->sizeHint());

    show();
}

void SatWindow::handleStepButton() {
    m_stepper.step();
    reloadSvg();
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::exportToPNG(const std::string &filename) {
    QImage image(m_svgWidget->size(), QImage::Format_RGB32);
    QPainter painter;
    painter.begin(&image);
    painter.fillRect(QRect({}, image.size()), QColor(255, 255, 255));
    m_svgWidget->renderer()->render(&painter);
    painter.end();
    image.save(QString::fromStdString(filename), "PNG");
}

void SatWindow::handleBranchButton() {
    m_stepper.branch();
    reloadSvg();
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleConflictButton() {
    m_stepper.nextConflict();
    reloadSvg();
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleRestartButton() {
    m_stepper.nextRestart();
    reloadSvg();
    endOfTrace(m_stepper.isFinished());
}

void SatWindow::handleLastRestartButton() {
    m_stepper.lastRestart();
    reloadSvg();
    m_lastRestartAction->setDisabled(true);
}

void SatWindow::startTimer() {
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(handleStepButton()));
    timer.start(100);
}

void SatWindow::endOfTrace(bool eof) {
    if (eof) {
        m_stepAction->setDisabled(true);
        m_branchAction->setDisabled(true);
        m_conflictAction->setDisabled(true);
        m_lastRestartAction->setDisabled(true);
        m_restartAction->setDisabled(true);
    }
}

void SatWindow::handleRelayoutButton() {
    m_stepper.relayout();
    reloadSvg();
    setInitialWindowSize(m_svgWidget->sizeHint());
}

void SatWindow::handleCullInput() {
    int degree = m_cullBox.text().toInt();
    m_stepper.cull(degree);
    reloadSvg();
    setInitialWindowSize(m_svgWidget->sizeHint());
}

void SatWindow::handleShowAllButton() {
    m_stepper.cull(std::numeric_limits<int>::max());
    m_cullBox.clear();
    reloadSvg();
    setInitialWindowSize(m_svgWidget->sizeHint());
}

void SatWindow::cluster() {
    m_stepper.cluster();
    reloadSvg();
    setInitialWindowSize(m_svgWidget->sizeHint());
}

void SatWindow::handleRectangleDrawn(TwoPoints rectangle, bool set) {
    auto nodeColor = set ? graphdrawer::NodeColor ::SET_TRUE
                         : graphdrawer::NodeColor::UNPROCESSED;
    m_stepper.colorNodesInRect(rectangle.p1.x(), rectangle.p2.x(),
                               rectangle.p1.y(), rectangle.p2.y(), nodeColor);
    reloadSvg();
}

void SatWindow::setFilename(std::string filename) {
    m_filename = filename;
}

void SatWindow::setForceSolve(bool forceSolve) {
    m_forceSolve = forceSolve;
}

void SatWindow::setShowSimplified(bool showSimplified) {
    m_showSimplified = showSimplified;
}

void SatWindow::scaleImage(double factor) {
    m_scaleFactor *= factor;
    m_svgWidget->resize(m_scaleFactor * m_svgWidget->sizeHint());

    adjustScrollBar(m_scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(m_scrollArea->verticalScrollBar(), factor);
}

void SatWindow::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value() +
                            ((factor - 1) * scrollBar->pageStep() / 2)));
}

void SatWindow::zoomIn() {
    scaleImage(1.25);
}

void SatWindow::zoomOut() {
    scaleImage(0.8);
}

void SatWindow::reloadSvg() {
    m_svgWidget->load(QString::fromStdString(m_stepper.getSVGPath()));
    m_svgWidget->resize(m_scaleFactor * m_svgWidget->sizeHint());
}

void SatWindow::setInitialWindowSize(QSize imageSize) {
    QRect rec = QApplication::desktop()->screenGeometry();
    const auto height = rec.height();
    const auto width = rec.width();
    if (imageSize.width() > width) {
        m_scaleFactor = static_cast<double>(width - 20) / (imageSize.width());
        m_svgWidget->resize(m_scaleFactor * m_svgWidget->sizeHint());
    }
    this->resize(std::min(width, imageSize.width()),
                 std::min(height, imageSize.height()));
}
