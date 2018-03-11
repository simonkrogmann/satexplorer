#include "SatWindow.hpp"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include <QSvgRenderer>
#include <iostream>

void nameAction(QToolBar *bar, QAction *action, const char *name = nullptr) {
    if (name && action->objectName().isEmpty()) {
        action->setObjectName(name);
    }
    bar->widgetForAction(action)->setObjectName(action->objectName());
}

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

    auto nextLabel = m_toolbar.addAction("Next..");
    m_stepAction =
        m_toolbar.addAction("Step", this, &SatWindow::handleStepButton);
    m_branchAction =
        m_toolbar.addAction("Branch", this, &SatWindow::handleBranchButton);
    m_conflictAction =
        m_toolbar.addAction("Conflict", this, &SatWindow::handleConflictButton);
    m_restartAction =
        m_toolbar.addAction("Restart", this, &SatWindow::handleRestartButton);
    m_lastRestartAction = m_toolbar.addAction(
        "Last Restart", this, &SatWindow::handleLastRestartButton);
    auto zoomOutAction = m_toolbar.addAction(" - ", this, &SatWindow::zoomOut);
    auto zoomLabel = m_toolbar.addAction("Zoom");
    auto zoomInAction = m_toolbar.addAction("+", this, &SatWindow::zoomIn);
    m_toolbar.addAction("Relayout", this, &SatWindow::handleRelayoutButton);
    m_toolbar.addAction("Cluster", this, &SatWindow::cluster);
    m_toolbar.addAction("Toggle Labels", this, &SatWindow::handleLabelButton);
    m_toolbar.addAction("Import Layout", this,
                        &SatWindow::handleImportLayoutButton);
    m_toolbar.addAction("Export Image", this, &SatWindow::handleExportButton);
    m_toolbar.addAction("Show All", this, &SatWindow::handleShowAllButton);

    zoomLabel->setDisabled(true);
    nextLabel->setDisabled(true);

    nameAction(&m_toolbar, nextLabel, "label");
    nameAction(&m_toolbar, m_stepAction, "left");
    nameAction(&m_toolbar, m_branchAction, "left");
    nameAction(&m_toolbar, m_conflictAction, "left");
    nameAction(&m_toolbar, m_restartAction, "right");
    nameAction(&m_toolbar, zoomOutAction, "left");
    nameAction(&m_toolbar, zoomLabel, "left");
    nameAction(&m_toolbar, zoomInAction, "right");
    m_toolbar.setStyleSheet(R"(
        QToolBar { spacing:0px; border-width = 0px; }
        QToolButton#right, #left {
            border-width: 1px;  margin-top: 3px; margin-bottom: 3px;
            border-color: #bbbbbb; border-style: solid; padding: 1px;
            padding-left: -3px; padding-right: -3px; }
        QToolButton#left { border-right: 0px;}
        QToolButton#right:disabled, #left:disabled, #label { color: black; }
        QToolButton#right:hover, #left:hover { background: white; }
        QToolButton#right:pressed, #left:pressed { background: lightgrey; }
        )");

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

    // TODO: Button to open File dialog?
    if (m_filename == "") {
        m_filename = QFileDialog::getOpenFileName(this, "Open Image", "/home/",
                                                  "Image Files (*.cnf)")
                         .toStdString();
    }
    m_stepper.initialize(m_filename, m_solverOptions);
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

void SatWindow::handleExportButton() {
    const std::string filename = "data/exported_image.png";
    exportToPNG(filename);
    std::cout << "Created image " << filename << std::endl;
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
}

void SatWindow::handleCullInput() {
    int degree = m_cullBox.text().toInt();
    m_stepper.cull(degree);
    reloadSvg();
}

void SatWindow::handleShowAllButton() {
    m_stepper.cull(std::numeric_limits<int>::max());
    m_cullBox.clear();
    reloadSvg();
}

void SatWindow::handleLabelButton() {
    m_stepper.toggleLabelRendering();
    reloadSvg();
}

void SatWindow::handleImportLayoutButton() {
    // TODO: FileDialog, correct ending, handle cancel button
    auto filename = QFileDialog::getOpenFileName(this, "Open Layout", "/home/",
                                                 "Layout Files (*.layout)")
                        .toStdString();
    m_stepper.importLayout(filename);
    reloadSvg();
}

void SatWindow::cluster() {
    m_stepper.cluster();
    reloadSvg();
}

void SatWindow::handleRectangleDrawn(TwoPoints rectangle, bool set) {
    auto nodeColor = set ? graphdrawer::NodeColor::SET_TRUE
                         : graphdrawer::NodeColor::UNPROCESSED;
    m_stepper.colorNodesInRect(rectangle.p1.x(), rectangle.p2.x(),
                               rectangle.p1.y(), rectangle.p2.y(), nodeColor);
    reloadSvg();
}

void SatWindow::setFilename(const std::string &filename) {
    m_filename = filename;
}

void SatWindow::setSolverOptions(const SolverOptions &options) {
    m_solverOptions = options;
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
