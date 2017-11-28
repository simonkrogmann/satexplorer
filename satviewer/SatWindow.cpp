#include "SatWindow.hpp"

#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QProcess>
#include <QPalette>
#include <QColor>

void SatWindow::run(){
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    this->setPalette(pal);

    // Button to open File dialogue?
    QString cnfPath = QFileDialog::getOpenFileName(this, "Open Image", "/home/", "Image Files (*.cnf)");
    auto svgPath = m_stepper.initialize(cnfPath.toStdString());
    setAutoFillBackground(true);

    load(QString::fromStdString(svgPath));

    // button for stepping through the solving process

    show();

}



