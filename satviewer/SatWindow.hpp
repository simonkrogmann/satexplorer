#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include "Stepper.hpp"

class ogdfWrapper;

class SatWindow : public QSvgWidget {
public:
    void run();

protected:
    Stepper m_stepper;
};

