#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QMainWindow>
#include "Stepper.hpp"

class ogdfWrapper;

class SatWindow : public QMainWindow {
    Q_OBJECT
public:
    void run();

    explicit SatWindow(QWidget * parent = 0);
    virtual ~SatWindow() = default;

protected:
    QSvgWidget m_svgWidget;
    Stepper m_stepper;
    QPushButton m_stepButton;

private slots:
    void handleStepButton();
};

