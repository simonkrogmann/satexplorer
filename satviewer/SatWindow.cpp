#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QPalette>
#include <QColor>

SatWindow::SatWindow(QWidget*parent) : QMainWindow(parent), m_svgWidget(this), m_stepper(), m_stepButton("Step", this) {}

void SatWindow::run(){
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    m_svgWidget.setPalette(pal);

    // Button to open File dialogue?
    QString cnfPath = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)");
    auto svgPath = m_stepper.initialize(cnfPath.toStdString());
    m_svgWidget.setAutoFillBackground(true);

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



