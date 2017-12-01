#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QMainWindow>
#include <QToolBar>
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
    QToolBar m_toolbar;

private slots:
    void handleStepButton();
    void handleBranchButton();
    void startTimer();
};

