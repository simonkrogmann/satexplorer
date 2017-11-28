#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QPalette>
#include <QColor>
#include <QSvgRenderer>

SatWindow::SatWindow(QWidget*parent) : QMainWindow(parent), m_svgWidget(this), m_stepper(), m_stepButton("Step", this), m_dockWidget(tr("Graph"), this) {
    setCentralWidget(&m_svgWidget);
    m_dockWidget.setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea);
    m_dockWidget.setWidget(&m_stepButton);
    addDockWidget(Qt::BottomDockWidgetArea, &m_dockWidget);
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

    // button for stepping through the solving process
    m_stepButton.setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));
    connect(&m_stepButton, SIGNAL (clicked()), this, SLOT (handleStepButton()));

    show();

}

void SatWindow::handleStepButton() {
    auto path = m_stepper.step();
    m_svgWidget.load(QString::fromStdString(path));
}



