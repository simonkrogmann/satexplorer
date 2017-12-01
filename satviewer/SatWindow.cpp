#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QPalette>
#include <QColor>
#include <QSvgRenderer>
#include <QTimer>

SatWindow::SatWindow(QWidget*parent) : QMainWindow(parent), m_svgWidget(this), m_stepper(), m_toolbar(tr("Graph"), this) {
    setCentralWidget(&m_svgWidget);
    m_toolbar.addAction("Step", this, &SatWindow::handleStepButton);
    m_toolbar.addAction("Branch", this, &SatWindow::handleBranchButton);
    addToolBar(Qt::BottomToolBarArea, &m_toolbar);
    setWindowTitle(tr("SatExplorer"));
}

void SatWindow::run(){
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    m_svgWidget.setPalette(pal);

    // Button to open File dialogue?
    QString cnfPath = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)");
    auto svgPath = m_stepper.initialize(cnfPath.toStdString());
    m_svgWidget.setAutoFillBackground(true);
    m_svgWidget.renderer()->setViewBox(QRect(QPoint(0, 0), QSize(500, 500)));

    m_svgWidget.load(QString::fromStdString(svgPath));

    show();

}

void SatWindow::handleStepButton() {
    auto path = m_stepper.step();
    m_svgWidget.load(QString::fromStdString(path));
}
void SatWindow::handleBranchButton() {
    auto path = m_stepper.branch();
    m_svgWidget.load(QString::fromStdString(path));
}

void SatWindow::startTimer() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(handleStepButton()));
    timer->start(100);
}



