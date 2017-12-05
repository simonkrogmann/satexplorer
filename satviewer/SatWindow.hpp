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

protected:
    QSvgWidget m_svgWidget;
    Stepper m_stepper;
    QToolBar m_toolbar;

    void endOfTrace(bool eof);
    // These pointers belong to the QToolbar and are its responsibility
    QAction* m_stepAction;
    QAction* m_branchAction;
    std::string m_filename;

    QLineEdit m_cullBox;
    QIntValidator m_validator;

private slots:
    void handleStepButton();
    void handleBranchButton();
    void handleCullInput();
    void handleShowAllButton();
    void startTimer();
};

