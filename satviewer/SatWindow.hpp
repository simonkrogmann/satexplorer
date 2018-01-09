#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>
#include <QMainWindow>
#include <QToolBar>
#include <QLineEdit>
#include <QIntValidator>
#include "Stepper.hpp"

class ogdfWrapper;
class QLineEdit;
class QIntValidator;

class SatWindow : public QMainWindow {
    Q_OBJECT
public:
    void run();

    explicit SatWindow(QWidget * parent = 0);
    virtual ~SatWindow() = default;

    void setFilename(std::string filename);
    void setForceSolve(bool forceSolve);

protected:
    QSvgWidget m_svgWidget;
    Stepper m_stepper;
    QToolBar m_toolbar;

    void endOfTrace(bool eof);
    // These pointers belong to the QToolbar and are its responsibility
    QAction* m_stepAction;
    QAction* m_conflictAction;
    QAction* m_branchAction;
    QAction* m_restartAction;
    QAction* m_lastRestartAction;
    QAction* m_relayoutAction;
    std::string m_filename;
    bool m_forceSolve;

    QLineEdit m_cullBox;
    QIntValidator m_validator;

private slots:
    void handleStepButton();
    void handleBranchButton();
    void handleConflictButton();
    void handleRestartButton();
    void handleLastRestartButton();
    void handleCullInput();
    void handleRelayoutButton();
    void handleShowAllButton();
    void startTimer();
};

