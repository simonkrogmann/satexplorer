#pragma once

#include <QApplication>
#include <QIntValidator>
#include <QLineEdit>
#include <QMainWindow>
#include <QToolBar>

#include "Stepper.hpp"

class ogdfWrapper;
class QLineEdit;
class QIntValidator;
class QScrollArea;
class QScrollBar;
class QSvgWidget;

class SatWindow : public QMainWindow {
    Q_OBJECT
public:
    void run();

    explicit SatWindow(QWidget* parent = 0);
    virtual ~SatWindow() = default;

    void setFilename(std::string filename);
    void setForceSolve(bool forceSolve);

protected:
    QSvgWidget* m_svgWidget;
    QScrollArea* m_scrollArea;
    Stepper m_stepper;
    QToolBar m_toolbar;
    double m_scaleFactor;

    void endOfTrace(bool eof);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar* scrollBar, double factor);
    void reloadSvg();
    void setInitialWindowSize(QSize imageSize);
    // These pointers belong to the QToolbar and are its responsibility
    QAction* m_stepAction;
    QAction* m_conflictAction;
    QAction* m_branchAction;
    QAction* m_restartAction;
    QAction* m_lastRestartAction;
    QAction* m_relayoutAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
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
    void zoomIn();
    void zoomOut();
};
