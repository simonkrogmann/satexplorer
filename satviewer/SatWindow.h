#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include "Stepper.h"

class ogdfWrapper;

class SatWindow : QSvgWidget {
public:
    void run();

protected:
    Stepper m_stepper;
};

