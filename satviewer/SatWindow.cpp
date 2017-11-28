#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>

void SatWindow::run(){

    // Button to open File dialogue?
    QString cnfPath = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)");
    m_stepper.initialize(cnfPath.toStdString());

    // QString svgPath = "wherever we decide to place the newly generated svg";
    // load(svgPath);

    // button for stepping through the solving process

    show();

}



