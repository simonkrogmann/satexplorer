#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>

void SatWindow::run(){

    // Button to open File dialogue?
    QString cnfPath = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)");
    auto svgPath = m_stepper.initialize(cnfPath.toStdString());
    autoFillBackground();

    load(QString::fromStdString(svgPath));

    // button for stepping through the solving process

    show();

}



